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
#include <signal.h>
#include "rasta_red_multiplexer.h"
#define SERVER_HOST "127.0.0.1"

#define SERVER_ID 0xA
#define CLIENT_ID 0xB

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

    uint16_t listenPortsServer[2] = {8888, 8889};
    uint16_t listenPortsClient1[2] = {5555, 5556};

    struct logger_t logger = logger_init(LOG_LEVEL_DEBUG, LOGGER_TYPE_CONSOLE);
    redundancy_mux mux;

    if (strcmp(argv[1], "c") == 0){
        printf("Client 1 (ID=0x%X)\n",CLIENT_ID);

        // set client id
        info.general.rasta_id = CLIENT_ID;

        // add server address and port to client config
        struct RastaConfigRedundancyConnections cons;
        cons.count = 2;
        cons.data = rmalloc(2 * sizeof(struct RastaIPData));
        rmemcpy(cons.data[0].ip, SERVER_HOST, 15);
        rmemcpy(cons.data[1].ip, SERVER_HOST, 15);
        cons.data[0].port = 8888;
        cons.data[1].port = 8889;
        info.redundancy.connections = cons;

        printf("Initializing client...\n");
        mux = redundancy_mux_init(logger, listenPortsClient1, 2, info);
        redundancy_mux_set_config_id(&mux, SERVER_ID);

        printf("Init Client done\n");

        redundancy_mux_open(&mux);
        printf("Connection open\n");


        unsigned long current_seq = 0;
        while (1){
            struct RastaPacket hb = createHeartbeat(SERVER_ID, CLIENT_ID, current_seq, current_seq, current_seq, current_seq, &hashing_context);
            current_seq++;
            redundancy_mux_send(&mux, hb);
            printf("Sent HB %lu\n", current_seq);

            struct RastaPacket received = redundancy_mux_retrieve_all(&mux);

            printf("Received HB %lu\n", received.sequence_number);
        }

        printf("Data sent, exiting...\n");
        redundancy_mux_close(&mux);
        printf("Connection closed\n");
    } else{
        printf("Server mode (ID=0x%X)\n", SERVER_ID);

        // set server id
        info.general.rasta_id = SERVER_ID;
        struct RastaConfigRedundancyConnections cons;
        cons.count = 0;
        info.redundancy.connections = cons;

        mux = redundancy_mux_init(logger, listenPortsServer, 2, info);

        mux.notifications.on_new_connection = on_new_connection;

        printf("Init Server done\n");

        redundancy_mux_open(&mux);
        printf("Connection open\n");
        printf("Waiting for message from Client 1\n");

        while (1){
            struct RastaPacket received = redundancy_mux_retrieve(&mux, CLIENT_ID);
            printf("Received HB %lu\n", received.sequence_number);
            received.sender_id = SERVER_ID;
            received.receiver_id = CLIENT_ID;

            redundancy_mux_send(&mux, received);

            printf("Sent HB %lu back to client\n", received.sequence_number);
        }

        printf("Closing connection...\n");
        redundancy_mux_close(&mux);
        printf("Connection closed\n");
    }

    return 0;
}