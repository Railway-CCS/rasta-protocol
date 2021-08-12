#include <stdio.h>
#include <stdlib.h>
#include <rasta_new.h>
#include <scip.h>
#include <scils.h>
#include <memory.h>
#include <scils_telegram_factory.h>
#include <rmemory.h>

#define CONFIG_PATH "../../../rasta_wrapper.cfg"
#define SCI_NAME "BTP"
#define SERVER_SCI_NAME "ESTW"
#define SERVER_ID 0x61

#define MODE_SCIP 0x01
#define MODE_SCILS 0x02


/**
 * A simple SCI-P / SCI-LS client to test the communication with the Java implementation. Pass 'scip' or 'scils' as
 * command line arguments to run the corresponding test client.
 * The client will connect to the server on localhost and as soon as the connection is up sent a status telegram in order
 * to let the server know its SCI name. After that it is waiting for change location / show signal aspect commands and
 * reply to them with the corresponding status message.
 *
 * Note that the server needs to have RaSTA ID = 0x61 and SCI name = ESTW in order for the example to work.
 */

scip_t * scip;
scils_t * scils;
int mode;

void printHelpAndExit(void){
    printf("Invalid Arguments!\n use 'scip' to start SCI-P client and 'scils' to start SCI-LS client.\n");
    exit(1);
}

void onReceive(struct rasta_notification_result *result){
    rastaApplicationMessage message = sr_get_received_data(result->handle, &result->connection);

    if (mode == MODE_SCILS){
        scils_on_rasta_receive(scils, message);
    } else {
        scip_on_rasta_receive(scip, message);
    }
}

void onConnectionStateChange(struct rasta_notification_result *result){
    if (result->connection.current_state == RASTA_CONNECTION_UP){
        printf("Connection established! Sending message to let the server know my SCI name\n");

        sci_return_code code;
        if (mode == MODE_SCILS){
            scils_signal_aspect * default_sa = scils_signal_aspect_defaults();
            code = scils_send_signal_aspect_status(scils, SERVER_SCI_NAME, *default_sa);
        } else {
            code = scip_send_location_status(scip, SERVER_SCI_NAME, POINT_LOCATION_RIGHT);
        }

        printf("Done (0x%02X)\n", code);
    }
}

void onSignalAspectChange(scils_t * ls, char * sender, scils_signal_aspect signal_aspect){
    printf("Received show signal aspect command from '%s' with MAIN=0x%02X\n", sci_get_name_string(sender), signal_aspect.main);
    printf("Sending back signal aspect status\n");

    scils_send_signal_aspect_status(ls, sender, signal_aspect);
    printf("Done\n");
}

void onLocationChange(scip_t * p, char * sender, scip_point_target_location location){
    printf("Received change location command from '%s' with new location=0x%02X\n", sci_get_name_string(sender), location);
    printf("Sending back location status\n");

    scip_send_location_status(p, sender, (scip_point_location)location);
    printf("Done\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) printHelpAndExit();

    struct rasta_handle h;

    if (strcmp(argv[1], "scip") == 0){
        mode = MODE_SCIP;
        scip = scip_init(&h, SCI_NAME);
        scip->notifications.on_change_location_received = onLocationChange;

        scip_register_sci_name(scip, SERVER_SCI_NAME, SERVER_ID);
    } else if (strcmp(argv[1], "scils") == 0){
        mode = MODE_SCILS;
        scils = scils_init(&h, SCI_NAME);
        scils->notifications.on_show_signal_aspect_received = onSignalAspectChange;

        scils_register_sci_name(scils, SERVER_SCI_NAME, SERVER_ID);
    } else {
        printHelpAndExit();
    }


    struct RastaIPData toServer[2];

    strcpy(toServer[0].ip, "127.0.0.1");
    strcpy(toServer[1].ip, "127.0.0.1");
    toServer[0].port = 8888;
    toServer[1].port = 8889;

    sr_init_handle(&h, CONFIG_PATH);
    h.notifications.on_receive = onReceive;
    h.notifications.on_connection_state_change = onConnectionStateChange;

    printf("Press Enter to connect to server...\n");
    getchar();

    sr_connect(&h, SERVER_ID, toServer);

    printf("Press enter to disconnect.\n"),
    getchar();

    sr_disconnect(&h, SERVER_ID);
    printf("Sent disconnection request. Press Enter again to cleanup.\n");

    getchar();

    if (mode == MODE_SCILS){
        scils_cleanup(scils);
    } else {
        scip_cleanup(scip);
    }
    sr_cleanup(&h);
}