#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H
#include <inttypes.h>
#include <time.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

// event callback pointer, return 0 to keep the loop running, everything else stops the loop
typedef char (*event_ptr)(void* h);

struct event_shared_information {
    event_ptr callback;
    void* carry_data;
    struct event_shared_information* next;
    struct event_shared_information** prev_mem_addr;
    char enabled;
};

/**
 * contains a function pointer to a callback function and interval in microseconds
 */
typedef struct timed_event {
    struct event_shared_information meta_information;
    uint64_t interval;
    uint64_t __last_call;
} timed_event;

/**
 * contains a function pointer to a callback function and a file descriptor
 */
typedef struct fd_event {
    struct event_shared_information meta_information;
    int fd;
} fd_event;

typedef struct event_container {
    // the fd_events kept in the container as a linked list
    fd_event* fd_event_list;
    fd_event** fd_event_list_append_to;
    // the timed_events kept in the container as a linked list
    timed_event* timed_event_list;
    timed_event** timed_event_list_append_to;
} event_container;

/**
 * initializes an empty event container
 * @param container the container to initialize
 */
void init_event_container(event_container* container);

/**
 * starts an event loop with the given events
 * @param events a pointer to the event "container",
 * which contains two linked lists filled with events
 */
void start_event_loop(event_container* events);

/**
 * reschedules the event to the current time + the event interval
 * resulting in a delay of the event
 * @param event the event to delay
 */
void reschedule_event(timed_event* event);

/**
 * enables a timed event, it will fire in event::interval nanoseconds
 * @param event the event to enable
 */
void enable_timed_event(timed_event* event);

/**
 * enables a fd event
 * @param event the event to enable
 */
void enable_fd_event(fd_event* event);

/**
 * disables a timed event
 * @param event the event to disable
 */
void disable_timed_event(timed_event* event);

/**
 * enables a fd event
 * @param event the event to enable
 */
void disable_fd_event(fd_event* event);

/**
 * adds an io-event to an event_container
 * @param container destination, appent here
 * @param event the event to add
 */
void add_fd_event(event_container* container, fd_event* event);

/**
 * removes an io-event from its current event container
 * @param event the event to remove
 */
void remove_fd_event(fd_event* event);

/**
 * adds an timed-event to an event_container
 * @param container destination, appent here
 * @param event the event to add
 */
void add_timed_event(event_container* container, timed_event* event);

/**
 * Adds an timed-event to an event_container but does not initialize the event timer.
 * Use only for short time suspension of an event.
 * @param container destination, appent here
 * @param event the event to add
 */
void add_timed_event_no_time_init(event_container* container, timed_event* event);

/**
 * remove an timed event from its current event container
 * @param event the event to remove
 */
void remove_timed_event(timed_event* event);

#ifdef __cplusplus
}
#endif

#endif
