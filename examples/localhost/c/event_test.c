#include<event_system.h>
#include<stdio.h>
#include<unistd.h>
const uint64_t interval = 1000 * 1000 * 1000;
uint64_t last_time;

uint64_t test_get_nanotime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000 + t.tv_nsec;
}

char event_time() {
    uint64_t n_time = test_get_nanotime();
    printf("since last call: %lu ns - expected:%lu ns\n", n_time - last_time, interval);
    last_time = n_time;
    return 0;
}

char event_read() {
    char buffer[128];
    ssize_t len = read(STDIN_FILENO, buffer, 127);
    buffer[len] = 0;
    printf("detected: %s", buffer);
    return 0;
}

int main() {
    last_time = test_get_nanotime();
    timed_event t_events[1];
    t_events[0].callback = event_time;
    t_events[0].interval = interval;
    fd_event f_events[1];
    f_events[0].callback = event_read;
    f_events[0].fd = STDIN_FILENO;
    start_event_loop(t_events, 1, f_events, 1);
}