#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rmemory.h"
#include "logging.h"
#include <syscall.h>
#include <logging.h>

/**
 * logs a string to the console
 * @param message the message that will be logged
 */
void log_to_console(const char * message){
    printf("%s", message);
    // flush buffer
    fflush(stdout);
}

/**
 * logs a string to a file. The string will be appended
 * @param message the message that will be logged
 * @param log_file the file where the @p message is appended
 */
void log_to_file(const char * message, const char * log_file){
    // check if path to log file has been set
    if (log_file == NULL){
        perror("Log file not specified\n");
    }

    // open file in append mode
    FILE * pFile = fopen(log_file, "a");

    if (pFile == NULL){
        // error while opening file
        perror("Could not open log file\n");
    }

    // write log message to file
    fprintf(pFile, "%s", message);

    fclose(pFile);
}

/**
 * generates a log message string from given parameters. Uses LOG_FORMAT as template for formatting
 * @param max_log_level the maximum log level of the logger
 * @param level the log level of the message to log
 * @param location the location the log message occurred
 * @param msg_str the log message
 * @return the log message string
 */
char * get_log_message_string(log_level max_log_level, log_level level, char * location, char * msg_str){

    // check if maximum log level allows this message
    if (level > max_log_level){
        // not allowed, return
        return NULL;
    }

    // generate timestamp
    time_t current_time = time(NULL);
    struct tm tt;
    struct tm * time_info = localtime_r(&current_time, &tt);
    char timestamp[30];
    char timestamp2[60];

    // ms since 1.1.1970
    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
            (unsigned long long)(tv.tv_sec) * 1000 +
            (unsigned long long)(tv.tv_usec) / 1000;


    // generate log level string
    char level_str[30];
    switch (level){
        case LOG_LEVEL_DEBUG:
            rstrcpy(level_str, "DEBUG");
            break;
        case LOG_LEVEL_ERROR:
            rstrcpy(level_str, "ERROR");
            break;
        case LOG_LEVEL_INFO:
            rstrcpy(level_str, "INFO");
            break;
        default:
            perror("invalid log level\n");
    }

    // format timestamp
    strftime(timestamp, sizeof(timestamp), "%x|%X", time_info);

    // add milliseconds to timestamp
    sprintf(timestamp2, "%s (Epoch time: %llu)", timestamp, millisecondsSinceEpoch);

    char *  msg_string = rmalloc(LOGGER_MAX_MSG_SIZE);
    sprintf(msg_string, LOG_FORMAT, timestamp2, level_str, location, msg_str);

    return msg_string;
}

struct logger_t logger_init(log_level max_log_level, logger_type type){
    struct logger_t logger;

    logger.type = type;
    logger.max_log_level = max_log_level;
    logger.log_file = NULL;

    // init the buffer FIFO
    logger.buffer = fifo_init(LOGGER_BUFFER_SIZE);

    return logger;
}

void logger_set_log_file(struct logger_t* logger, char * path){
    logger->log_file = path;
}

void logger_log(struct logger_t * logger, log_level level, char* location, char* format, ...){
    if (logger == NULL || logger->max_log_level == LOG_LEVEL_NONE){
        return;
    }

    char message[LOGGER_MAX_MSG_SIZE / 2];
    va_list args;
    va_start(args, format);

    vsprintf(&message[0], format, args);
    va_end(args);

    log_level max_lvl = logger->max_log_level;
    char * msg = get_log_message_string(max_lvl, level, location, message);
    if (msg == NULL){
        // log level to low
        return;
    }
    if (msg != NULL){
        logger_type type = logger->type;
        char * file = logger->log_file;

        if (type == LOGGER_TYPE_CONSOLE){
            // log to console
            log_to_console(msg);
        } else if (type == LOGGER_TYPE_FILE){
            // log to file
            log_to_file(msg, file);
        } else if (type == LOGGER_TYPE_BOTH){
            // log to console and file
            log_to_console(msg);
            log_to_file(msg, file);
        }
    }
}

void logger_log_if(struct logger_t * logger, int cond, log_level level, char * location, char * format, ...){
    if (!cond){
        // condition false -> nothing to log
        return;
    }

    if (logger == NULL || logger->max_log_level == LOG_LEVEL_NONE){
        return;
    }

    char message[LOGGER_MAX_MSG_SIZE / 2];
    va_list args;
    va_start(args, format);

    vsprintf(&message[0], format, args);
    va_end(args);

    log_level max_lvl = logger->max_log_level;

    char * msg = get_log_message_string(max_lvl, level, location, message);
    if (msg == NULL){
        // log level to low
        return;
    }

    // add message string to the write buffer
    fifo_push(logger->buffer, msg);
}

void logger_destroy(struct logger_t * logger){

    // free all remaining log messages
    char * elem;
    while ((elem = fifo_pop(logger->buffer)) != NULL) {
        rfree(elem);
    }

    fifo_destroy(logger->buffer);
    rfree(logger->wrapper_ptr);
}
