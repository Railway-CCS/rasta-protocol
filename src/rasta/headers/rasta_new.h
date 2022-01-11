//
// Created by tobia on 24.02.2018.
//

#ifndef LST_SIMULATOR_RASTA_NEW_H
#define LST_SIMULATOR_RASTA_NEW_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

//TODO: check
//#include <errno.h>
#include "rastahandle.h"
#include "event_system.h"

/**
 * size of ring buffer where data is hold for retransmissions
 */
#define MAX_QUEUE_SIZE 10 // TODO: maybe in config file

/**
 * the maximum length of application messages in the data of a RaSTA packet.
 * Length of a SCI PDU is max. 44 bytes
 */
#define MAX_APP_MSG_LEN 60

/**
 * maximum length of a RaSTA packet (16 byte MD4 + 5 * 44 bytes of app messages)
 */
#define MAX_PACKET_LEN 264

/**
 * the RaSTA version that is implemented
 */
#define RASTA_VERSION "0303"

#define DIAGNOSTIC_INTERVAL_SIZE 500

/**
 * The config key for the value of the initial sequence number of the SR layer
 */
#define RASTA_CONFIG_KEY_INITIAL_SEQ_NUM "RASTA_INITIAL_SEQ"

/**
 * Reasons for DiscReq as specified in 5.4.6
 */
typedef enum {
    /**
     * Disconnection because of user request
     */
            RASTA_DISC_REASON_USERREQUEST =0,
    /**
     * Disconnection because of receiving an unexpected type of packet
     */
            RASTA_DISC_REASON_UNEXPECTEDTYPE =2,
    /**
     * Disconnection because of an error in the sequence number check
     */
            RASTA_DISC_REASON_SEQNERROR=3,
    /**
     * Disconnection because of a timeout
     */
            RASTA_DISC_REASON_TIMEOUT=4,
    /**
     * Disconnection because of the call of the service was not allowed
     */
            RASTA_DISC_REASON_SERVICENOTALLOWED=5,
    /**
     * Disconnection because of the version was not accepted
     */
            RASTA_DISC_REASON_INCOMPATIBLEVERSION=6,
    /**
     * Disconnection because retransmission failed
     */
            RASTA_DISC_REASON_RETRFAILED=7,
    /**
     * Disconnection because an error in the protocol flow
     */
            RASTA_DISC_REASON_PROTOCOLERROR=8
} rasta_disconnect_reason;



typedef struct {
    unsigned long id;
    struct RastaByteArray appMessage;
}rastaApplicationMessage;





/**
 * initializes the rasta handle and starts all threads
 * configuration is loaded from file
 * @param handle
 * @param config_file_path
 * @param listenports
 * @param port_count
 */
void sr_init_handle(struct rasta_handle* handle, const char* config_file_path);

/**
 * initializes the rasta handle and starts all threads
 * configuration is loaded from parameters
 * @param handle
 * @param configuration RastaConfigInfo - all values must be set
 * @param accepted_version an initialized DictionaryArray with all version accepted by this handle
 * @param logger the initialized logger
 */
void sr_init_handle_manually(struct rasta_handle *handle, struct RastaConfigInfo configuration, struct DictionaryArray accepted_version, struct logger_t logger);

/**
 * connects to another rasta instance
 * @param handle
 * @param id
 */
void sr_connect(struct rasta_handle *handle, unsigned long id, struct RastaIPData *channels);

/**
 * send data to another instance
 * @param h
 * @param remote_id
 * @param app_messages
 */
void sr_send(struct rasta_handle *h, unsigned long remote_id, struct RastaMessageData app_messages);


/**
 * get data from message buffer
 * this is used in the onReceive Event to get the received message
 * @param h
 * @param connection
 * @return the applicationmessage, where id is the sender rasta id and appMessage is the received data
 */
rastaApplicationMessage sr_get_received_data(struct rasta_handle *h, struct rasta_connection * connection);

/**
 * closes the connection to the given remote_id
 * @param h
 * @param remote_id
 */
void sr_disconnect(struct rasta_handle *h, unsigned long remote_id);

/**
 * used to end all threads an free assigned ressources
 * always use this when a programm terminates otherwise it may not start again
 * @param h
 */
void sr_cleanup(struct rasta_handle *h);

void sr_begin(struct rasta_handle * h, fd_event * extern_fd_events, int len);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTA_NEW_H
