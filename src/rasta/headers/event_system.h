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
typedef char (*event_ptr)();

/**
 * contains a function pointer to a callback function and interval in microseconds
 */
typedef struct timed_event {
    event_ptr callback;
    uint64_t interval;
    uint64_t __last_call;
    void* carry_data;
    char enabled;
} timed_event;

/**
 * contains a function pointer to a callback function and a file descriptor
 */
typedef struct fd_event {
    event_ptr callback;
    void* carry_data;
    int fd;
    char enabled;
} fd_event;

/**
 * starts an event loop with the given events
 * the events may not be removed while the loop is running, but can be modified
 * @param timed_events an array with the looping events to handle
 * @param timed_events_len the length of the timed_event array
 * @param fd_events an array with the events, that get called whenever the given fd gets readable
 * @param fd_events_len the length of the fd event array
 */
void start_event_loop(timed_event* timed_events, int timed_events_len, fd_event* fd_events, int fd_events_len);

/**
 * rescedules the event to the current time + the event interval
 * resulting in a delay of the event
 * @param event the event to delay
 */
void rescedule_event(timed_event* event);

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

#ifdef __cplusplus
}
#endif

#endif
