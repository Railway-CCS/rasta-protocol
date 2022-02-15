#include <event_system.h>
#include <stdio.h>
#include <unistd.h>

#define SECOND_TO_NANO(s) s * (uint64_t) 1000000000

const uint64_t heartbeat_interval = SECOND_TO_NANO(1);
const uint64_t disconnect_interval = SECOND_TO_NANO(5);
uint64_t last_time;

uint64_t test_get_nanotime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000 + t.tv_nsec;
}

char send_heartbeat_event(void * carry_data) {
    uint64_t n_time = test_get_nanotime();
    printf("time since last call: %8lu us - expected:%8lu us\n", (n_time - last_time) / 1000, heartbeat_interval / 1000);
    last_time = n_time;
    return 0;
}

char disconnect_event(void* carry_data) {
    printf("disconnecting due to inactivity\n");
    return 1;
}

char event_read(void * carry_data) {
    char buffer[128];
    ssize_t len = read(STDIN_FILENO, buffer, 127);
    if (buffer[0] != '\n') {
        buffer[len] = 0;
        printf("detected: %s", buffer);
    }
    reschedule_event((timed_event*) carry_data);
    return 0;
}

int main() {
    last_time = test_get_nanotime();
    timed_event t_events[2];
    t_events[0].meta_information.callback = send_heartbeat_event;
    t_events[0].interval = heartbeat_interval;
    t_events[0].meta_information.carry_data = NULL;

    t_events[1].meta_information.callback = disconnect_event;
    t_events[1].interval = disconnect_interval;
    t_events[1].meta_information.carry_data = NULL;

    fd_event f_events[1];
    f_events[0].meta_information.callback = event_read;
    f_events[0].fd = STDIN_FILENO;
    f_events[0].meta_information.carry_data = t_events + 1;

    struct event_container container;
    init_event_container(&container);
    add_timed_event(&container, &(t_events[0]));
    add_timed_event(&container, &(t_events[1]));
    add_fd_event(&container, &(f_events[0]));

    start_event_loop(&container);
}
