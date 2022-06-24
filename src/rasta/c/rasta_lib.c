#include<rasta_lib.h>
#include<rasta_new.h>
#include<memory.h>
#include<stdbool.h>

void rasta_lib_init_configuration(rasta_lib_configuration_t user_configuration, const char* config_file_path) {
    sr_init_handle(&user_configuration->h, config_file_path);
    memset(&user_configuration->rasta_lib_event_system, 0, sizeof(user_configuration->rasta_lib_event_system));
    memset(&user_configuration->callback, 0, sizeof(user_configuration->callback));
    user_configuration->h.user_handles = &user_configuration->callback;
}

void rasta_lib_start(rasta_lib_configuration_t user_configuration, int wait_for_handshake) {
    sr_begin(&user_configuration->h, &user_configuration->rasta_lib_event_system, wait_for_handshake);
}
