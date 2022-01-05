#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H
#include"rastahandle.h"
#include<unistd.h>

#define MAX_EVENT_HANDLER 16

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

struct event_handler_settings;

typedef void (*event_ptr)(struct event_handler_settings* events, uint64_t nano_since_last_call);

struct event_handler_settings {
    /**
     * contains the callback function pointer and information about the events
     */
    struct {
        event_ptr callback;
        uint64_t interval;
    } events[MAX_EVENT_HANDLER];
    unsigned int len;
    char running;
};

/**
 * intializes a event_handler_settings struct
 * @param events the struct to initialize
 */
void init_events(struct event_handler_settings* events);

/**
 * starts a blocking loop, handling the events specified
 * @param events the event handler to handle in this loop
 */
void start_events(struct event_handler_settings* events);

/**
 * adds a event handler to the specified loop
 * @param callback_function the function that is called
 * @param interval the interval at which the function is called in milliseconds
 * @return 0 if the event could be added 1 if not
 */
int add_event(struct event_handler_settings* events, event_ptr callback_function, unsigned int interval);

/**
 * stops a event system
 * @param events the system to stop
 */
void stop_events(struct event_handler_settings* events);

#ifdef __cplusplus
}
#endif

#endif