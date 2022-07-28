//
// Created by tobia on 24.02.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rasta_new.h>
#include <rmemory.h>

#define CONFIG_PATH_S "../../../rasta_server_local.cfg"
#define CONFIG_PATH_C1 "../../../rasta_client1_local.cfg"
#define CONFIG_PATH_C2 "../../../rasta_client2_local.cfg"

#define ID_R 0x61
#define ID_S1 0x62
#define ID_S2 0x63

void printHelpAndExit(void){
    printf("Invalid Arguments!\n use 'r' to start in receiver mode and 's1' or 's2' to start in sender mode.\n");
    exit(1);
}

void addRastaString(struct RastaMessageData * data, int pos, char * str) {
    size_t size = strlen(str) + 1;

    struct RastaByteArray msg ;
    allocateRastaByteArray(&msg, size);
    rmemcpy(msg.bytes, str, size);

    data->data_array[pos] = msg;
}

int client1 = 1;
int client2 = 1;

void onConnectionStateChange(struct rasta_notification_result *result) {
    printf("\n Connectionstate change (remote: %lu)", result->connection.remote_id);

    switch (result->connection.current_state) {
        case RASTA_CONNECTION_CLOSED:
            printf("\nCONNECTION_CLOSED \n\n");
            break;
        case RASTA_CONNECTION_START:
            printf("\nCONNECTION_START \n\n");
            break;
        case RASTA_CONNECTION_DOWN:
            printf("\nCONNECTION_DOWN \n\n");
            break;
        case RASTA_CONNECTION_UP:
            printf("\nCONNECTION_UP \n\n");
            //send data to server
            if (result->connection.my_id == ID_S1) { //Client 1
                struct RastaMessageData messageData1;
                allocateRastaMessageData(&messageData1, 1);

                // messageData1.data_array[0] = msg1;
                // messageData1.data_array[1] = msg2;
                addRastaString(&messageData1, 0, "Message from Sender 1");

                //send data to server
                sr_send(result->handle,ID_R, messageData1);

                //freeRastaMessageData(&messageData1);
            } else if (result->connection.my_id == ID_S2) { //Client 2
                struct RastaMessageData messageData1;
                allocateRastaMessageData(&messageData1, 1);

                // messageData1.data_array[0] = msg1;
                // messageData1.data_array[1] = msg2;
                addRastaString(&messageData1, 0, "Message from Sender 2");

                //send data to server
                sr_send(result->handle,ID_R, messageData1);

                //freeRastaMessageData(&messageData1);
            }
            else if (result->connection.my_id == ID_R) {
                if (result->connection.remote_id == ID_S1) client1 = 0;
                else if (result->connection.remote_id == ID_S2) client2 = 0;
            }


            break;
        case RASTA_CONNECTION_RETRREQ:
            printf("\nCONNECTION_RETRREQ \n\n");
            break;
        case RASTA_CONNECTION_RETRRUN:
            printf("\nCONNECTION_RETRRUN \n\n");
            break;
    }

}

void onHandshakeCompleted(struct rasta_notification_result *result){
    printf("Handshake complete, state is now UP (with ID 0x%lX)\n", result->connection.remote_id);
}

void onTimeout(struct rasta_notification_result *result){
    printf("Entity 0x%lX had a heartbeat timeout!\n", result->connection.remote_id);
}

void onReceive(struct rasta_notification_result *result) {
    rastaApplicationMessage p;

    switch (result->connection.my_id) {
        case ID_R:
            //Server
            printf("\nReceived data from Client %lu", result->connection.remote_id);

            p = sr_get_received_data(result->handle,&result->connection);

            printf("\nPacket is from %lu", p.id);
            printf("\nMsg: %s", p.appMessage.bytes);

            printf("\n\n\n");

            printf("\nSend it to other client \n");

            unsigned long target;
            if (p.id == ID_S1) {
                if (client2) {
                    // other client not connected
                    return;
                }
                target = ID_S2;
            }
            else {
                if (client1) {
                    // other client not connected
                    return;
                }
                target = ID_S1;
            }

            printf("Client message from %lu is now send to %lu\n", p.id,target);

            struct RastaMessageData messageData1;
            allocateRastaMessageData(&messageData1, 1);

            addRastaString(&messageData1,0,(char*)p.appMessage.bytes);



            sr_send(result->handle,target,messageData1);

            printf("Message forwarded\n");

            sleep(1);

            printf("Disconnect to client %lu \n\n\n", target);

            sr_disconnect(result->handle,target);

            break;
        case ID_S1: case ID_S2:
            printf("\nReceived data from Server %lu", result->connection.remote_id);

            p = sr_get_received_data(result->handle,&result->connection);

            printf("\nPacket is from %lu", p.id);
            printf("\nMsg: %s", p.appMessage.bytes);

            printf("\n\n\n");
            break;
    }
}

struct connect_event_data {
    struct rasta_handle * h;
    struct RastaIPData * ip_data_arr;
    fd_event * connect_event;
    fd_event * schwarzenegger;
};

char connect_on_stdin(void * carry_data) {
    printf("try to connect\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("->   Connection request sent to 0x%lX\n", (unsigned long)ID_R);
    struct connect_event_data * data = carry_data;
    sr_connect(data->h, ID_R, data->ip_data_arr);
    enable_fd_event(data->schwarzenegger);
    disable_fd_event(data->connect_event);
    return 0;
}

char terminator(void * h) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    sr_cleanup(h);
    return 1;
}

int main(int argc, char *argv[]){

    if (argc != 2) printHelpAndExit();

    struct rasta_handle h;

    struct RastaIPData toServer[2];

    strcpy(toServer[0].ip, "127.0.0.1");
    strcpy(toServer[1].ip, "127.0.0.1");
    toServer[0].port = 8888;
    toServer[1].port = 8889;

    fd_event fd_events[2];
    struct connect_event_data connect_on_stdin_event_data = {
        .h = &h,
        .ip_data_arr = toServer,
        .connect_event = &fd_events[1],
        .schwarzenegger = &fd_events[0]
    };

    fd_events[0].meta_information.callback = terminator;
    fd_events[0].meta_information.carry_data = &h;
    fd_events[0].fd = STDIN_FILENO;
    fd_events[0].meta_information.enabled = 0;

    fd_events[1].meta_information.callback = connect_on_stdin;
    fd_events[1].meta_information.carry_data = &connect_on_stdin_event_data;
    fd_events[1].fd = STDIN_FILENO;
    fd_events[1].meta_information.enabled = 0;

    if (strcmp(argv[1], "r") == 0) {
        printf("->   R (ID = 0x%lX)\n", (unsigned long)ID_R);

        getchar();
        sr_init_handle(&h, CONFIG_PATH_S);
        h.notifications.on_connection_state_change = onConnectionStateChange;
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeCompleted;
        h.notifications.on_heartbeat_timeout = onTimeout;
        enable_fd_event(&fd_events[0]);
        sr_begin(&h, fd_events, 1);
    }
    else if (strcmp(argv[1], "s1") == 0) {
        printf("->   S1 (ID = 0x%lX)\n", (unsigned long)ID_S1);

        sr_init_handle(&h, CONFIG_PATH_C1);
        h.notifications.on_connection_state_change = onConnectionStateChange;
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeCompleted;
        printf("->   Press Enter to connect\n");
        disable_fd_event(&fd_events[0]);
        enable_fd_event(&fd_events[1]);
        sr_begin(&h, fd_events, 2);
    }
    else if (strcmp(argv[1], "s2") == 0) {
        printf("->   S2 (ID = 0x%lX)\n", (unsigned long)ID_S2);

        sr_init_handle(&h, CONFIG_PATH_C2);
        h.notifications.on_connection_state_change = onConnectionStateChange;
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeCompleted;
        printf("->   Press Enter to connect\n");
        disable_fd_event(&fd_events[0]);
        enable_fd_event(&fd_events[1]);
        sr_begin(&h, fd_events, 2);
    }
}

