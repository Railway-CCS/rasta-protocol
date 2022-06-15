#ifndef INCLUDE_RASTA_LIB_H
#define INCLUDE_RASTA_LIB_H
#include<rasta_new.h>
#include<rastahandle.h>

// The header, which the user will include later.

typedef struct rasta_connection rasta_lib_connection_t[1];

struct user_callbacks {
    /**
     * Allocate the memory neccessary for the connection list
     * and return it. Alternativly return NULL to refuse the connection.
     */
    void* (*on_connection_start)(rasta_lib_connection_t);

    /**
     * Whenever a connection disconnects, this handler is called to free the memory
     * allocated by user_handles::on_connection_start.
     */
    void (*on_disconnect)(rasta_lib_connection_t, void*);

    /**
     * This handler is called in case RaSTA cleanup is called.
     */
    void (*on_rasta_cleanup)();
};

typedef struct rasta_lib_configuration_s {
    struct rasta_handle h;
    event_system rasta_lib_event_system;
    struct user_callbacks callback;
} rasta_lib_configuration_t[1];

typedef fd_event rasta_lib_fd_event;
typedef timed_event rasta_lib_timed_event;

void rasta_lib_init_configuration(rasta_lib_configuration_t user_configuration, const char* config_file_path);

void rasta_lib_start(rasta_lib_configuration_t user_configuration);

#endif