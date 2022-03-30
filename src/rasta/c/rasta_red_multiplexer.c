#include <rasta_red_multiplexer.h>
#include <rastaredundancy_new.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include "rastahandle.h"
#include "event_system.h"
#include "rmemory.h"
#include "udp.h"
#include "rastautil.h"

/* --- Notifications --- */

/**
 * wrapper for parameter in the diagnose notification thread handler
 */
struct diagnose_notification_parameter_wrapper {
    /**
     * the used redundancy multiplexer
     */
    redundancy_mux * mux;

    /**
     * value of N_diagnose
     */
    int n_diagnose;

    /**
     * current value of N_missed
     */
    int n_missed;

    /**
     * current value of T_drift
     */
    unsigned long t_drift;

    /**
     * current value of T_drift2
     */
    unsigned long t_drift2;

    /**
     * associated id of the redundancy channel this notification origins from
     */
    unsigned long channel_id;
};

/**
 * wrapper for parameter in the onNewNotification notification thread handler
 */
struct new_connection_notification_parameter_wrapper{
    /**
     * the used redundancy multiplexer
     */
    redundancy_mux * mux;

    /**
     * the id of the new redundancy channel
     */
    unsigned long id;
};

/**
 * the is the function that handles the call of the onDiagnosticsAvailable notification pointer.
 * this runs on the main thread
 * @param connection the connection that will be used
 * @return unused
 */
void red_on_new_connection_caller(struct new_connection_notification_parameter_wrapper * w) {

    logger_log(&w->mux->logger, LOG_LEVEL_DEBUG, "RaSTA Redundancy onNewConnection caller", "calling onNewConnection function");
    (*w->mux->notifications.on_new_connection)(w->mux, w->id);

    w->mux->notifications_running = (unsigned short)(w->mux->notifications_running -1);
}

/**
 * fires the onDiagnoseAvailable event.
 * This implementation will take care if the function pointer is NULL and start a thread to call the notification
 * @param mux the redundancy multiplexer that is used
 * @param id the id of the new redundacy channel
 */
void red_call_on_new_connection(redundancy_mux * mux, unsigned long id){
    if (mux->notifications.on_new_connection == NULL){
        // notification not set, do nothing
        return;
    }

    mux->notifications_running++;

    struct new_connection_notification_parameter_wrapper* wrapper =
            rmalloc(sizeof(struct new_connection_notification_parameter_wrapper));
    wrapper->mux = mux;
    wrapper->id = id;

    red_on_new_connection_caller(wrapper);
    free(wrapper);

    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA Redundancy call onNewConnection", "called onNewConnection");
}


/**
 * the is the function that handles the call of the onDiagnosticsAvailable notification pointer.
 * this runs on the main thread
 * @param wrapper a wrapper that contains the mux and the diagnose data
 * @return unused
 */
void red_on_diagnostic_caller(struct diagnose_notification_parameter_wrapper * w){
    logger_log(&w->mux->logger, LOG_LEVEL_DEBUG, "RaSTA Redundancy onDiagnostics caller", "calling onDiagnostics function");
    (*w->mux->notifications.on_diagnostics_available)(w->mux, w->n_diagnose, w->n_missed, w->t_drift, w->t_drift2, w->channel_id);

    w->mux->notifications_running = (unsigned short)(w->mux->notifications_running -1);
}

/**
 * fires the onDiagnoseAvailable event.
 * This implementation will take care if the function pointer is NULL and start a thread to call the notification
 * @param mux the redundancy multiplexer that is used
 * @param n_diagnose the value of N_Diagnose
 * @param n_missed the current value of N_missed
 * @param t_drift the current value of T_drift
 * @param t_drift2 the current value of T_drift2
 * @param id the id of the redundancy channel
 */
void red_call_on_diagnostic(redundancy_mux * mux, int n_diagnose,
                          int n_missed, unsigned long t_drift, unsigned long t_drift2, unsigned long id){
    if (mux->notifications.on_diagnostics_available == NULL){
        // notification not set, do nothing
        return;
    }

    mux->notifications_running++;

    struct diagnose_notification_parameter_wrapper wrapper;
    wrapper.mux = mux;
    wrapper.n_diagnose = n_diagnose;
    wrapper.n_missed = n_missed;
    wrapper.t_drift = t_drift;
    wrapper.t_drift2 = t_drift2;
    wrapper.channel_id = id;

    red_on_diagnostic_caller(&wrapper);

    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA Redundancy call onDiagnostics", "called onDiagnostics");
}



/* --------------------- */

/**
 * received data on a UDP socket
 * @param mux the multiplexer that is used
 * @param channel_id the index of the udp socket
 */
void receive_packet(redundancy_mux * mux, int channel_id){
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux receive", "Receive called");

    // receive a packet
    unsigned char * buffer = rmalloc(sizeof(unsigned char) * MAX_DEFER_QUEUE_MSG_SIZE);

    // the sender of the received packet
    struct sockaddr_in sender;

    int fd = mux->udp_socket_fds[channel_id];

    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux receive", "channel %d waiting for data on fd %d...", channel_id, fd);

    // wait for pdu
    size_t len = udp_receive(fd, buffer, MAX_DEFER_QUEUE_MSG_SIZE, &sender);
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux receive", "channel %d received data on upd", channel_id);
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux receive", "channel %d received data len = %d", channel_id, len);

    struct RastaByteArray incomingData;
    incomingData.length = (unsigned int)len;
    incomingData.bytes = buffer;

    rasta_hashing_context_t test;
    struct crc_options options;

    test.hash_length = mux->sr_hashing_context.hash_length;
    test.algorithm = mux->sr_hashing_context.algorithm;
    allocateRastaByteArray(&test.key, mux->sr_hashing_context.key.length);
    rmemcpy(test.key.bytes, mux->sr_hashing_context.key.bytes, mux->sr_hashing_context.key.length);
    rmemcpy(&options, &mux->config.redundancy.crc_type, sizeof(mux->config.redundancy.crc_type));

    struct RastaRedundancyPacket receivedPacket = bytesToRastaRedundancyPacket(incomingData,
            options, &test);

    freeRastaByteArray(&test.key);


    rasta_transport_channel connected_channel;
    connected_channel.ip_address = rmalloc(sizeof(char) * 15);
    sockaddr_to_host(sender,connected_channel.ip_address);
    connected_channel.port = ntohs(sender.sin_port);

    // find assiociated redundancy channel
    for (unsigned int i = 0; i < mux->channel_count; ++i) {
        if (receivedPacket.data.sender_id == mux->connected_channels[i].associated_id){
            // found redundancy channel with associated id
            // need to check if redundancy channel already knows ip & port of sender
            rasta_redundancy_channel channel = mux->connected_channels[i];
            if (channel.connected_channel_count < mux->port_count){
                // not all remote transport channel endpoints discovered

                int is_channel_saved= 0;

                for (unsigned int j = 0; j < channel.connected_channel_count; ++j) {
                    if (channel.connected_channels[j].port == connected_channel.port &&
                        strcmp(connected_channel.ip_address, channel.connected_channels[j].ip_address) == 0){
                        // channel is already saved
                        is_channel_saved = 1;
                    }
                }

                if (!is_channel_saved){
                    // channel wasn't saved yet -> add to list
                    mux->connected_channels[i].connected_channels[channel.connected_channel_count].ip_address = connected_channel.ip_address;
                    mux->connected_channels[i].connected_channels[channel.connected_channel_count].port = connected_channel.port;
                    mux->connected_channels[i].connected_channel_count++;

                    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux receive", "channel %d discovered client transport channel %s:%d for connection to 0x%lX",
                               channel_id, connected_channel.ip_address, connected_channel.port, channel.associated_id);
                } else {
                    // temp channel no longer needed -> free memory
                    rfree(connected_channel.ip_address);
                }
            }


            // call the receive function of the associated channel
            /*logger_log(&mux->logger, LOG_LEVEL_DEBUG, "MUX", "count=%d", mux->channel_count);
            for (int k = 0; k < mux->channel_count; ++k) {
                logger_log(&mux->logger, LOG_LEVEL_DEBUG, "MUX", "channel %d, id=%0x%lX", i, mux->connected_channels[i].associated_id);
            }*/
            rasta_red_f_receive(redundancy_mux_get_channel(mux, receivedPacket.data.sender_id), receivedPacket, channel_id);

            rfree(buffer);
            return;
        }
    }

    // no associated channel found -> received message from new partner
    logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux receive", "received pdu from unknown entity with id=0x%lX", receivedPacket.data.sender_id);
    rasta_redundancy_channel new_channel = rasta_red_init(mux->logger, mux->config, mux->port_count, receivedPacket.data.sender_id);
    new_channel.associated_id = receivedPacket.data.sender_id;
    // add transport channel to redundancy channel
    new_channel.connected_channels[0].ip_address = connected_channel.ip_address;
    new_channel.connected_channels[0].port = connected_channel.port;
    new_channel.connected_channel_count++;

    new_channel.is_open = 1;

    //reallocate memory for new client
    mux->connected_channels = rrealloc(mux->connected_channels, (mux->channel_count + 1) * sizeof(rasta_redundancy_channel));

    mux->connected_channels[mux->channel_count] = new_channel;
    mux->channel_count++;

    // fire new redundancy channel notification
    red_call_on_new_connection(mux, new_channel.associated_id);

    // call receive function of new channel
    rasta_red_f_receive(redundancy_mux_get_channel(mux, new_channel.associated_id), receivedPacket, channel_id);

    // free receive buffer
    rfree(buffer);
}

char channel_receive_event(void* carry_data) {
    struct receive_event_data* data = carry_data;
    struct rasta_handle* h = data->h;
    unsigned int mux_channel_count = h->mux.channel_count;

    for (size_t i = 0; i < mux_channel_count; ++i) {
        rasta_redundancy_channel current = h->mux.connected_channels[i];
        int n_diagnose = h->mux.config.redundancy.n_diagnose;

        unsigned long channel_diag_start_time = current.connected_channels[data->channel_index].diagnostics_data.start_time;

        if (current_ts() - channel_diag_start_time >= (unsigned long)n_diagnose){
            // increase n_missed by amount of messages that are not received

            // amount of missed packets
            int missed_count = current.diagnostics_packet_buffer.count -
                    current.connected_channels[data->channel_index].diagnostics_data.received_packets;

            // increase n_missed
            current.connected_channels[data->channel_index].diagnostics_data.n_missed += missed_count;

            // window finished, fire event
            // fire diagnostic notification
            red_call_on_diagnostic(&h->mux,
                                    h->mux.config.redundancy.n_diagnose,
                                    current.connected_channels[data->channel_index].diagnostics_data.n_missed,
                                    current.connected_channels[data->channel_index].diagnostics_data.t_drift,
                                    current.connected_channels[data->channel_index].diagnostics_data.t_drift2,
                                    current.associated_id);

            // reset values
            current.connected_channels[data->channel_index].diagnostics_data.n_missed = 0;
            current.connected_channels[data->channel_index].diagnostics_data.received_packets = 0;
            current.connected_channels[data->channel_index].diagnostics_data.t_drift = 0;
            current.connected_channels[data->channel_index].diagnostics_data.t_drift2 = 0;
            current.connected_channels[data->channel_index].diagnostics_data.start_time = current_ts();

            deferqueue_clear(&current.diagnostics_packet_buffer);
        }

        // channel count might have changed due to removal of channels
        mux_channel_count = h->mux.channel_count;
    }

    logger_log(&h->mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux receive thread", "Thread %d calling receive",
                data->channel_index);
    receive_packet(&h->mux, data->channel_index);
    logger_log(&h->mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux receive thread", "Thread %d receive done",
                data->channel_index);
    return 0;
}

char channel_timeout_event(void * carry_data) {
    struct timeout_event_data * data = carry_data;
    redundancy_mux* mx = data->mux;
    if (!mx->is_open) return 0;

    unsigned int mux_channel_count = mx->channel_count;

    for (size_t i = 0; i < mux_channel_count; ++i) {
        data->event->interval = 100000;
        rasta_redundancy_channel current_channel = mx->connected_channels[i];

        // get channel information
        unsigned int channel_defer_q_count = current_channel.defer_q.count;
        unsigned int channel_t_seq = current_channel.configuration_parameters.t_seq;
        unsigned long channel_oldest_ts = current_channel.defer_q.elements[0].received_timestamp;

        if(channel_defer_q_count == 0){
            // skip if queue is empty
            continue;
        }
        unsigned long current_time = current_ts();

        // defer queue is sorted, so the first element is always the oldest
        if ((current_time - channel_oldest_ts) > channel_t_seq){
            // timeout, send to next layer

            logger_log(&mx->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux timeout thread", "timout detected for connection %d. calling f_deferTmo", i);
            rasta_red_f_deferTmo(&current_channel);
        } else{
            data->event->interval = 1000 * (channel_t_seq - (current_time - channel_oldest_ts));
        }
    }
    return 0;
}

/**
 * initializes the timeout event
 * @param event the event
 * @param t_data the carry data for the first event
 * @param mux the redundancy multiplexer that will contain the channels
 */
void init_timeout_events(timed_event* event, struct timeout_event_data* t_data, struct redundancy_mux* mux) {
    int open = mux->is_open;
    t_data->mux = mux;
    t_data->event = event;
    event->meta_information.callback = channel_timeout_event;
    event->meta_information.carry_data = t_data;
    event->meta_information.enabled = !open;
    event->interval = 100000;
}

/* ----------------------------*/

redundancy_mux redundancy_mux_init_(struct logger_t logger, struct RastaConfigInfo config){
    redundancy_mux mux;

    mux.logger = logger;
    //mux.listen_ports = listen_ports;
    mux.port_count = config.redundancy.connections.count;
    mux.config = config;

    mux.is_open = 0;
    mux.notifications_running = 0;

    logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "init memory for %d listen ports", mux.port_count);

    // init and bind udp sockets + threads array
    mux.udp_socket_fds = rmalloc(mux.port_count * sizeof(int));


    // allocate memory for connected channels
    mux.connected_channels = rmalloc(sizeof(rasta_redundancy_channel));
    mux.channel_count = 0;

    // init notifications to NULL
    mux.notifications.on_diagnostics_available = NULL;
    mux.notifications.on_new_connection = NULL;

    // load ports that are specified in config
    if (mux.config.redundancy.connections.count > 0){
        logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "loading listen from config");

        mux.listen_ports = rmalloc(sizeof(uint16_t) * mux.config.redundancy.connections.count);
        for (unsigned int j = 0; j < mux.config.redundancy.connections.count; ++j) {
            // init socket
            mux.udp_socket_fds[j] = udp_init();

            // bind socket to device and port
            udp_bind_device(mux.udp_socket_fds[j],
                            (uint16_t )mux.config.redundancy.connections.data[j].port,
                            mux.config.redundancy.connections.data[j].ip);

            mux.listen_ports[j] = (uint16_t )mux.config.redundancy.connections.data[j].port;
        }
    }

    // init hashing context
    mux.sr_hashing_context.hash_length = config.sending.md4_type;
    mux.sr_hashing_context.algorithm = config.sending.sr_hash_algorithm;

    if (mux.sr_hashing_context.algorithm == RASTA_ALGO_MD4){
        // use MD4 IV as key
        rasta_md4_set_key(&mux.sr_hashing_context, config.sending.md4_a, config.sending.md4_b,
                          config.sending.md4_c, config.sending.md4_d);
    } else {
        // use the sr_hash_key
        allocateRastaByteArray(&mux.sr_hashing_context.key, sizeof(unsigned int));

        // convert unsigned in to byte array
        mux.sr_hashing_context.key.bytes[0] = (config.sending.sr_hash_key >> 24) & 0xFF;
        mux.sr_hashing_context.key.bytes[1] = (config.sending.sr_hash_key >> 16) & 0xFF;
        mux.sr_hashing_context.key.bytes[2] = (config.sending.sr_hash_key >> 8) & 0xFF;
        mux.sr_hashing_context.key.bytes[3] = (config.sending.sr_hash_key) & 0xFF;
    }

    logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "initialization done");
    return mux;
}

redundancy_mux redundancy_mux_init(struct logger_t logger, uint16_t * listen_ports, unsigned int port_count, struct RastaConfigInfo config){
    redundancy_mux mux;

    mux.logger = logger;
    mux.listen_ports = listen_ports;
    mux.port_count = port_count;
    mux.config = config;

    mux.is_open = 0;
    mux.notifications_running = 0;

    logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "init memory for %d listen ports", port_count);

    // init and bind udp sockets + threads array
    mux.udp_socket_fds = rmalloc(port_count * sizeof(int));

    // set up udp sockets
    for (unsigned int i = 0; i < port_count; ++i) {
        logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "setting up udp socket %d/%d", i+1,port_count);
        mux.udp_socket_fds[i] = udp_init();
        udp_bind(mux.udp_socket_fds[i], listen_ports[i]);
    }

    // allocate memory for connected channels
    mux.connected_channels = rmalloc(sizeof(rasta_redundancy_channel));
    mux.channel_count = 0;

    // init notifications to NULL
    mux.notifications.on_diagnostics_available = NULL;
    mux.notifications.on_new_connection = NULL;

    // load channel that is specified in config
    if (mux.config.redundancy.connections.count > 0){
        logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "loading redundancy channel from config");
        rasta_redundancy_channel new_channel = rasta_red_init(mux.logger, mux.config, mux.port_count, mux.config.general.rasta_id);
        new_channel.associated_id = 0x0;

        for (unsigned int j = 0; j < mux.config.redundancy.connections.count; ++j) {
            logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "setting up transport channel %d/%d",
                       j+1, mux.config.redundancy.connections.count);
            logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "transport channel: ip=%s, port=%d",
                       mux.config.redundancy.connections.data[j].ip, mux.config.redundancy.connections.data[j].port);
            // no associated channel found -> received message from new partner
            // add transport channel to redundancy channel
            rasta_red_add_transport_channel(&new_channel, mux.config.redundancy.connections.data[j].ip,
                                            (uint16_t )mux.config.redundancy.connections.data[j].port);
        }


        mux.connected_channels[mux.channel_count] = new_channel;
        mux.channel_count++;
    }

    logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "initialization done");
    return mux;
}

redundancy_mux redundancy_mux_init_with_devices(struct logger_t logger, struct RastaIPData * listen_ports, unsigned int port_count, struct RastaConfigInfo config){
    redundancy_mux mux;

    mux.logger = logger;
    mux.listen_ports = listen_ports;
    mux.port_count = port_count;
    mux.config = config;

    mux.is_open = 0;
    mux.notifications_running = 0;

    // initialize the multiplexer mutex

    logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "init memory for %d listen ports", port_count);

    // init and bind udp sockets + threads array
    mux.udp_socket_fds = rmalloc(port_count * sizeof(int));

    // set up udp sockets
    for (unsigned int i = 0; i < port_count; ++i) {
        logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "setting up udp socket %d/%d", i+1,port_count);
        mux.udp_socket_fds[i] = udp_init();
        udp_bind_device(mux.udp_socket_fds[i], (uint16_t)listen_ports[i].port, listen_ports[i].ip);
    }

    // allocate memory for connected channels
    mux.connected_channels = rmalloc(sizeof(rasta_redundancy_channel));
    mux.channel_count = 0;

    // init notifications to NULL
    mux.notifications.on_diagnostics_available = NULL;
    mux.notifications.on_new_connection = NULL;

    // load channel that is specified in config
    if (mux.config.redundancy.connections.count > 0){
        logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "loading redundancy channel from config");
        rasta_redundancy_channel new_channel = rasta_red_init(mux.logger, mux.config, mux.port_count, mux.config.general.rasta_id);
        new_channel.associated_id = 0x0;

        for (unsigned int j = 0; j < mux.config.redundancy.connections.count; ++j) {
            logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "setting up transport channel %d/%d",
                       j+1, mux.config.redundancy.connections.count);
            logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "transport channel: ip=%s, port=%d",
                       mux.config.redundancy.connections.data[j].ip, mux.config.redundancy.connections.data[j].port);
            // no associated channel found -> received message from new partner
            // add transport channel to redundancy channel
            rasta_red_add_transport_channel(&new_channel, mux.config.redundancy.connections.data[j].ip,
                                            (uint16_t )mux.config.redundancy.connections.data[j].port);
        }


        mux.connected_channels[mux.channel_count] = new_channel;
        mux.channel_count++;
    }

    logger_log(&mux.logger, LOG_LEVEL_DEBUG, "RaSTA RedMux init", "initialization done");
    return mux;
}

void redundancy_mux_close(redundancy_mux * mux){
    // set flag to 0, will cause the threads to stop and cleanup before exiting
    mux->is_open = 0;

    // close the sockets of the transport channels
    for (unsigned int i = 0; i < mux->port_count; ++i) {
        logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux close", "closing udp socket %d/%d", i+1, mux->port_count);
        udp_close(mux->udp_socket_fds[i]);
    }

    // free arrays
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux close", "freeing thread data");
    rfree(mux->udp_socket_fds);
    mux->port_count = 0;

    // close the redundancy channels
    for (unsigned int j = 0; j < mux->channel_count; ++j) {
        logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux close", "cleanup connected channel %d/%d", j+1, mux->channel_count);
        rasta_red_cleanup(&mux->connected_channels[j]);
    }
    rfree(mux->connected_channels);

    freeRastaByteArray(&mux->sr_hashing_context.key);

    logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux close", "redundancy multiplexer closed");
}

rasta_redundancy_channel * redundancy_mux_get_channel(redundancy_mux * mux, unsigned long id){
    // iterate over all known channels
    for (unsigned int i = 0; i < mux->channel_count; ++i) {
        // check if channel id == wanted id
        if (mux->connected_channels[i].associated_id == id){
            return &mux->connected_channels[i];
        }
    }

    // wanted id is unknown, return NULL
    return NULL;
}

void redundancy_mux_set_config_id(redundancy_mux * mux, unsigned long id){
    // only set if a channel is available
    if (mux->channel_count > 0){
        mux->connected_channels[0].associated_id = id;
    }
}

void redundancy_mux_send(redundancy_mux * mux, struct RastaPacket data){
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux send", "sending a data packet to id 0x%lX", data.receiver_id);

    // get the channel to the remote entity by the data's received_id
    rasta_redundancy_channel * receiver = redundancy_mux_get_channel(mux, data.receiver_id);

    if (receiver == NULL){
        logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux send", "redundancy channel with id=0x%lX unknown", data.receiver_id);
        // not receiver found
        return;
    }
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux send", "current seq_tx=%d", receiver->seq_tx);

    // create packet to send and convert to byte array
    struct RastaRedundancyPacket packet = createRedundancyPacket(receiver->seq_tx, data, mux->config.redundancy.crc_type);
    struct RastaByteArray data_to_send = rastaRedundancyPacketToBytes(packet, &receiver->hashing_context);

    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux send", "redundancy packet created");

    // increase seq_tx
    receiver->seq_tx = receiver->seq_tx +1;

    // send on every transport channels
    rasta_transport_channel channel;
    for (unsigned int i = 0; i < receiver->connected_channel_count; ++i) {
        logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux send", "Sending on transport channel %d/%d",
                   i+1, receiver->connected_channel_count);

        channel = receiver->connected_channels[i];

        // send using the channel specific udp socket
        udp_send(mux->udp_socket_fds[i], data_to_send.bytes, data_to_send.length, channel.ip_address, channel.port);

        logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux send", "Sent data over channel %s:%d",
                   channel.ip_address, channel.port);
    }

    freeRastaByteArray(&data_to_send);

    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA Red send", "Data sent over all transport channels");
}

int redundancy_try_mux_retrieve(redundancy_mux * mux, unsigned long id, struct RastaPacket * out) {
    // get the channel by id
    rasta_redundancy_channel * target = redundancy_mux_get_channel(mux, id);

    if (target == NULL){
        logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux retrieve", "entity with id 0x%lX not connected, passing", id);
        return 0;
    }

    struct RastaByteArray * element;

    if (fifo_get_size(target->fifo_recv) == 0) {
        return 0;
    }


    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux retrieve", "Found element in queue");

    element = fifo_pop(target->fifo_recv);

    struct RastaPacket packet = bytesToRastaPacket(*element, &target->hashing_context);

    freeRastaByteArray(element);
    rfree(element);

    *out = packet;
    return 1;
}

void redundancy_mux_wait_for_notifications(redundancy_mux * mux){
    if (mux->notifications_running == 0){
        logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux wait", "all notification threads finished");
        return;
    }
    logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux wait", "waiting for %d notification thread(s) to finish", mux->notifications_running);
    while (mux->notifications_running > 0){
        // busy wait
        // to avoid to much CPU utilization, force context switch by sleeping for 0ns
        nanosleep((const struct timespec[]){{0, 0L}}, NULL);
    }
    logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux wait", "all notification threads finished");
}

void redundancy_mux_wait_for_entity(redundancy_mux * mux, unsigned long id){
    logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux wait", "waiting for entity with id=0x%lX", id);
    rasta_redundancy_channel * target = NULL;
    while (target == NULL){
        target = redundancy_mux_get_channel(mux, id);
        // to avoid to much CPU utilization, force context switch by sleeping for 0ns
        nanosleep((const struct timespec[]){{0, 0L}}, NULL);
    }
    logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux wait", "entity with id=0x%lX available", id);
}

void redundancy_mux_add_channel(redundancy_mux * mux, unsigned long id, struct RastaIPData * transport_channels){
    rasta_redundancy_channel channel = rasta_red_init(mux->logger, mux->config, mux->port_count, id);

    // add transport channels
    for (unsigned int i = 0; i < mux->port_count; ++i) {
        rasta_red_add_transport_channel(&channel, transport_channels[i].ip, (uint16_t)transport_channels[i].port);
    }

    //reallocate memory for new client
    mux->connected_channels = rrealloc(mux->connected_channels, (mux->channel_count + 1) * sizeof(rasta_redundancy_channel));

    mux->connected_channels[mux->channel_count] = channel;
    mux->channel_count++;

    logger_log(&mux->logger, LOG_LEVEL_INFO, "RaSTA RedMux add channel", "added new redundancy channel for ID=0x%X", id);
}

void redundancy_mux_remove_channel(redundancy_mux * mux, unsigned long channel_id){
    rasta_redundancy_channel * channel = redundancy_mux_get_channel(mux, channel_id);
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux remove channel", "removing channel with ID=0x%X", channel_id);

    if (channel == NULL){
        // no channel with given id
        return;
    }

    rasta_redundancy_channel * new_channels = rmalloc((mux->channel_count -1) * sizeof(rasta_redundancy_channel));

    int newIndex = 0;
    for (unsigned int i = 0; i < mux->channel_count; ++i) {
        rasta_redundancy_channel c = mux->connected_channels[i];

        if (c.associated_id == channel_id){
            logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux remove channel", "skipping channel with ID=0x%X", c.associated_id);
            // channel to remove, skip
            continue;
        }

        logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux remove channel", "copy channel with ID=0x%X", c.associated_id);
        // otherwise copy to new channel array
        new_channels[newIndex] = c;
        newIndex++;
    }

    rfree(mux->connected_channels);
    mux->connected_channels = new_channels;
    mux->channel_count --;
    logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux remove channel", "%d channels left", mux->channel_count);
}

/**
 * gets the amount of messages in the receive queue of the connected channel with index @p redundancy_channel_index
 * @param mux the multiplexer that is used
 * @param redundancy_channel_index the index of the redundancy channel inside the mux connected_channels array
 * @return amount of messages in the queue
 */
unsigned int get_queue_msg_count(redundancy_mux * mux, int redundancy_channel_index){
    if (redundancy_channel_index > (int)mux->channel_count -1){
        // channel does not exist anymore
        return 0;
    }

    rasta_redundancy_channel channel = mux->connected_channels[redundancy_channel_index];

    if (channel.fifo_recv == NULL){
        return 0;
    }
    unsigned int size = fifo_get_size(channel.fifo_recv);

    return size;
}

int redundancy_mux_try_retrieve_all(redundancy_mux * mux, struct RastaPacket* out) {
    for (size_t i = 0; i < mux->channel_count; i++) {
        if (get_queue_msg_count(mux, i) > 0){
            logger_log(&mux->logger, LOG_LEVEL_DEBUG, "RaSTA RedMux retrieve all", "channel with index %d has messages", i);
            redundancy_try_mux_retrieve(mux, mux->connected_channels[i].associated_id, out);
            return 1;
        }
    }
    return 0;
}
