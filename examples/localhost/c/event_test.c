#include<event_system.h>
#include<stdio.h>
#include<unistd.h>
const uint64_t sleep_time = 1000000;
const uint64_t interval = 50;
char was_manipulated = 0;

void event1(struct event_handler_settings* events, uint64_t time_since_last_call) {
    printf("since last call:%lu ns - expected:%lu ns\n", time_since_last_call, interval * 1000);
    printf("raw error: %lu ns", time_since_last_call - interval * 1000);
    if (was_manipulated) {
        printf(" - error without delay: %lu ns", time_since_last_call - interval - sleep_time);
        was_manipulated = 0;
    }
    printf("\n");
}

void event2(struct event_handler_settings* events, uint64_t time_since_last_call) {
    printf("waiting %lu ns\n", sleep_time);
    was_manipulated = 1;
    usleep(sleep_time);
}

void event3(struct event_handler_settings* events, uint64_t time_since_last_call) {
    stop_events(events);
}

int main() {
    struct event_handler_settings events;
    init_events(&events);
    add_event(&events, event1, interval);
    //add_event(&events, event2, 2400000);
    //add_event(&events, event3, 1000);
    start_events(&events);
}