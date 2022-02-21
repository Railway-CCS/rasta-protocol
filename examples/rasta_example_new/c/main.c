//
// Created by tobia on 24.02.2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "rasta_new.h"
#include "rmemory.h"

#define CONFIG_PATH_S "../../../rasta_server.cfg"
#define CONFIG_PATH_C1 "../../../rasta_client1.cfg"
#define CONFIG_PATH_C2 "../../../rasta_client2.cfg"

#define ID_R 0x61
#define ID_S1 0x62
#define ID_S2 0x63

void printHelpAndExit(void){
    printf("Invalid Arguments!\n use 'r' to start in receiver mode and 's1' or 's2' to start in sender mode.\n");
    exit(1);
}

void addRastaString(struct RastaMessageData * data, int pos, char * str) {
    int size =  strlen(str) + 1;

    struct RastaByteArray msg ;
    allocateRastaByteArray(&msg, size);
    rmemcpy(msg.bytes, str, size);

    data->data_array[pos] = msg;
}

int client1 = 1;
int client2 = 1;

void onConnectionStateChange(struct rasta_notification_result *result) {
    printf("\n Connectionstate change (remote: %u)", result->connection.remote_id);

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
        default:
            break;
    }

}

void onHandshakeCompleted(struct rasta_notification_result *result){
    printf("Handshake complete, state is now UP (with ID 0x%X)\n", result->connection.remote_id);
}

void onTimeout(struct rasta_notification_result *result){
    printf("Entity 0x%X had a heartbeat timeout!\n", result->connection.remote_id);
}

void onReceive(struct rasta_notification_result *result) {
    rastaApplicationMessage p;

    switch (result->connection.my_id) {
        case ID_R:
            //Server
            printf("\nReceived data from Client %u", result->connection.remote_id);

            p = sr_get_received_data(result->handle,&result->connection);

            printf("\nPacket is from %lu", p.id);
            printf("\nMsg: %s", p.appMessage.bytes);

            printf("\n\n\n");

            printf("\nSend it to other client \n");

            unsigned long target;
            if (p.id == ID_S1) {
                while (client2) {sleep(1);}
                target = ID_S2;
            }
            else {
                while (client1) {sleep(1);}
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
            printf("\nReceived data from Server %u", result->connection.remote_id);

            p = sr_get_received_data(result->handle,&result->connection);

            printf("\nPacket is from %lu", p.id);
            printf("\nMsg: %s", p.appMessage.bytes);

            printf("\n\n\n");
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]){

    if (argc != 2) printHelpAndExit();

    struct rasta_handle h;

    struct RastaIPData toServer[2];

#ifdef EXAMPLE_IP_OVERRIDE
    strcpy(toServer[0].ip, getenv("SERVER_CH1"));
    strcpy(toServer[1].ip, getenv("SERVER_CH2"));
#else
    strcpy(toServer[0].ip, "10.0.0.100");
    strcpy(toServer[1].ip, "10.0.0.101");
#endif
    toServer[0].port = 8888;
    toServer[1].port = 8889;

    printf("Server at %s:%d and %s:%d\n", toServer[0].ip, toServer[0].port, toServer[1].ip, toServer[1].port);


    if (strcmp(argv[1], "r") == 0) {
        printf("->   R (ID = 0x%lX)\n", (unsigned long)ID_R);

        getchar();
        sr_init_handle(&h, CONFIG_PATH_S);
        h.notifications.on_connection_state_change = onConnectionStateChange;
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeCompleted;
        h.notifications.on_heartbeat_timeout = onTimeout;

    }
    else if (strcmp(argv[1], "s1") == 0) {
        printf("->   S1 (ID = 0x%lX)\n", (unsigned long)ID_S1);

        sr_init_handle(&h, CONFIG_PATH_C1);
        h.notifications.on_connection_state_change = onConnectionStateChange;
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeCompleted;
        printf("->   Press Enter to connect\n");
        getchar();
        sr_connect(&h,ID_R,toServer);
        printf("->   Connection request sent to 0x%lX\n", (unsigned long)ID_R);

    }
    else if (strcmp(argv[1], "s2") == 0) {
        printf("->   S2 (ID = 0x%lX)\n", (unsigned long)ID_S2);

        sr_init_handle(&h, CONFIG_PATH_C2);
        h.notifications.on_connection_state_change = onConnectionStateChange;
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeCompleted;
        printf("->   Press Enter to connect\n");
        getchar();
        sr_connect(&h,ID_R,toServer);
        printf("->   Connection request sent to 0x%lX\n", (unsigned long)ID_R);

    }





    getchar();
    sr_cleanup(&h);
}

