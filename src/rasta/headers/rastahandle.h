//
// Created by tobia on 22.03.2018.
//

#ifndef LST_SIMULATOR_RASTAHANDLE_H
#define LST_SIMULATOR_RASTAHANDLE_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <mqueue.h>
#include "rastalist.h"
#include <rastahashing.h>
//TODO: check
//#include <stdint.h>
#include "rastafactory.h"
#include "logging.h"
#include "config.h"
#include "rasta_red_multiplexer.h"

/**
 * struct that is returned in all notifications
 */
struct rasta_notification_result {
    /**
     * copy of the calling rasta connection (this should always be used first)
     */
    struct rasta_connection connection;

    /**
     * handle, don't touch
     */
    struct rasta_handle *handle;
};

/**
 * pointer to a function that will be called when application messages are ready for processing
 * first parameter is the connection that fired the event
 */
typedef void(*on_receive_ptr)(struct rasta_notification_result *result);

/**
 * pointer to a function that will be called when connection state has changed
 * first parameter is the connection that fired the event
 */
typedef void(*on_connection_state_change_ptr)(struct rasta_notification_result *result);

/**
 * pointer to a function that will be called when diagnostic notification will be send
 * first parameter is the connection that fired the event
 * second parameter is the length for the provided array
 * third parameter it the array with tracked diagnostic data
 */
typedef void(*on_diagnostic_notification_ptr)(struct rasta_notification_result *result);

/**
 * pointer to a function that will be called when a DiscReq are received
 * first parameter is the connection that fired the event.
 * second parameter is the reason for this DiscReq
 * third parameter is the detail for this DiscReq
 */
typedef void(*on_disconnection_request_received_ptr)(struct rasta_notification_result *result, unsigned short reason, unsigned short detail);

/**
 * pointer to a function that will be called when an entity successfully completed the handshake and is now in state UP.
 * first parameter is the connection that fired the event
 */
typedef void(*on_handshake_complete_ptr)(struct rasta_notification_result *);

/**
 * pointer to a function that will be called when the T_i timer of an entity expired.
 * first parameter is the connection that fired the event
 */
typedef void(*on_heartbeat_timeout_ptr)(struct rasta_notification_result *);

/**
 * function pointers for the notifications that are specified in 5.2.2
 */
struct rasta_notification_ptr{
    /**
     * called when a application message is ready for processing
     */
    on_receive_ptr on_receive;

    /**
     * called when connection state has changed
     */
    on_connection_state_change_ptr  on_connection_state_change;

    /**
     * called when diagnostic notification will be send
     */
    on_diagnostic_notification_ptr on_diagnostic_notification;

    /**
     * called when a DiscReq are received
     */
    on_disconnection_request_received_ptr on_disconnection_request_received;

    /**
     * called when a diagnostic notification of the redundancy layer occures
     */
    on_diagnostics_available_red_ptr on_redundancy_diagnostic_notification;

    /**
     * called when an entity successfully completed the handshake and is now in state UP
     */
    on_handshake_complete_ptr on_handshake_complete;

    /**
     * called when the T_i timer of an entity expired
     */
    on_heartbeat_timeout_ptr on_heartbeat_timeout;
};

struct rasta_disconnect_notification_result {
    struct rasta_notification_result result;
    unsigned short reason;
    unsigned short detail;
};



struct rasta_sending_handle {
    /**
 * configuration values
 */
    struct RastaConfigInfoSending config;
    struct RastaConfigInfoGeneral info;

    struct logger_t *logger;

    struct redundancy_mux *mux;

    /**
     * handle for notification only
     */
    struct rasta_handle * handle;

    /**
     * pointer to the connections
     */
    struct RastaList *connections;

    int *running;

    /**
     * The paramenters that are used for SR checksums
     */
    rasta_hashing_context_t * hashing_context;
};

struct rasta_heartbeat_handle {
    /**
 * configuration values
 */
    struct RastaConfigInfoSending config;
    struct RastaConfigInfoGeneral info;

    struct logger_t *logger;

    struct redundancy_mux *mux;

    /**
     * handle for notification only
     */
    struct rasta_handle * handle;

    /**
     * pointer to the connections
     */
    struct RastaList *connections;

    int *running;

    /**
     * The paramenters that are used for SR checksums
     */
    rasta_hashing_context_t * hashing_context;
};

struct rasta_receive_handle {
    /**
     * configuration values
     */
    struct RastaConfigInfoSending config;
    struct RastaConfigInfoGeneral info;
    struct DictionaryArray accepted_version;

    struct logger_t *logger;

    struct redundancy_mux *mux;

    /**
     * handle for notification only
     */
    struct rasta_handle * handle;

    /**
     * pointer to the connections
     */
    struct RastaList *connections;

    int *running;

    /**
     * The paramenters that are used for SR checksums
     */
    rasta_hashing_context_t * hashing_context;

};

struct rasta_handle {
    /**
    * the receiving data
    */
    struct rasta_receive_handle *receive_handle;
    int recv_running;

    /**
     * the sending data
     */
    struct rasta_sending_handle *send_handle;
    int send_running;

    /**
     * the heartbeat data
     */
    struct rasta_heartbeat_handle *heartbeat_handle;
    int hb_running;

    /**
     * literall events used in the event loop
     */
    event_container events;

    /**
    * pointers to functions that will be called on notifications as described in 5.2.2 and 5.5.6.4
    */
    struct rasta_notification_ptr notifications;

    /**
    * the logger which is used to log protocol activities
    */
    struct logger_t logger;

    struct logger_t redlogger;

    /**
     * RaSTA parameters that have been loaded from file
    */
    struct RastaConfig config;


    /**
     * versions that this RaSTA entity will accept during the handshake
     */
    //struct DictionaryArray accepted_version;


    /**
     * provides access to the redundancy layer
     */
    struct redundancy_mux mux;

    /**
     * array of rasta conenctions
     */
    struct RastaList connections;

    /**
     * The paramenters that are used for SR checksums
     */
    rasta_hashing_context_t hashing_context;
};
/**
 * creates the container for all notification events
 * @param handle
 * @param connection
 * @return
 */
struct rasta_notification_result sr_create_notification_result(struct rasta_handle *handle, struct rasta_connection *connection);

/**
 * fires the onConnectionStateChange event set in the rasta handle
 * @param result
 */
void fire_on_connection_state_change(struct rasta_notification_result result);

/**
 * fires the onReceive event set in the rasta handle
 * @param result
 */
void fire_on_receive(struct rasta_notification_result result);

/**
 * fires the onDisconnectionRequest event set in the rasta handle
 * @param result
 * @param data
 */
void fire_on_discrequest_state_change(struct rasta_notification_result result, struct RastaDisconnectionData data);

/**
 * fires the onDiagnosticAvailable event set in the rasta handle
 * @param result
 */
void fire_on_diagnostic_notification(struct rasta_notification_result result);

/**
 * fires the onHandshakeComplete event set in the rasta handle
 * @param result
 */
void fire_on_handshake_complete(struct rasta_notification_result result);

/**
 * fires the onHeartbeatTimeout event set in the rasta handle
 * @param result
 */
void fire_on_heartbeat_timeout(struct rasta_notification_result result);

/**
 * initializes the rasta handle
 * configurates itself with the given config file automatically
 * @param h
 * @param config_file_path
 */
void rasta_handle_init(struct rasta_handle *h, const char* config_file_path);

/**
 * initializes the rasta handle
 * configurateable through parameters
 * @param h
 * @param configuration
 * @param accepted_versions
 * @param logger
 */
void rasta_handle_manually_init(struct rasta_handle *h, struct RastaConfigInfo configuration, struct DictionaryArray accepted_versions , struct logger_t logger);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTAHANDLE_H
