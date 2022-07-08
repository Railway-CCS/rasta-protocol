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

/**
 * thread handle for the write thread
 * @param logger_ptr pointer to the logger
 * @return unused / NULL
 */
void * write_log_messages(void * logger_ptr){
    // enable possibility to cancel thread
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    write_thread_parameter_wrapper * wrapper = (write_thread_parameter_wrapper *)logger_ptr;

    while (1){

        char * message = fifo_pop(wrapper->buffer);

        if (message != NULL){
            logger_type type = wrapper->type;
            char * file = wrapper->log_file;

            if (type == LOGGER_TYPE_CONSOLE){
                // log to console
                log_to_console(message);
            } else if (type == LOGGER_TYPE_FILE){
                // log to file
                log_to_file(message, file);
            } else if (type == LOGGER_TYPE_BOTH){
                // log to console and file
                log_to_console(message);
                log_to_file(message, file);
            }

            //rfree(message);
        }

        if (fifo_get_size(wrapper->buffer) == 0){
            usleep(100000);
        } else{
            // to avoid to much CPU utilization, force context switch by sleeping for 0ns
            nanosleep((const struct timespec[]){{0, 0L}}, NULL);
        }

        rfree(message);
    }
}

/**
 * creates and starts the write thread
 * @param logger the logger whose write thread is started
 */
void start_writing(struct logger_t * logger){
    pthread_t write_thread;

    write_thread_parameter_wrapper * wrapper = malloc(sizeof(write_thread_parameter_wrapper));
    wrapper->buffer = logger->buffer;

    if (logger->type != LOGGER_TYPE_CONSOLE){
        //wrapper->log_file= rmalloc(strlen((logger->log_file)));
        wrapper->log_file= rmalloc(strlen((logger->log_file)));
        rmemcpy(wrapper->log_file, logger->log_file, strlen((logger->log_file)));
    }
    wrapper->type = logger->type;

    // start the received thread for the port
    if (pthread_create(&write_thread, NULL, write_log_messages, wrapper)){
        char * log_msg = get_log_message_string(LOG_LEVEL_DEBUG, LOG_LEVEL_ERROR, "Logger init", "error while creating thread");
        log_to_console(log_msg);
        rfree(log_msg);
        exit(1);
    }

    logger->wrapper_ptr = wrapper;
    logger->write_thread = write_thread;
}

struct logger_t logger_init(log_level max_log_level, logger_type type){
    struct logger_t logger;

    logger.type = type;
    logger.max_log_level = max_log_level;
    logger.log_file = NULL;

    // init the mutex
    pthread_mutex_init(&logger.mutex, NULL);

    // init the buffer FIFO
    logger.buffer = fifo_init(LOGGER_BUFFER_SIZE);

    if (type == LOGGER_TYPE_CONSOLE){
        // if type is console, start write thread immediately
        // if type is file or both, it will be started when file is set

        // start write thread
        start_writing(&logger);
    }

    return logger;
}

void logger_set_log_file(struct logger_t* logger, char * path){
    logger->log_file = path;

    start_writing(logger);
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

    pthread_mutex_lock(&logger->mutex);
    log_level max_lvl = logger->max_log_level;
    pthread_mutex_unlock(&logger->mutex);
    char * msg = get_log_message_string(max_lvl, level, location, message);
    if (msg == NULL){
        // log level to low
        return;
    }

    // add message string to the write buffer
   fifo_push(logger->buffer, msg);
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

    pthread_mutex_lock(&logger->mutex);
    log_level max_lvl = logger->max_log_level;
    pthread_mutex_unlock(&logger->mutex);

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

    pthread_cancel(logger->write_thread);
    pthread_join(logger->write_thread, NULL);

    pthread_mutex_destroy(&logger->mutex);

    fifo_destroy(logger->buffer);
    rfree(logger->wrapper_ptr);
}
