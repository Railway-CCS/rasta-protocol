/*
 * This example is a client implementation to test the native wrapper for Java.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rasta_new.h>
#include "rasta_new.h"
#include "rmemory.h"
#include <unistd.h>

#define CONFIG_PATH_C1 "../../../rasta_wrapper.cfg"

#define ID_R 0x61
#define ID_S1 0x62

void addRastaString(struct RastaMessageData * data, int pos, char * str) {
    int size =  strlen(str) + 1;

    struct RastaByteArray msg ;
    allocateRastaByteArray(&msg, size);
    rmemcpy(msg.bytes, str, size);

    data->data_array[pos] = msg;
}

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
            if (result->connection.my_id == ID_S1) {
                struct RastaMessageData messageData1;
                allocateRastaMessageData(&messageData1, 1);

                addRastaString(&messageData1, 0, "Message from Sender 1");

                //send data to server
                sr_send(result->handle,ID_R, messageData1);

                //freeRastaMessageData(&messageData1);
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

void onReceive(struct rasta_notification_result *result) {
    rastaApplicationMessage p;

    printf("\nReceived data from %lu\n", result->connection.remote_id);

    p = sr_get_received_data(result->handle,&result->connection);

    printf("\nPacket is from %lu\n", p.id);
    printf("\nMsg: %s\n", p.appMessage.bytes);
}

int main(int argc, char *argv[]){
    struct rasta_handle h;

    struct RastaIPData toServer[2];

    strcpy(toServer[0].ip, "127.0.0.1");
    strcpy(toServer[1].ip, "127.0.0.1");
    toServer[0].port = 8888;
    toServer[1].port = 8889;


    printf("->   S1 (ID = 0x%lX)\n", (unsigned long)ID_S1);

    sr_init_handle(&h, CONFIG_PATH_C1);
    h.notifications.on_connection_state_change = onConnectionStateChange;
    h.notifications.on_receive = onReceive;
    printf("->   Press Enter to connect\n");
    getchar();
    sr_connect(&h,ID_R,toServer);
    printf("->   Connection request sent to 0x%lX\n", (unsigned long)ID_R);

    getchar();
    sr_cleanup(&h);
}

