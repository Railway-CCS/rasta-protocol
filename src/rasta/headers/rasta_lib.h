#ifndef INCLUDE_RASTA_LIB_H
#define INCLUDE_RASTA_LIB_H
#include<rasta_new.h>
#include<rastahandle.h>

// The header, which the user will include later.

typedef struct rasta_lib_configuration_s {
    struct rasta_handle h;
    event_system rasta_lib_event_system;
} rasta_lib_configuration_t[1];

typedef fd_event rasta_lib_fd_event;
typedef timed_event rasta_lib_timed_event;

void rasta_lib_init_configuration(rasta_lib_configuration_t user_configuration, const char* config_file_path);

void rasta_lib_start(rasta_lib_configuration_t user_configuration);

#endif