/**
 * This example demonstrates the use of the udp module, by implementing a simple echo server
 * and the corresponding client.
 * The server is bound to an specified port and starts listening. When data is received it is sent back to the sender
 * and the application terminates.
 * The client sends a string message to the specified host and port and waits for a response. When a response is
 * received the client terminates.
 *
 * To use this example, first start an instance in server mode and after that another instance in client mode
 * to send a message and receive the echo.
 * The program is used with command line parameter 's' to start as the server
 * and parameters 'c <message>' to start as a client where <message> is the string which is sent to the server
 */

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "udp.h"

// the server host
#define HOST "127.0.0.1"

// thr port the server should listen on
#define PORT 8888

// the maximum length of the receive buffer in byte
#define BUF_LEN 1024

void printHelpAndExit(void){
    printf("Invalid Arguments!\n use 's' to start in server mode and 'c <message>' to start in client mode.\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        printHelpAndExit();
    }

    // init the udp socket and store the file_descriptor to address the socket later on
    int fd = udp_init();

    // buffer to store the received data
    //char response[BUF_LEN];
    unsigned char* response = malloc(BUF_LEN * sizeof(char));


    // to store information from the sender, like address and port
    struct sockaddr_in sender;

    if (strcmp(argv[1], "c") == 0){
        if (argc != 3){
            printHelpAndExit();
        }

        // client mode
        printf("Started in client mode.\n");

        // the message that will be sent
        unsigned char* message = (unsigned char*)argv[2];

        // send data to server
        printf("Sending message '%s' to server on %s:%d...", message, HOST, PORT);
        udp_send(fd, message, strlen(argv[2]), HOST, PORT);
        printf("DONE\n");

        // wait for an echo
        printf("waiting for response...\n");

        // blocks until data is received
        size_t resp_len = udp_receive(fd, response, BUF_LEN, &sender);

        printf("Received message '%s' with length %lu from %s:%d\n", response, resp_len, inet_ntoa(sender.sin_addr), htons(sender.sin_port));

        //free(response);
    } else if (strcmp(argv[1], "s") == 0) {
        // server mode
        printf("Started in server mode.\n Binding to port %d...", PORT);

        // bind the udp socket to the specified port
        udp_bind(fd, PORT);
        printf("DONE\n");

        memset((char *) &sender, 0, sizeof(sender));

        // blocks until data is received
        size_t resp_len = udp_receive(fd, response, BUF_LEN, &sender);

        printf("Received message '%s' with length %lu from %s:%d\n", response, resp_len, inet_ntoa(sender.sin_addr), htons(sender.sin_port));
        printf("Echoing back to sender...");

        // send the received data back to the sender
        udp_send_sockaddr(fd, response, resp_len, sender);
        printf("DONE\n");

    } else {
        printHelpAndExit();
    }

    // close the udp socket
    udp_close(fd);

    free(response);
    return 0;
}