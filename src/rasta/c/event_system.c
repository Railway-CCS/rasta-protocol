#include "event_system.h"
#include "rasta_new.h"
#include <time.h>
#include <sys/select.h>

uint64_t get_nanotime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000 + t.tv_nsec;
}

/**
 * sleeps but keeps track of the fd events
 * @param time_to_wait the time to sleep in nanoseconds
 * @param fd_events the fd event array
 * @param len the length of the fd event array
 * @return the amount of fd events that got called or -1 to terminate the event loop
 */
int event_system_sleep(uint64_t time_to_wait, event_container* events) {
    struct timeval tv;
    tv.tv_sec = time_to_wait / 1000000000;
    tv.tv_usec = (time_to_wait / 1000) % 1000000;
    int nfds = 0;
    // find highest fd
    for (fd_event* event = events->fd_event_list; event; event = (fd_event*) event->meta_information.next) {
        nfds = nfds < event->fd ? event->fd : nfds;
    }
    // set the fd to watch
    fd_set on_readable;
    FD_ZERO(&on_readable);
    for (fd_event* event = events->fd_event_list; event; event = (fd_event*) event->meta_information.next) {
        if (event->meta_information.enabled) {
            FD_SET(event->fd, &on_readable);
        }
    }
    // wait
    int result = select(nfds + 1, &on_readable, NULL, NULL, &tv);
    if (result == -1) {
        // syscall error or error on select()
        return -1;
    }
    for (fd_event* event = events->fd_event_list; event; event = (fd_event*) event->meta_information.next) {
        if (event->meta_information.enabled && FD_ISSET(event->fd, &on_readable)) {
            if (event->meta_information.callback(event->meta_information.carry_data)) return -1;
        }
    }
    return result;
}

/**
 * reschedules the event to the current time + the event interval
 * resulting in a delay of the event
 * @param event the event to delay
 */
void reschedule_event(timed_event * event) {
    event->__last_call = get_nanotime();
}

/**
 * calculates the next timed event that has to be called and the time to wait for it
 * @param timed_events array of the events
 * @param len the lenght of the array
 * @param next_event_index the index of the next event will be written in here, can be NULL
 * @param cur_time the current time
 * @return uint64_t the time to wait
 */
uint64_t calc_next_timed_event(event_container* events, timed_event** next_event, uint64_t cur_time) {
    uint64_t time_to_wait = UINT64_MAX;
    for (timed_event* event = events->timed_event_list; event; event = (timed_event*) event->meta_information.next) {
        if (event->meta_information.enabled) {
            uint64_t continue_at = event->__last_call + event->interval;
            if (continue_at <= cur_time) {
                if (next_event) {
                    *next_event = event;
                }
                return 0;
            }
            else {
                uint64_t new_time_to_wait = continue_at - cur_time;
                if (new_time_to_wait < time_to_wait) {
                    if (next_event) {
                        *next_event = event;
                    }
                    time_to_wait = new_time_to_wait;
                }
            }
        }
    }
    return time_to_wait;
}

/**
 * starts an event loop with the given events
 * @param events a pointer to the event "container",
 * which contains two linked lists filled with events
 */
void start_event_loop(event_container* events) {
    uint64_t cur_time = get_nanotime();
    // linked list foreach
    for (timed_event* event = events->timed_event_list; event; event = (timed_event*) event->meta_information.next) {
        event->__last_call = cur_time;
    }
    while (1) {
        timed_event* next_event;
        cur_time = get_nanotime();
        uint64_t time_to_wait = calc_next_timed_event(events, &next_event, cur_time);
        if (time_to_wait == UINT64_MAX) {
            // there are no active events
            int result = event_system_sleep(1000, events);
            if (result == -1) {
                break;
            }
            continue;
        }
        else if (time_to_wait != 0) {
            int result = event_system_sleep(time_to_wait, events);
            if (result == -1) {
                // select failed, exit loop
                return;
            }
            else if (result >= 0) {
                // the sleep didn't time out, but a fd event occured
                // recalculate next timed event in case one got rescheduled
                continue;
            }
        }
        // fire event and exit in case it returns something else than 0
        if (next_event->meta_information.callback(next_event->meta_information.carry_data)) break;
        // update timed_event::__last_call
        next_event->__last_call = cur_time + time_to_wait;
    }
}

/**
 * enables a timed event, it will fire in event::interval nanoseconds
 * @param event the event to enable
 */
void enable_timed_event(timed_event* event) {
    event->meta_information.enabled = 1;
    reschedule_event(event);
}

/**
 * temporarily disables a timed event
 * @param event the event to disable
 */
void disable_timed_event(timed_event* event) {
    event->meta_information.enabled = 0;
}

/**
 * enables a fd event
 * @param event the event to enable
 */
void enable_fd_event(fd_event* event) {
    event->meta_information.enabled = 1;
}

/**
 * enables a fd event
 * @param event the event to enable
 */
void disable_fd_event(fd_event* event) {
    event->meta_information.enabled = 0;
}

/**
 * initializes an empty event container
 * @param container the container to initialize
 */
void init_event_container(event_container* container) {
    container->fd_event_list = NULL;
    container->fd_event_list_append_to = &container->fd_event_list;
    container->timed_event_list = NULL;
    container->timed_event_list_append_to = &container->timed_event_list;
}

void linked_list_add(struct event_shared_information* linked_list, struct event_shared_information*** linked_list_append_to, struct event_shared_information* to_add) {
    to_add->next = NULL;
    to_add->prev_mem_addr = *linked_list_append_to;
    **linked_list_append_to = to_add;
    *linked_list_append_to = &to_add->next;
}

void linked_list_remove(struct event_shared_information* to_add) {
    *to_add->prev_mem_addr = to_add->next;
}

/**
 * adds an io-event to an event_container
 * @param container destination, appent here
 * @param event the event to add
 */
void add_fd_event(event_container* container, fd_event* event) {
    linked_list_add(
        (struct event_shared_information*) container->fd_event_list,
        (struct event_shared_information***) &container->fd_event_list_append_to,
        &event->meta_information
    );
}

/**
 * removes an io-event from its current event container
 * @param event the event to remove
 */
void remove_fd_event(fd_event* event) {
    linked_list_remove(&event->meta_information);
}

/**
 * Adds an timed-event to an event_container.
 * @param container destination, appent here
 * @param event the event to add
 */
void add_timed_event(event_container* container, timed_event* event) {
    add_timed_event_no_time_init(container, event);
    event->__last_call = get_nanotime();
}

/**
 * Adds an timed-event to an event_container but does not initialize the event timer.
 * Use only for short time suspension of an event.
 * @param container destination, appent here
 * @param event the event to add
 */
void add_timed_event_no_time_init(event_container* container, timed_event* event) {
    linked_list_add(
        (struct event_shared_information*) container->timed_event_list,
        (struct event_shared_information***) &container->timed_event_list_append_to,
        &event->meta_information
    );
}

/**
 * remove an timed event from its current event container
 * @param event the event to remove
 */
void remove_timed_event(timed_event* event) {
    linked_list_remove(&event->meta_information);
}
