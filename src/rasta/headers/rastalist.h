//
// Created by tobia on 05.02.2018.
//

#ifndef LST_SIMULATOR_RASTALIST_H
#define LST_SIMULATOR_RASTALIST_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <stdint.h>
#include "event_system.h"
#include "fifo.h"

typedef enum {
    RASTA_ROLE_CLIENT = 0,
    RASTA_ROLE_SERVER = 1
} rasta_role;

/**
 * representation of the connection state in the SR layer
 */
typedef enum {
    /**
     * The connection is closed
     */
            RASTA_CONNECTION_CLOSED,
    /**
     * OpenConnection was called, the connection is ready to be established
     */
            RASTA_CONNECTION_DOWN,
    /**
     * In case the role is client: a ConReq was sent, waiting for ConResp
     * In case the role is server: a ConResp was sent, waiting for HB
     */
            RASTA_CONNECTION_START,
    /**
     * The connection was established, ready to send data
     */
            RASTA_CONNECTION_UP,
    /**
     * Retransmission requested
     * RetrReq was sent, waiting for RetrResp
     */
            RASTA_CONNECTION_RETRREQ,
    /**
     * Retransmission running
     * After receiving the RetrResp, resend data will be accepted until HB or regular data arrives
     */
            RASTA_CONNECTION_RETRRUN
} rasta_sr_state;

/**
* representation of the RaSTA error counters, as specified in 5.5.5
*/
struct rasta_error_counters{
    /**
     * received message with faulty checksum
     */
    unsigned int safety;

    /**
     * received message with unreasonable sender/receiver
     */
    unsigned int address;

    /**
     * received message with undefined type
     */
    unsigned int type;

    /**
     * received message with unreasonable sequence number
     */
    unsigned int sn;

    /**
     * received message with unreasonable confirmed sequence number
     */
    unsigned int cs;
};

/**
 * Representation of a sub interval defined in 5.5.6.4 and used to diagnose healthiness of a connection
 */
struct diagnostic_interval {
    /**
     * represents the start point of this interval.
     * an interval lies between 0 to T_MAX
     */
    unsigned int interval_start;
    /**
     * represents the end point for this interval
     * an interval lies between 0 to T_MAX
     */
    unsigned int interval_end;

    /**
     * counts successful reached messages that lies between current interval_start and interval_end
     */
    unsigned int message_count;
    /**
     * counts every message assigned to this interval that's T_ALIVE value lies between this interval, too
     */
    unsigned int t_alive_message_count;
};

/**
 * The data that is passed to most timed events.
 */
struct timed_event_data {
    void * handle;
    int connection_index;
};

struct rasta_connection{

    /**
     * the event operating the heartbeats on this connection
     */
    timed_event send_heartbeat_event;
    struct timed_event_data heartbeat_carry_data;

    /**
     * the event watching the connection timeout
     */
    timed_event timeout_event;
    struct timed_event_data timeout_carry_data;

    /**
     * 1 if the process for sending heartbeats should be paused, otherwise 0
     */
    int hb_stopped;

    /**
     * bool value if data from the send buffer is sent right now
     */
    int is_sending;

    /**
     * blocks heartbeats until connection handshake is complete
     */
    int hb_locked;

    rasta_sr_state current_state;

    /**
     * the name of the receiving message queue
     */
    fifo_t * fifo_app_msg;

    /**
     * the name of the sending message queue
     */
    fifo_t * fifo_send;

    /**
     * the N_SENDMAX of the connection partner,  -1 if not connected
     */
    int connected_recv_buffer_size;

    /**
     * defines if who started the connection
     * Client: Connection request sent
     * Server: Connection request received
     */
    rasta_role role;

    /**
     * send sequence number (seq nr of the next PDU that will be sent)
     */
    uint32_t sn_t;
    /**
     * receive sequence number (expected seq nr of the next received PDU)
     */
    uint32_t sn_r;
    /**
     * sequence number that has to be checked in the next sent PDU
     */
    uint32_t cs_t;
    /**
     * last received, checked sequence number
     */
    uint32_t cs_r;

    /**
     * timestamp of the last received relevant message
     */
    uint32_t ts_r;
    /**
     * checked timestamp of the last received relevant message
     */
    uint32_t cts_r;

    /**
     * relative time used to monitor incoming messages
     */
    unsigned int t_i;

    /**
     * the RaSTA connections sender identifier
     */
    uint32_t my_id;
    /**
     * the RaSTA connections receiver identifier
     */
    uint32_t remote_id;
    /**
     * the identifier of the RaSTA network this connection belongs to
     */
    uint32_t network_id;

    /**
     * counts received diagnostic relevant messages since last diagnosticNotification
     */
    unsigned int received_diagnostic_message_count;
    /**
     * length of diagnostic_intervals array
     */
    unsigned int diagnostic_intervals_length;
    /**
     * diagnostic intervals defined at 5.5.6.4 to diagnose healthiness of this connection
     * number of fields defined by DIAGNOSTIC_INTERVAL_SIZE
     */
    struct diagnostic_interval* diagnostic_intervals;

    /**
     * the pdu fifo for retransmission purposes
     */
    fifo_t * fifo_retr;

    /**
    *   the error counters as specified in 5.5.5
    */
    struct rasta_error_counters errors;
};

struct RastaList {
    struct rasta_connection *data;

    unsigned int size;
    unsigned int actual_size;
};

/**
 * adds a rasta_connection (item) to the end of the list (list)
 * NOTE: the type of the list must be RASTA_LIST_SR
 * @param list
 * @param item
 */
int rastalist_addConnection(struct RastaList *list, struct rasta_connection item);

/**
 * removes the given id from the list
 * @param list
 * @param id
 */
void rastalist_remove(struct RastaList *list, unsigned int id);

/**
 * returns the amount of elements in the list
 * @param list
 * @return
 */
unsigned int rastalist_count(struct RastaList *list);
/**
 * returns the item on id (id)
 * NOTE: the type of the list must be RASTA_LIST_SR and id should be in range. Otherwise remote_id is set to 0
 * @param list
 * @param id
 * @return
 */
struct rasta_connection * rastalist_getConnection(struct RastaList *list, unsigned int id);

/**
 * returns the connection with remote_id
 * @param list
 * @param remote_id
 * @return
 */
struct rasta_connection * rastalist_getConnectionByRemote(struct RastaList *list, unsigned long remote_id);

/**
 * returns the id of the connection with remote_id. If not in list return -1
 * @param list
 * @param remote_id
 * @return
 */
int rastalist_getConnectionId(struct RastaList *list, unsigned long remote_id);


/**
 * returns a allocated list
 * @param initial_size should be 2 or bigger (otherwise set to 2)
 * @param type the type the list is set to
 * @return
 */
struct RastaList rastalist_create(unsigned int initial_size);

/**
 * frees the list
 * NOTE: all elements must be freed manually
 * @param list
 */
void rastalist_free(struct RastaList* list);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTALIST_H
