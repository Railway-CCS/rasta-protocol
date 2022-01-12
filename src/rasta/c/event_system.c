#include"event_system.h"
#include"rasta_new.h"
#include<time.h>
#include<sys/select.h>

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
int event_system_sleep(uint64_t time_to_wait, fd_event fd_events[], int len) {
    struct timeval tv;
    tv.tv_sec = time_to_wait / 1000000000;
    tv.tv_usec = (time_to_wait / 1000) % 1000000;
    int nfds = 0;
    // find highest fd
    for (int i = 0; i < len; i++) nfds = nfds < fd_events[i].fd ? fd_events[i].fd : nfds;
    // set the fd to watch
    fd_set on_readable;
    FD_ZERO(&on_readable);
    for (int i = 0; i < len; i++) {
        if (fd_events[i].enabled) {
            FD_SET(fd_events[i].fd, &on_readable);
        }
    }
    // wait
    int result = select(nfds + 1, &on_readable, NULL, NULL, &tv);
    if (result == -1) {
        // syscall error or error on select()
        return -1;
    }
    for (int i = 0; i < len; i++) {
        if (fd_events[i].enabled && FD_ISSET(fd_events[i].fd, &on_readable)) {
            if (fd_events[i].callback(fd_events[i].carry_data)) return -1;
        }
    }
    return result;
}

/**
 * rescedules the event to the current time + the event interval
 * resulting in a delay of the event
 * @param event the event to delay
 */
void rescedule_event(timed_event * event) {
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
uint64_t calc_next_timed_event(struct timed_event timed_events[], int len, int * next_event_index, uint64_t cur_time) {
    uint64_t time_to_wait = UINT64_MAX;
    for (int i = 0; i < len; i++) {
        if (timed_events[i].enabled) {
            uint64_t continue_at = timed_events[i].__last_call + timed_events[i].interval;
            if (continue_at <= cur_time) {
                if (next_event_index) {
                    *next_event_index = i;
                }
                return 0;
            }
            else {
                uint64_t new_time_to_wait = continue_at - cur_time;
                if (new_time_to_wait < time_to_wait) {
                    if (next_event_index) {
                        *next_event_index = i;
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
 * the events may not be removed while the loop is running but can be modified
 * @param timed_events an array with the looping events to handle
 * @param timed_events_len the length of the timed_event array
 * @param fd_events an array with the events, that get called whenever the given fd gets readable
 * @param fd_events_len the length of the fd event array
 */
void start_event_loop(timed_event timed_events[], int timed_events_len, fd_event fd_events[], int fd_events_len) {
    uint64_t cur_time = get_nanotime();
    for (int i = 0; i < timed_events_len; i++) {
        timed_events[i].__last_call = cur_time;
    }
    while (1) {
        int next_event;
        cur_time = get_nanotime();
        uint64_t time_to_wait = calc_next_timed_event(timed_events, timed_events_len, &next_event, cur_time);
        if (time_to_wait == UINT64_MAX) {
            // there are no active events
            int result = event_system_sleep(1000, fd_events, fd_events_len);
            if (result == -1) {
                break;
            }
            continue;
        }
        else if (time_to_wait != 0) {
            int result = event_system_sleep(time_to_wait, fd_events, fd_events_len);
            if (result == -1) {
                // select failed, exit loop
                return;
            }
            else if (result >= 0) {
                // the sleep didn't time out, but a fd event occured
                // recalculate next timed event in case one got resceduled
                continue;
            }
        }
        // fire event and exit in case it returns something else than 0
        if (timed_events[next_event].callback(timed_events[next_event].carry_data)) break;
        // update timed_event::__last_call
        timed_events[next_event].__last_call = cur_time + time_to_wait;
    }
}

/**
 * enables a timed event, it will fire in event::interval nanoseconds
 * @param event the event to enable
 */
void enable_timed_event(timed_event* event) {
    event->enabled = 1;
    rescedule_event(event);
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