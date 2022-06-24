#include <rasta_new.h>
#include <scils.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <rmemory.h>
#include <scils_telegram_factory.h>

#define CONFIG_PATH_S "../../../rasta_server_local.cfg"
#define CONFIG_PATH_C "../../../rasta_client1_local.cfg"

#define ID_S 0x61
#define ID_C 0x62

#define SCI_NAME_S "S"
#define SCI_NAME_C "C"

scils_t * scils;

void printHelpAndExit(void){
    printf("Invalid Arguments!\n use 's' to start in server mode and 'c' client mode.\n");
    exit(1);
}

void onReceive(struct rasta_notification_result *result){
    rastaApplicationMessage message = sr_get_received_data(result->handle, &result->connection);
    scils_on_rasta_receive(scils, message);
}

void onHandshakeComplete(struct rasta_notification_result *result) {
    if (result->connection.my_id == ID_C){

        printf("Sending show signal aspect command...\n");
        scils_signal_aspect * signal_aspect = scils_signal_aspect_defaults();
        signal_aspect->main = SCILS_MAIN_HP_0;

        sci_return_code code = scils_send_show_signal_aspect(scils, SCI_NAME_S, *signal_aspect);

        rfree(signal_aspect);
        if (code == SUCCESS){
            printf("Sent show signal aspect command to server\n");
        } else{
            printf("Something went wrong, error code 0x%02X was returned!\n", code);
        }
    }
}

void onShowSignalAspect(scils_t * ls, char * sender, scils_signal_aspect signal_aspect){
    printf("Received show signal aspect with MAIN = 0x%02X from %s\n", signal_aspect.main, sci_get_name_string(sender));

    printf("Sending back location status...\n");
    sci_return_code code = scils_send_signal_aspect_status(ls, sender, signal_aspect);
    if (code == SUCCESS){
        printf("Sent signal aspect status\n");
    } else{
        printf("Something went wrong, error code 0x%02X was returned!\n", code);
    }
}

void onSignalAspectStatus(scils_t * ls, char * sender, scils_signal_aspect signal_aspect){
    (void)ls;
    printf("Received location status from %s. LS showing main = 0x%02X.\n",sci_get_name_string(sender), signal_aspect.main);
}

int main(int argc, char *argv[]){

    if (argc != 2) printHelpAndExit();

    struct rasta_handle h;

    struct RastaIPData toServer[2];

    strcpy(toServer[0].ip, "127.0.0.1");
    strcpy(toServer[1].ip, "127.0.0.1");
    toServer[0].port = 8888;
    toServer[1].port = 8889;

    if (strcmp(argv[1], "s") == 0) {
        printf("->   S (ID = 0x%lX)\n", (unsigned long)ID_S);

        getchar();
        sr_init_handle(&h, CONFIG_PATH_S);
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeComplete;
        scils = scils_init(&h, SCI_NAME_S);
        scils->notifications.on_show_signal_aspect_received = onShowSignalAspect;
    }
    else if (strcmp(argv[1], "c") == 0) {
        printf("->   C (ID = 0x%lX)\n", (unsigned long)ID_C);

        sr_init_handle(&h, CONFIG_PATH_C);
        h.notifications.on_receive = onReceive;
        h.notifications.on_handshake_complete = onHandshakeComplete;
        printf("->   Press Enter to connect\n");
        getchar();
        sr_connect(&h,ID_S,toServer);

        scils = scils_init(&h, SCI_NAME_C);
        scils->notifications.on_signal_aspect_status_received = onSignalAspectStatus;

        scils_register_sci_name(scils, SCI_NAME_S, ID_S);
    }

    getchar();

    scils_cleanup(scils);
    sr_cleanup(&h);
}
