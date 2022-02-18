/**
 * This example demonstrates the use of the logging module.
 */

#include <stdio.h>
#include <unistd.h>
#include "logging.h"

int main(int argc, char *argv[])  {
    printf("Start console logging test.\n");
    // create a console logger that will only print log levels INFO and ERROR
    struct logger_t logger = logger_init(LOG_LEVEL_INFO, LOGGER_TYPE_CONSOLE);

    logger_log(&logger, LOG_LEVEL_INFO, "MAIN_EX", "This should be printed");
    logger_log(&logger, LOG_LEVEL_ERROR, "MAIN_EX", "This should be printed %s", "too");
    logger_log(&logger, LOG_LEVEL_DEBUG, "MAIN_EX", "This shouldn't be printed as DEBUG>INFO");

    logger_log_if(&logger, (2*2) == 4, LOG_LEVEL_INFO, "MAIN_EX", "This should be printed, as cond=1");
    logger_log_if(&logger, (2*3) == 4, LOG_LEVEL_INFO, "MAIN_EX", "This shouldn't be printed, as cond=0");

    sleep(1);
    logger_destroy(&logger);

    printf("Console logging test complete.\n");

    printf("Start file logging test.\n");
    // create a file logger the will print all message levels
    logger = logger_init(LOG_LEVEL_DEBUG, LOGGER_TYPE_FILE);
    // set the log file path
    logger_set_log_file(&logger, "log.txt");

    logger_log(&logger, LOG_LEVEL_INFO, "MAIN_EX_FILE", "This should be printed");
    logger_log(&logger, LOG_LEVEL_ERROR, "MAIN_EX_FILE", "This should be printed too");
    logger_log(&logger, LOG_LEVEL_DEBUG, "MAIN_EX_FILE", "This should be printed as well");;

    sleep(1);
    logger_destroy(&logger);

    printf("File logging test complete.\n");

    printf("Start combined logging test.\n");
    // create a combined console/file logger, that will log all log levels
    logger = logger_init(LOG_LEVEL_DEBUG, LOGGER_TYPE_BOTH);
    // set the log file path
    logger_set_log_file(&logger, "log.txt");

    logger_log(&logger, LOG_LEVEL_INFO, "MAIN_EX_BOTH", "This should be printed on both %d", 2);
    logger_log(&logger, LOG_LEVEL_ERROR, "MAIN_EX_BOTH", "This should be printed on both too");
    logger_log(&logger, LOG_LEVEL_DEBUG, "MAIN_EX_BOTH", "This should be printed on both as well");

    sleep(1);
    logger_destroy(&logger);

    printf("Combined logging test complete.\n");
    return 0;
}
