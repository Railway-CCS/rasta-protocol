#include "event_system.h"
#include "rasta_new.h"
#include <time.h>
#include <sys/select.h>

uint64_t get_nanotime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000 + t.tv_nsec;
}

int get_max_nfds(struct fd_event_linked_list_s* fd_events) {
    int nfds = 0;
    // find highest fd and set nfds to 1 higher
    for (fd_event* current = fd_events->first; current; current = current->next) {
        nfds = nfds < current->fd ? current->fd : nfds;
    }
    nfds++;
    return nfds;
}

/**
 * sleeps but keeps track of the fd events
 * @param time_to_wait the time to sleep in nanoseconds
 * @param fd_events the fd event array
 * @param len the length of the fd event array
 * @return the amount of fd events that got called or -1 to terminate the event loop
 */
int event_system_sleep(uint64_t time_to_wait, struct fd_event_linked_list_s* fd_events) {
    struct timeval tv;
    tv.tv_sec = time_to_wait / 1000000000;
    tv.tv_usec = (time_to_wait / 1000) % 1000000;
    int nfds = get_max_nfds(fd_events);
    if (nfds >= FD_SETSIZE) {
        // too high file desriptors, can be fixed by using poll instead but should not be an issue
        return -1;
    }
    // zero and set the fd to watch
    fd_set on_readable;
    fd_set on_writable;
    fd_set on_exceptional;
    FD_ZERO(&on_readable);
    FD_ZERO(&on_writable);
    FD_ZERO(&on_exceptional);
    for (fd_event* current = fd_events->first; current; current = current->next) {
        if (current->enabled) {
            if (current->options & EV_READABLE)
                FD_SET(current->fd, &on_readable);
            if (current->options & EV_WRITABLE)
                FD_SET(current->fd, &on_writable);
            if (current->options & EV_EXCEPTIONAL)
                FD_SET(current->fd, &on_exceptional);
        }
    }
    // wait
    int result = select(nfds, &on_readable, &on_writable, &on_exceptional, &tv);
    if (result == -1) {
        // syscall error or error on select()
        return -1;
    }
    for (fd_event* current = fd_events->first; current; current = current->next) {
        if (current->enabled && FD_ISSET(current->fd, &on_readable)) {
            if (current->callback(current->carry_data)) return -1;
        }
        if (current->enabled && FD_ISSET(current->fd, &on_writable)) {
            if (current->callback(current->carry_data)) return -1;
        }
        if (current->enabled && FD_ISSET(current->fd, &on_exceptional)) {
            if (current->callback(current->carry_data)) return -1;
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
    event->last_call = get_nanotime();
}

/**
 * calculates the next timed event that has to be called and the time to wait for it
 * @param timed_events array of the events
 * @param len the lenght of the array
 * @param next_event_index the index of the next event will be written in here, can be NULL
 * @param cur_time the current time
 * @return uint64_t the time to wait
 */
uint64_t calc_next_timed_event(struct timed_event_linked_list_s* timed_events, timed_event** next_timed_event, uint64_t cur_time) {
    uint64_t time_to_wait = UINT64_MAX;
    for (timed_event* current = timed_events->first; current; current = current->next) {
        if (current->enabled) {
            uint64_t continue_at = current->last_call + current->interval;
            if (continue_at <= cur_time) {
                if (next_timed_event) {
                    *next_timed_event = current;
                }
                return 0;
            }
            else {
                uint64_t new_time_to_wait = continue_at - cur_time;
                if (new_time_to_wait < time_to_wait) {
                    if (next_timed_event) {
                        *next_timed_event = current;
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
 * the events may not be removed while the loop is running, but can be modified
 * @param ev_sys contains all the events the loop should handel.
 * Can be modified from the calling thread while running.
 */
void start_event_loop(event_system* ev_sys) {
    uint64_t cur_time = get_nanotime();
    for (timed_event* current = ev_sys->timed_events.first; current; current = current->next) {
        current->last_call = cur_time;
    }
    while (1) {
        timed_event* next_event;
        cur_time = get_nanotime();
        uint64_t time_to_wait = calc_next_timed_event(&ev_sys->timed_events, &next_event, cur_time);
        if (time_to_wait == UINT64_MAX) {
            // there are no active events - just wait for fd events
            int result = event_system_sleep(~0, &ev_sys->fd_events);
            if (result == -1) {
                break;
            }
            continue;
        }
        else if (time_to_wait != 0) {
            int result = event_system_sleep(time_to_wait, &ev_sys->fd_events);
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
        if (next_event->callback(next_event->carry_data)) {
            break;
        }
        // update timed_event::last_call
        next_event->last_call = cur_time + time_to_wait;
    }
}

/**
 * enables a timed event, it will fire in event::interval nanoseconds
 * @param event the event to enable
 */
void enable_timed_event(timed_event* event) {
    event->enabled = 1;
    reschedule_event(event);
}

/**
 * temporarily disables a timed event
 * @param event the event to disable
 */
void disable_timed_event(timed_event* event) {
    event->enabled = 0;
}

/**
 * enables a fd event
 * @param event the event to enable
 */
void enable_fd_event(fd_event* event) {
    event->enabled = 1;
}

/**
 * enables a fd event
 * @param event the event to enable
 */
void disable_fd_event(fd_event* event) {
    event->enabled = 0;
}

/**
 * Add a timed event to an event system. 
 * A event can only be in one event system at a time. 
 * (not thread safe)
 * @param ev_sys the event will be added to this event system
 * @param event the event to add
 */
void add_timed_event(event_system* ev_sys, timed_event* event) {
    // simple linked list add
    if (ev_sys->timed_events.last) {
        event->prev = ev_sys->timed_events.last;
        ev_sys->timed_events.last->next = event;
    }
    else {
        ev_sys->timed_events.first = event;
        ev_sys->timed_events.last = event;
    }
    event->prev = ev_sys->timed_events.last;
    event->next = NULL;
}

/**
 * Removes a timed event from its event system. 
 * (not thread safe)
 * @param ev_sys the event will be added to this event system
 * @param event the event to add
 */
void remove_timed_event(timed_event* event) {
    // simple linked list remove
    if (event->prev) event->prev->next = event->next;
    if (event->next) event->next->prev = event->prev;
}

/**
 * Add a fd event to an event system. 
 * A event can only be in one event system at a time. 
 * (not thread safe)
 * @param ev_sys the event will be added to this event system
 * @param event the event to add
 * @param options set how the event should be triggered. (EV_READABLE | EV_WRITEABLE | EV_CHANGE)
 */
void add_fd_event(event_system* ev_sys, fd_event* event, int options) {
    // simple linked list add
    if (ev_sys->fd_events.last) {
        event->prev = ev_sys->fd_events.last;
        ev_sys->fd_events.last->next = event;
    }
    else {
        ev_sys->fd_events.first = event;
        ev_sys->fd_events.last = event;
    }
    event->prev = ev_sys->fd_events.last;
    event->next = NULL;

    event->options = options;
}

/**
 * Removes a fd event from its event system. 
 * (not thread safe)
 * @param ev_sys the event will be added to this event system
 * @param event the event to add
 */
void remove_fd_event(fd_event* event) {
    if (event->prev) event->prev->next = event->next;
    if (event->next) event->next->prev = event->prev;
}