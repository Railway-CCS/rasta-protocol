#include"event_system.h"
#include"rasta_new.h"
#include<time.h>
#include<unistd.h>

uint64_t get_nanotime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000 + t.tv_nsec;
}

/**
 * intializes a event_handler_settings struct
 * @param events the struct to initialize
 */
void init_events(struct event_handler_settings* events) {
    events->len = 0;
    events->running = 0;
}

/**
 * starts a event system an the current threat
 * @param events the event handler to handle in this loop
 */
void start_events(struct event_handler_settings* events) {
    events->running = 1;
    uint64_t cur_time = get_nanotime();
    uint64_t last_call[MAX_EVENT_HANDLER];
    for (int i = 0; i < events->len; i++) {
        last_call[i] = cur_time;
    }
    while (events->running) {
        cur_time = get_nanotime();
        uint64_t time_to_wait = UINT64_MAX;
        int next_event;
        for (int i = 0; i < events->len; i++) {
            uint64_t nano_continue = last_call[i] + events->events[i].interval;
            uint64_t n_time_to_wait = nano_continue > cur_time ? nano_continue - cur_time : 0;
            if (n_time_to_wait < time_to_wait) {
                next_event = i;
                time_to_wait = n_time_to_wait;
            }
        }
        if (time_to_wait != 0) {
            struct timespec t;
            struct timespec r;
            t.tv_sec = time_to_wait / 1000000000;
            t.tv_nsec = time_to_wait % 1000000000;
            printf("%u:%u\n", t.tv_sec, t.tv_nsec);
            nanosleep(&t, &r);
        }
        events->events[next_event].callback(events, cur_time - last_call[next_event]);
        last_call[next_event] = cur_time + time_to_wait;
    }
}

/**
 * adds a event handler to the specified loop
 * @param callback_function the function that is called
 * @param interval the interval at which the function is called in milliseconds
 * @return 0 if the event could be added 1 if not
 */
int add_event(struct event_handler_settings* events, event_ptr callback_function, unsigned int interval) {
    if (events->len >= MAX_EVENT_HANDLER) return 1;
    events->events[events->len].callback = callback_function;
    // add at least 1 us of delay
    if (interval == 0) interval = 1;
    events->events[events->len].interval = interval * 1000;
    events->len++;
}

/**
 * stops a event system
 * @param events the system to stop
 */
void stop_events(struct event_handler_settings* events) {
    events->running = 0;
}