/**
 * This module provides basic logging functionality
 * logging to a file, to the console or both are supported
 *
 * The supported log level are
 * Debug, Info, Error (and None which can only be used to specify the maximum log level)
 */

#ifndef LST_SIMULATOR_LOGGING_H
#define LST_SIMULATOR_LOGGING_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <pthread.h>
#include <mqueue.h>
#include "fifo.h"

#define LOG_FORMAT "[%s][%s][%s] %s\n"

/**
 * maximum amount of log messages in the buffer
 */
#define LOGGER_BUFFER_SIZE 10

/**
 * maximum size of log messages in bytes
 */
#define LOGGER_MAX_MSG_SIZE 4096

/**
 * the log level
 */
typedef enum {
    /**
     * Messages meant for debugging
     */
    LOG_LEVEL_DEBUG = 3,
    /**
     * Messages for general connection status
     */
    LOG_LEVEL_INFO = 2,
    /**
     * Error messages
     */
    LOG_LEVEL_ERROR = 1,
    /**
     * Symbolic log level: nothing will be logged
     * Can only be used as maximum log level
     */
    LOG_LEVEL_NONE = 0
} log_level;

/**
 * represents the type of logging that should be used
 */
typedef enum {
    /**
     * log messages to console
     */
    LOGGER_TYPE_CONSOLE = 0,
    /**
     * log messages to file
     */
    LOGGER_TYPE_FILE = 1,
    /**
     * log to console and file
     */
    LOGGER_TYPE_BOTH = 2
}logger_type;

/**
 * wrapper struct to pass multiple parameters to the write thread handler
 */
typedef struct {
    /**
     * the buffer FIFO
     */
    fifo_t * buffer;

    /**
     * if logger is Console or Both: path to the log file. NULL otherwise
     */
    char * log_file;

    /**
     * the type of the logger
     */
    logger_type type;
}write_thread_parameter_wrapper;

/**
 * represents a logger
 */
struct logger_t{
    /**
     * maximum log level the logger will log
     */
    log_level max_log_level;

    /**
     * the type of logging that will be used
     */
    logger_type type;

    /**
     * the path to the log file, when file logging is used
     */
    char* log_file;

    /**
     * the mutex used to enable use in multiple threads
     */
    pthread_mutex_t mutex;

    /**
     * the thread that writes the buffered log messages to the output
     */
    pthread_t write_thread;

    /**
     * the FIFO that is used to buffer log messages
     */
    fifo_t * buffer;

    /**
     * the pointer to the wrapper that was passed to the write thread. Used to free in logger_destroy.
     */
    write_thread_parameter_wrapper * wrapper_ptr;
};

/**
 * initializes the logger
 * @param max_log_level the maximum log level the will be logged, oder is DEBUG>INFO>ERROR>NONE
 * @param type the type of logging
 * @return a logger struct
 */
struct logger_t logger_init(log_level max_log_level, logger_type type);

/**
 * sets the path to the log file
 * @param logger the logger where the file should be set
 * @param path the path to the file
 */
void logger_set_log_file(struct logger_t* logger, char * path);

/**
 * logs a message
 * @param logger the logger which should be used
 * @param level the log level of the message
 * @param location the location where the log message occurred
 * @param format the message which should be logged. can contain formatting information like %s, %d, ...
 * @param ... the format parameters
 */
void logger_log(struct logger_t * logger, log_level level, char* location ,char* format, ...);

/**
 * logs a message of a specified condition is true (1)
 * @param logger the logger which should be used
 * @param cond the message will only be logged if this expression is 1
 * @param level the log level of the message
 * @param location the location where the log message occurred
 * @param format the message which should be logged. can contain formatting information like %s, %d, ...
 * @param ... the format parameters
 */
void logger_log_if(struct logger_t * logger, int cond, log_level level, char * location, char * format, ...);

/**
 * stops the write thread and frees resources of the logger
 * @param logger the logger that is cleared
 */
void logger_destroy(struct logger_t * logger);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_LOGGING_H
