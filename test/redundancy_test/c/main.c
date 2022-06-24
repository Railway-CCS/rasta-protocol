/**
 * This is a simple forwarding example using only the redundancy layer.
 * An entity configured as server (ID 0xA) will listen on 2 ports for incoming messages from an entity with ID 0xB (this is client 1)
 * when a message is received, it will forward the message to client 2 (ID 0xC) and close the redundancy channels.
 * Client 1 just sends a PDU with Seq.# = 42 to the server and closes the redundancy channel.
 * Client 2 waits for messages from the server and once it receives a PDU, it will print the Seq.# and close the redundancy channel.
 *
 * To use this example first start the server with "./redundancy_test s",
 * in another terminal start client 2 with "./redundancy_test c2" and finally start client 1, again in another terminal,
 * with "./redundancy_test c1"
 */

#include <stdint.h>
#include <memory.h>
#include <stdio.h>
#include <rasta_red_multiplexer.h>
#include <rmemory.h>
#include <unistd.h>
#include <rastaredundancy_new.h>
#include "rasta_red_multiplexer.h"

#define SERVER_TC1_HOST "10.0.0.100"
#define SERVER_TC2_HOST "10.0.0.101"

#define SERVER_ID 0xA
#define CLIENT_1_ID 0xB
#define CLIENT_2_ID 0xC

void on_new_connection(redundancy_mux * mux, unsigned long id){
    printf("New entity with ID=0x%lX\n", id);
}

int main(int argc, char *argv[]){
    rasta_hashing_context_t hashing_context;
    hashing_context.algorithm = RASTA_ALGO_MD4;
    hashing_context.hash_length = RASTA_CHECKSUM_8B;
    rasta_md4_set_key(&hashing_context, 0, 0, 0, 0);

    struct RastaConfigInfo info;
    struct RastaConfigInfoRedundancy configInfoRedundancy;
    configInfoRedundancy.t_seq = 100;
    configInfoRedundancy.n_diagnose = 10;
    configInfoRedundancy.crc_type = crc_init_opt_a();
    configInfoRedundancy.n_deferqueue_size = 2;
    info.redundancy = configInfoRedundancy;

    struct RastaIPData * listenPortsServer = rmalloc(2 * sizeof(struct RastaIPData));
    rmemcpy(listenPortsServer[0].ip, "10.0.0.100", 16);
    listenPortsServer[0].port = 8888;
    rmemcpy(listenPortsServer[1].ip, "10.0.0.101", 16);
    listenPortsServer[1].port = 8889;

    struct RastaIPData * listenPortsClient1 = rmalloc(2 * sizeof(struct RastaIPData));
    rmemcpy(listenPortsClient1[0].ip, "10.0.0.200", 16);
    listenPortsClient1[0].port = 5555;
    rmemcpy(listenPortsClient1[1].ip, "10.0.0.201", 16);
    listenPortsClient1[1].port = 5556;

    struct RastaIPData *  listenPortsClient2 = rmalloc(2 * sizeof(struct RastaIPData));
    rmemcpy(listenPortsClient2[0].ip, "10.0.0.1", 16);
    listenPortsClient2[0].port = 5557;
    rmemcpy(listenPortsClient2[1].ip, "10.0.0.2", 16);
    listenPortsClient2[1].port = 5558;


    struct RastaIPData * serverConnection = rmalloc(2 * sizeof(struct RastaIPData));
    rmemcpy(serverConnection[0].ip, SERVER_TC1_HOST, 16);
    rmemcpy(serverConnection[1].ip, SERVER_TC2_HOST, 16);
    serverConnection[0].port = 8888;
    serverConnection[1].port = 8889;

    struct logger_t logger = logger_init(LOG_LEVEL_NONE, LOGGER_TYPE_CONSOLE);
    redundancy_mux mux;

    if (strcmp(argv[1], "c1") == 0){
        printf("Client 1 (ID=0x%X)\n",CLIENT_1_ID);

        // set client id
        info.general.rasta_id = CLIENT_1_ID;

        // add server address and port to client config
        struct RastaConfigRedundancyConnections cons;
        cons.count = 2;
        cons.data = listenPortsClient1;
        info.redundancy.connections = cons;

        printf("Initializing client 1...\n");
        mux = redundancy_mux_init_(logger, info);
        redundancy_mux_add_channel(&mux, SERVER_ID, serverConnection);

        printf("Init Client 1 done\n");

        redundancy_mux_open(&mux);
        printf("Connection open\n");

        printf("Sending message to server\n");
        struct RastaPacket data = createConnectionRequest(SERVER_ID, CLIENT_1_ID, 42, 42, 42, "0303", &hashing_context);

        redundancy_mux_send(&mux, data);
        printf("Data sent, exiting...\n");
        redundancy_mux_close(&mux);
        printf("Connection closed\n");
    } else if (strcmp(argv[1], "c2") == 0){
        printf("Client 2 (ID=0x%X)\n", CLIENT_2_ID);

        // set client id
        info.general.rasta_id = CLIENT_2_ID;

        // add server address and port to client config
        struct RastaConfigRedundancyConnections cons;
        cons.count = 2;
        cons.data = listenPortsClient2;
        info.redundancy.connections = cons;

        printf("Initializing client 2...\n");
        mux = redundancy_mux_init_(logger, info);

        //redundancy_mux_set_config_id(&mux, SERVER_ID);
        redundancy_mux_add_channel(&mux, SERVER_ID, serverConnection);

        printf("Init Client 2 done\n");

        redundancy_mux_open(&mux);
        printf("Connection open\n");
        printf("Sending a hello message to server\n");

        // a "hello message" is need in this case as the server doesn't now Client 2's transport channels when started
        // by receiving data from Client 2 the server will detect the transport channels and it's possible to send to
        // Client 2
        struct RastaPacket hello = createConnectionRequest(SERVER_ID, CLIENT_2_ID, 0, 0, 0, "0303", &hashing_context);
        redundancy_mux_send(&mux, hello);

        printf("Done\n");
        printf("Waiting for message from server...\n");

        struct RastaPacket received = redundancy_mux_retrieve_all(&mux);

        printf("Received PDU with Seq.# = %lu from 0x%lX\n", received.sequence_number, received.sender_id);
        printf("Closing connection...\n");

        redundancy_mux_close(&mux);

        printf("Connection closed\n");
    }
    else{
        printf("Server mode (ID=0x%X)\n", SERVER_ID);

        // set server id
        info.general.rasta_id = SERVER_ID;
        struct RastaConfigRedundancyConnections cons;
        cons.count = 2;
        cons.data = listenPortsServer;
        info.redundancy.connections = cons;

        mux = redundancy_mux_init_(logger, info);

        mux.notifications.on_new_connection = on_new_connection;

        printf("Init Server done\n");

        redundancy_mux_open(&mux);
        printf("Connection open\n");
        printf("Waiting for message from Client 1\n");

        redundancy_mux_wait_for_entity(&mux, CLIENT_1_ID);
        struct RastaPacket received = redundancy_mux_retrieve(&mux, CLIENT_1_ID);

        printf("Received PDU from Client 1 with Seq.# = %lu from 0x%lX\n", received.sequence_number, received.sender_id);
        printf("Forwarding received PDU to Client 2\n");

        // update source and destination IDs on received packet
        received.sender_id = SERVER_ID;
        received.receiver_id = CLIENT_2_ID;

        redundancy_mux_send(&mux, received);
        printf("Sent PDU to Client 2\n");
        printf("Closing connection...\n");

        redundancy_mux_close(&mux);
        printf("Connection closed\n");
    }

    rfree(listenPortsClient1);
    rfree(listenPortsClient2);
    rfree(listenPortsServer);
    rfree(serverConnection);

    return 0;
}
