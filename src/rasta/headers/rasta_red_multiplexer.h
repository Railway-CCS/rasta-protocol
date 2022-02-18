#ifndef LST_SIMULATOR_RASTA_RED_MULTIPLEXER_H
#define LST_SIMULATOR_RASTA_RED_MULTIPLEXER_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <stdint.h>
#include <event_system.h>
#include "rastamodule.h"
#include "rastaredundancy_new.h"

/**
 * define struct as type here to allow usage in notification pointers
 */
typedef struct redundancy_mux redundancy_mux;

struct receive_event_data {
    fd_event * event;
    struct rasta_handle * h;
    int channel_index;
};

/**
 * pointer to a function that will be called in a separate thread when a new entity has sent data to this entity
 * first parameter is the redundancy multiplexer that fired the event
 * 2nd parameter is id of the new redundancy channel / entity
 */
typedef void(*on_new_connection_ptr)(redundancy_mux *, unsigned long);

/**
 * pointer to a function that will be called in a separate thread when diagnostic data is available
 * first parameter is the redundancy multiplexer that fired the event
 * 2nd parameter is N_Diagnose
 * 3rd parameter is N_missed
 * 4th parameter is T_drift
 * 5th parameter is T_drift2
 * 6th parameter is id of redundancy channel where the notification originates
 */
typedef void(*on_diagnostics_available_red_ptr)(redundancy_mux *, int, int, unsigned long, unsigned long,
                                                unsigned long);

/**
 * representation of the notifications that are available for the redundancy layer
 */
typedef struct {
    /**
     * the assigned function is called when diagnose information of a transport channel is available.
     * The function will be called in a separate thread
     */
    on_diagnostics_available_red_ptr on_diagnostics_available;

    /**
     * the assigned function is called when a new redundancy channel has sent data to this entity.
     * The function will be called in a separate thread
     */
    on_new_connection_ptr  on_new_connection;
}rasta_redundancy_notifications;

struct timeout_event_data {
    timed_event * event;
    redundancy_mux * mux;
};

/**
 * initialize the event handling channel timeouts and the corresponding carry data
 * @param event the event to initialize
 * @param carry_data the carry data to initialize
 * @param mux the redundancy_mux that will contain channels
 */
void init_timeout_events(timed_event * event, struct timeout_event_data * t_data, struct redundancy_mux * mux);

/**
 * representation of a redundancy layer multiplexer.
 * is used to handle multiple redundancy channels.
 */
struct redundancy_mux{
    /**
     * the ports where this entity will listen (where the udp sockets are bound)
     */
    uint16_t * listen_ports;

    /**
     * amount of listen ports, i.e. length of the listen_ports array
     */
    unsigned int port_count;

    /**
     * the file descriptors of the used udp sockes. array has length port_count
     */
    int * udp_socket_fds;

    /**
     * the redundancy channels to remote entities this multiplexer is aware of
     */
    rasta_redundancy_channel * connected_channels;

    /**
     * the amount of known redundancy channels, i.e. the length of connected_channels
     */
    unsigned int channel_count;

    /**
     * 1 if the redundancy multiplexer is active, 0 otherwise
     */
    int is_open;

    /**
     * the logger that is used to log information
     */
    struct logger_t logger;

    /**
     * configuration data for the multiplexer and redundancy channels
     */
    struct RastaConfigInfo config;

    /**
     * the notifications of this multiplexer and it's redundancy channels
     */
    rasta_redundancy_notifications notifications;

    /**
     * amount of notification thread that are currently running
     */
     unsigned int notifications_running;

     /**
      * Hashing paramenter for SR layer checksum
      */
      rasta_hashing_context_t sr_hashing_context;
};

/**
 * initializes an redundancy layer multiplexer
 * @param logger the logger that is used to log information
 * @param listen_ports the ports where data can be received
 * @param port_count the amount of elements in @p listen_ports
 * @param config configuration for redundancy channels
 * @return an initialized redundancy layer multiplexer
 */
redundancy_mux redundancy_mux_init(struct logger_t logger, uint16_t * listen_ports, unsigned int port_count, struct RastaConfigInfo config);

/**
 * initializes an redundancy layer multiplexer. The ports and interfaces to listen on are read from the config.
 * @param logger the logger that is used to log information
 * @param config configuration for redundancy channels
 * @return an initialized redundancy layer multiplexer
 */
redundancy_mux redundancy_mux_init_(struct logger_t logger, struct RastaConfigInfo config);
/**
 * starts the redundancy layer multiplexer and opens (if specified) all redundancy channels
 * @param mux the multiplexer that will be opened
 */
void redundancy_mux_open(redundancy_mux * mux);

/**
 * stops the redundancy layer multiplexer and closes all redundancy channels before cleaning up memory
 * @param mux the multiplexer that will be closed
 */
void redundancy_mux_close(redundancy_mux * mux);

int channel_receive_event(void * carry_data);

/**
 * getter for a redundancy channel
 * @param mux the redundancy multiplexer that contains the channel
 * @param id the RaSTA ID that is associated with the channel
 * @return the channel or if ID is unknown NULL
 */
rasta_redundancy_channel * redundancy_mux_get_channel(redundancy_mux * mux, unsigned long id);

/**
 * setter for the RaSTA ID of the redundancy channel, that has been specified in the config.
 * This is only necessary as a client where the redundancy channel to a server is specified in the config file
 * @param mux the mux where the ID will be set
 * @param id the RaSTA ID of the config channel
 */
void redundancy_mux_set_config_id(redundancy_mux * mux, unsigned long id);

/**
 * sends data to a known redundancy channel. The channel where the data is sent is identified by the receiver id field
 * in the @p data PDU
 * @param mux the multiplexer which will try to send the @p data
 * @param data the PDU which will be sent to data#receiver_id
 */
void redundancy_mux_send(redundancy_mux * mux, struct RastaPacket data);

/**
 * retrieves a message from the queue of the redundancy channel to entity with RaSTA ID @p id.
 * If the queue is empty, this call will block until a message is available.
 * If the id is unknown, this call will block until a redundancy connection to the entity is known
 * @param mux the multiplexer that is used
 * @param id the RaSTA ID of the entity whose message is retrieved
 * @return the oldest (i.e. the PDU that arrived first) SR layer PDU in the receive buffer of the redundacy channel
 */
int redundancy_try_mux_retrieve(redundancy_mux * mux, unsigned long id, struct RastaPacket * out);

/**
 * blocks until all notification threads are finished
 * @param mux the multiplexer that is used
 */
void redundancy_mux_wait_for_notifications(redundancy_mux * mux);

/**
 * blocks until an entity with RaSTA ID @p id is discovered (i.e. the multiplexer has received something from that entity)
 * @param mux the multiplexer that is used
 * @param id the RaSTA ID of the entity
 */
void redundancy_mux_wait_for_entity(redundancy_mux * mux, unsigned long id);

/**
 * adds a new redundancy channel to the multiplexer id and given transport channels.
 * The size of the transport channel array hast to be mux#port_count
 * @param id the RaSTA ID that will identify the new redundancy channel (ID of remote partner)
 * @param mux the multiplexer where the new redundancy channel is added
 * @param transport_channels the transport channels of the new redundancy channel
 */
void redundancy_mux_add_channel(redundancy_mux * mux, unsigned long id, struct RastaIPData * transport_channels);

/**
 * removes an existing redundancy channel from the multiplexer if the channels exists. If the channel with the given
 * ID does not exist in the mux, nothing happens
 * @param mux the multiplexer where the new redundancy channel is removed
 * @param channel_id the RaSTA ID that identifies the redundancy channel (ID of remote partner)
 */
void redundancy_mux_remove_channel(redundancy_mux * mux, unsigned long channel_id);

/**
 * block until a PDU is available in any of the connected redundancy channels
 * @param mux the multiplexer that is used
 * @return a packet that was received in any of the connected redundancy channels
 */
int redundancy_mux_try_retrieve_all(redundancy_mux * mux, struct RastaPacket* out);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTA_RED_MULTIPLEXER_H
