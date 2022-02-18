//
// Created by tobia on 22.03.2018.
//

#include <stdlib.h>
#include "rastahandle.h"
#include "rmemory.h"


#define RASTA_CONFIG_KEY_LOGGER_TYPE "LOGGER_TYPE"
#define RASTA_CONFIG_KEY_LOGGER_FILE "LOGGER_FILE"
#define RASTA_CONFIG_KEY_LOGGER_MAX_LEVEL "LOGGER_MAX_LEVEL"
#define RASTA_CONFIG_KEY_ACCEPTED_VERSIONS "RASTA_ACCEPTED_VERSIONS"

//---------- Util functions for calling notifications in new thread ----------


struct rasta_notification_result sr_create_notification_result(struct rasta_handle *handle, struct rasta_connection *connection) {
    struct rasta_notification_result r;

    r.handle = handle;
    r.connection = *connection;

    return r;
}

/**
 * the is the function that handles the call of the onConnectionStateChange notification pointer.
 * this runs on a separate thread
 * @param connection the connection that will be used
 * @return unused
 */
void* on_constatechange_call(void * container){
    struct rasta_notification_result * result = (struct rasta_notification_result * )container;

    (*result->handle->notifications.on_connection_state_change)(result);

    // notification handler completed, decrease amount of running threads
    result->handle->running_notifications = result->handle->running_notifications -1;

    //free container
    rfree(container);

    return NULL;
}

/**
 * fires the onConnectionStateChange event.
 * This implementation will take care if the function pointer is NULL and start a thread to call the notification
 * @param connection the connection that is used
 */
void fire_on_connection_state_change(struct rasta_notification_result result){
    pthread_mutex_lock(&result.handle->notification_lock);
    if (result.handle->notifications.on_connection_state_change == NULL){
        // notification not set, do nothing
        pthread_mutex_unlock(&result.handle->notification_lock);
        return;
    }

    // a thread will be started, increase amount of running notification threads
    result.handle->running_notifications = result.handle->running_notifications + 1;

    pthread_t caller_thread;

    //create container
    struct rasta_notification_result* container = rmalloc(sizeof(struct rasta_notification_result));
    *container = result;

    if (pthread_create(&caller_thread, NULL, on_constatechange_call, container)){
        logger_log(&result.handle->logger, LOG_LEVEL_ERROR, "RaSTA call on connection state change", "error while creating thread");
        exit(1);
    }

    pthread_mutex_unlock(&result.handle->notification_lock);
}


void* on_receive_call(void * container){
    struct rasta_notification_result * result = (struct rasta_notification_result * )container;

    (*result->handle->notifications.on_receive)(result);

    // notification handler completed, decrease amount of running threads
    result->handle->running_notifications = result->handle->running_notifications -1;

    //free container
    rfree(container);

    return NULL;
}

/**
 * fires the onConnectionStateChange event.
 * This implementation will take care if the function pointer is NULL and start a thread to call the notification
 * @param connection the connection that is used
 */
void fire_on_receive(struct rasta_notification_result result){
    pthread_mutex_lock(&result.handle->notification_lock);
    if (result.handle->notifications.on_receive == NULL){
        // notification not set, do nothing
        pthread_mutex_unlock(&result.handle->notification_lock);
        return;
    }

    // a thread will be started, increase amount of running notification threads
    result.handle->running_notifications = result.handle->running_notifications + 1;

    pthread_t caller_thread;

    //create container
    struct rasta_notification_result* container = rmalloc(sizeof(struct rasta_notification_result));
    *container = result;

    if (pthread_create(&caller_thread, NULL, on_receive_call, container)){
        logger_log(&result.handle->logger, LOG_LEVEL_ERROR, "RaSTA call on connection state change", "error while creating thread");
        exit(1);
    }
    pthread_mutex_unlock(&result.handle->notification_lock);
}

void* on_discrequest_change_call(void * container){
    struct rasta_disconnect_notification_result * result = (struct rasta_disconnect_notification_result * )container;

    (*result->result.handle->notifications.on_disconnection_request_received)(&result->result,result->reason,result->detail);

    // notification handler completed, decrease amount of running threads
    result->result.handle->running_notifications = result->result.handle->running_notifications -1;

    //free container
    rfree(container);

    return NULL;
}

/**
 * fires the onConnectionStateChange event.
 * This implementation will take care if the function pointer is NULL and start a thread to call the notification
 * @param connection the connection that is used
 */
void fire_on_discrequest_state_change(struct rasta_notification_result result, struct RastaDisconnectionData data){
    pthread_mutex_lock(&result.handle->notification_lock);

    if (result.handle->notifications.on_disconnection_request_received == NULL){
        // notification not set, do nothing
        pthread_mutex_unlock(&result.handle->notification_lock);
        return;
    }

    // a thread will be started, increase amount of running notification threads
    result.handle->running_notifications = result.handle->running_notifications + 1;

    pthread_t caller_thread;

    //create container


    struct rasta_disconnect_notification_result* container = rmalloc(sizeof(struct rasta_disconnect_notification_result));
    container->result = result;
    container->reason = data.reason;
    container->detail = data.details;

    if (pthread_create(&caller_thread, NULL, on_discrequest_change_call, container)){
        logger_log(&result.handle->logger, LOG_LEVEL_ERROR, "RaSTA call disconnection", "error while creating thread");
        exit(1);
    }

    pthread_mutex_unlock(&result.handle->notification_lock);
}


/**
 * the is the function that handles the call of the onDiagnosticNotification notification pointer.
 * this runs on a separate thread
 * @param connection the connection that will be used
 * @return unused
 */
void* on_diagnostic_call(void * container){
    struct rasta_notification_result * result = (struct rasta_notification_result * )container;

    (*result->handle->notifications.on_diagnostic_notification)(result);

    // notification handler completed, decrease amount of running threads
    result->handle->running_notifications = result->handle->running_notifications -1;

    //free container
    rfree(container);

    return NULL;
}

/**
 * fires the onDiagnosticNotification event.
 * This implementation will take care if the function pointer is NULL and start a thread to call the notification
 * @param connection the connection that is used
 */
void fire_on_diagnostic_notification(struct rasta_notification_result result){
    pthread_mutex_lock(&result.handle->notification_lock);

    if (result.handle->notifications.on_diagnostic_notification == NULL){
        // notification not set, do nothing
        pthread_mutex_unlock(&result.handle->notification_lock);
        return;
    }

    if (result.connection.received_diagnostic_message_count <= 0) {
        // no diagnostic notification to send
        pthread_mutex_unlock(&result.handle->notification_lock);
        return;
    }

    // a thread will be started, increase amount of running notification threads
    result.handle->running_notifications = result.handle->running_notifications + 1;

    pthread_t caller_thread;

    //create container
    struct rasta_notification_result* container = rmalloc(sizeof(struct rasta_notification_result));
    *container = result;

    if (pthread_create(&caller_thread, NULL, on_diagnostic_call, container)){
        logger_log(&result.handle->logger, LOG_LEVEL_ERROR, "RaSTA call on diagnostic change", "error while creating thread");
        exit(1);
    }

    pthread_mutex_unlock(&result.handle->notification_lock);
}

void * on_handshake_complete_call(void * container){
    struct rasta_notification_result * result = (struct rasta_notification_result * )container;

    (*result->handle->notifications.on_handshake_complete)(result);

    // notification handler completed, decrease amount of running threads
    result->handle->running_notifications = result->handle->running_notifications -1;

    //free container
    rfree(container);

    return NULL;
}

void fire_on_handshake_complete(struct rasta_notification_result result){
    pthread_mutex_lock(&result.handle->notification_lock);

    if (result.handle->notifications.on_handshake_complete == NULL){
        // notification not set, do nothing
        pthread_mutex_unlock(&result.handle->notification_lock);
        return;
    }

    // a thread will be started, increase amount of running notification threads
    result.handle->running_notifications = result.handle->running_notifications + 1;

    pthread_t caller_thread;

    //create container
    struct rasta_notification_result* container = rmalloc(sizeof(struct rasta_notification_result));
    *container = result;

    if (pthread_create(&caller_thread, NULL, on_handshake_complete_call, container)){
        logger_log(&result.handle->logger, LOG_LEVEL_ERROR, "RaSTA call on handshake complete", "error while creating thread");
        exit(1);
    }

    pthread_mutex_unlock(&result.handle->notification_lock);
}

void * on_heartbeat_timeout_call(void * container){
    struct rasta_notification_result * result = (struct rasta_notification_result * )container;

    (*result->handle->notifications.on_heartbeat_timeout)(result);

    // notification handler completed, decrease amount of running threads
    result->handle->running_notifications = result->handle->running_notifications -1;

    //free container
    rfree(container);

    return NULL;
}

void fire_on_hearbeat_timeout(struct rasta_notification_result result){
    pthread_mutex_lock(&result.handle->notification_lock);

    if (result.handle->notifications.on_heartbeat_timeout == NULL){
        // notification not set, do nothing
        pthread_mutex_unlock(&result.handle->notification_lock);
        return;
    }

    // a thread will be started, increase amount of running notification threads
    result.handle->running_notifications = result.handle->running_notifications + 1;

    pthread_t caller_thread;

    //create container
    struct rasta_notification_result* container = rmalloc(sizeof(struct rasta_notification_result));
    *container = result;

    if (pthread_create(&caller_thread, NULL, on_heartbeat_timeout_call, container)){
        logger_log(&result.handle->logger, LOG_LEVEL_ERROR, "RaSTA call on heartbeat timeout", "error while creating thread");
        exit(1);
    }

    pthread_mutex_unlock(&result.handle->notification_lock);
}

void rasta_handle_manually_init(struct rasta_handle *h, struct RastaConfigInfo configuration, struct DictionaryArray accepted_versions , struct logger_t logger) {

    h->config.values = configuration;

    h->logger = logger;


    // set notification pointers to NULL
    h->notifications.on_receive = NULL;
    h->notifications.on_connection_state_change= NULL;
    h->notifications.on_diagnostic_notification = NULL;
    h->notifications.on_disconnection_request_received = NULL;
    h->notifications.on_redundancy_diagnostic_notification = NULL;

    h->running_notifications = 0;

    pthread_mutex_init(&h->notification_lock, 0);


    // init the list
    h->connections = rastalist_create(2);

    // init hashing context
    h->hashing_context.hash_length = h->config.values.sending.md4_type;
    h->hashing_context.algorithm = h->config.values.sending.sr_hash_algorithm;

    if (h->hashing_context.algorithm == RASTA_ALGO_MD4){
        // use MD4 IV as key
        rasta_md4_set_key(&h->hashing_context, h->config.values.sending.md4_a, h->config.values.sending.md4_b,
                          h->config.values.sending.md4_c, h->config.values.sending.md4_d);
    } else {
        // use the sr_hash_key
        allocateRastaByteArray(&h->hashing_context.key, sizeof(unsigned int));

        // convert unsigned in to byte array
        h->hashing_context.key.bytes[0] = (h->config.values.sending.sr_hash_key >> 24) & 0xFF;
        h->hashing_context.key.bytes[1] = (h->config.values.sending.sr_hash_key >> 16) & 0xFF;
        h->hashing_context.key.bytes[2] = (h->config.values.sending.sr_hash_key >> 8) & 0xFF;
        h->hashing_context.key.bytes[3] = (h->config.values.sending.sr_hash_key) & 0xFF;
    }

    //setup thread data
    h->recv_running = 0;
    h->send_running = 0;
    h->hb_running = 0;

    h->receive_handle = rmalloc(sizeof(struct rasta_receive_handle));
    h->heartbeat_handle = rmalloc(sizeof(struct rasta_heartbeat_handle));
    h->send_handle = rmalloc(sizeof(struct rasta_sending_handle));

    //receive
    h->receive_handle->config = h->config.values.sending;
    h->receive_handle->info = h->config.values.general;
    h->receive_handle->handle = h;
    h->receive_handle->connections = &h->connections;
    h->receive_handle->running = &h->recv_running;
    h->receive_handle->logger = &h->logger;
    h->receive_handle->mux = &h->mux;
    h->receive_handle->hashing_context = &h->hashing_context;

    //send
    h->send_handle->config = h->config.values.sending;
    h->send_handle->info = h->config.values.general;
    h->send_handle->handle = h;
    h->send_handle->connections = &h->connections;
    h->send_handle->running = &h->send_running;
    h->send_handle->logger = &h->logger;
    h->send_handle->mux = &h->mux;
    h->send_handle->hashing_context = &h->hashing_context;

    //heartbeat
    h->heartbeat_handle->config = h->config.values.sending;
    h->heartbeat_handle->info = h->config.values.general;
    h->heartbeat_handle->handle = h;
    h->heartbeat_handle->connections = &h->connections;
    h->heartbeat_handle->running = &h->hb_running;
    h->heartbeat_handle->logger = &h->logger;
    h->heartbeat_handle->mux = &h->mux;
    h->heartbeat_handle->hashing_context = &h->hashing_context;

    h->receive_handle->accepted_version = accepted_versions;
}

void rasta_handle_init(struct rasta_handle *h, const char* config_file_path) {

    h->config = config_load(config_file_path);

    // load logger configuration
    struct DictionaryEntry logger_ty = config_get(&h->config, RASTA_CONFIG_KEY_LOGGER_TYPE);
    struct DictionaryEntry logger_maxlvl = config_get(&h->config,
                                                      RASTA_CONFIG_KEY_LOGGER_MAX_LEVEL);
    struct DictionaryEntry logger_file = config_get(&h->config, RASTA_CONFIG_KEY_LOGGER_FILE);

    if (logger_ty.type == DICTIONARY_NUMBER && logger_maxlvl.type == DICTIONARY_NUMBER) {
        h->logger = logger_init((log_level) logger_maxlvl.value.number, (logger_type) logger_ty.value.number);

        //h->redlogger = logger_init(LOG_LEVEL_NONE,LOGGER_TYPE_CONSOLE);
        h->redlogger = h->logger;

        if (h->logger.type == LOGGER_TYPE_FILE) {
            // need to set log file
            if (logger_file.type == DICTIONARY_STRING) {
                logger_set_log_file(&h->logger, logger_file.value.string.c);
            } else {
                // error in config
                exit(1);
            }
        }
    } else {
        // error in config
        exit(1);
    }

    // get accepted versions from config
    struct DictionaryEntry config_accepted_version = config_get(&h->config,
                                                                RASTA_CONFIG_KEY_ACCEPTED_VERSIONS);


    // set notification pointers to NULL
    h->notifications.on_receive = NULL;
    h->notifications.on_connection_state_change= NULL;
    h->notifications.on_diagnostic_notification = NULL;
    h->notifications.on_disconnection_request_received = NULL;
    h->notifications.on_redundancy_diagnostic_notification = NULL;

    h->running_notifications = 0;

    pthread_mutex_init(&h->notification_lock, 0);


    // init the list
    h->connections = rastalist_create(2);

    // init hashing context
    h->hashing_context.hash_length = h->config.values.sending.md4_type;
    h->hashing_context.algorithm = h->config.values.sending.sr_hash_algorithm;

    if (h->hashing_context.algorithm == RASTA_ALGO_MD4){
        // use MD4 IV as key
        rasta_md4_set_key(&h->hashing_context, h->config.values.sending.md4_a, h->config.values.sending.md4_b,
                          h->config.values.sending.md4_c, h->config.values.sending.md4_d);
    } else {
        // use the sr_hash_key
        allocateRastaByteArray(&h->hashing_context.key, sizeof(unsigned int));

        // convert unsigned in to byte array
        h->hashing_context.key.bytes[0] = (h->config.values.sending.sr_hash_key >> 24) & 0xFF;
        h->hashing_context.key.bytes[1] = (h->config.values.sending.sr_hash_key >> 16) & 0xFF;
        h->hashing_context.key.bytes[2] = (h->config.values.sending.sr_hash_key >> 8) & 0xFF;
        h->hashing_context.key.bytes[3] = (h->config.values.sending.sr_hash_key) & 0xFF;
    }

    //setup thread data
    h->recv_running = 0;
    h->send_running = 0;
    h->hb_running = 0;

    h->receive_handle = rmalloc(sizeof(struct rasta_receive_handle));
    h->heartbeat_handle = rmalloc(sizeof(struct rasta_heartbeat_handle));
    h->send_handle = rmalloc(sizeof(struct rasta_sending_handle));

    //receive
    h->receive_handle->config = h->config.values.sending;
    h->receive_handle->info = h->config.values.general;
    h->receive_handle->handle = h;
    h->receive_handle->connections = &h->connections;
    h->receive_handle->running = &h->recv_running;
    h->receive_handle->logger = &h->logger;
    h->receive_handle->mux = &h->mux;
    h->receive_handle->hashing_context = &h->hashing_context;

    //send
    h->send_handle->config = h->config.values.sending;
    h->send_handle->info = h->config.values.general;
    h->send_handle->handle = h;
    h->send_handle->connections = &h->connections;
    h->send_handle->running = &h->send_running;
    h->send_handle->logger = &h->logger;
    h->send_handle->mux = &h->mux;
    h->send_handle->hashing_context = &h->hashing_context;

    //heartbeat
    h->heartbeat_handle->config = h->config.values.sending;
    h->heartbeat_handle->info = h->config.values.general;
    h->heartbeat_handle->handle = h;
    h->heartbeat_handle->connections = &h->connections;
    h->heartbeat_handle->running = &h->hb_running;
    h->heartbeat_handle->logger = &h->logger;
    h->heartbeat_handle->mux = &h->mux;
    h->heartbeat_handle->hashing_context = &h->hashing_context;


    if (config_accepted_version.type == DICTIONARY_ARRAY) {
        h->receive_handle->accepted_version = allocate_DictionaryArray(config_accepted_version.value.array.count);
        for (unsigned int i = 0; i < config_accepted_version.value.array.count; ++i) {
            logger_log(&h->logger, LOG_LEVEL_DEBUG, "RaSTA HANDLE_INIT", "Loaded accepted version: %s", config_accepted_version.value.array.data[i].c);
            struct DictionaryString s;
            rmemcpy(s.c, config_accepted_version.value.array.data[i].c, 256);
            h->receive_handle->accepted_version.data[i] = s;
        }
    }


}
