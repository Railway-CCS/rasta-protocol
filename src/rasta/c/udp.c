#include "udp.h"
#include <stdio.h>
#include <string.h> //memset
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "rmemory.h"

/**
 * clears the erros of the socket and prepares for closing
 * @param fd the file descriptor
 * @return the socket state
 */
int getSO_ERROR(int fd) {
    int err = 1;
    socklen_t len = sizeof err;
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len))
        exit(1);
    if (err)
        errno = err;              // set errno to the socket SO_ERROR
    return err;
}


void udp_bind(int file_descriptor, uint16_t port) {
    struct sockaddr_in local;

    // set struct to 0s
    rmemset((char *) &local, 0, sizeof(local));

    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind socket to port
    if( bind(file_descriptor , (struct sockaddr*)&local, sizeof(local) ) == -1)
    {
        // bind failed
        perror("could not bind the socket to port " + port);
        exit(1);
    }
}

void udp_bind_device(int file_descriptor, uint16_t port, char * ip) {
    struct sockaddr_in local;

    // set struct to 0s
    rmemset((char *) &local, 0, sizeof(local));

    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(ip);

    // bind socket to port
    if( bind(file_descriptor , (struct sockaddr*)&local, sizeof(struct sockaddr_in) ) == -1)
    {
        // bind failed
        perror("could not bind the socket to port");
        exit(1);
    }
}

void udp_close(int file_descriptor) {
    //close(file_descriptor);
    if (file_descriptor >= 0) {
        getSO_ERROR(file_descriptor); // first clear any errors, which can cause close to fail
        if (shutdown(file_descriptor, SHUT_RDWR) < 0) // secondly, terminate the 'reliable' delivery
            if (errno != ENOTCONN && errno != EINVAL){ // SGI causes EINVAL
                perror("shutdown");
                exit(1);
            }
        if (close(file_descriptor) < 0) // finally call close()
        {
            perror("close");
            exit(1);
        }
    }
}

size_t udp_receive(int file_descriptor, unsigned char *received_message, size_t max_buffer_len, struct sockaddr_in *sender) {
    ssize_t recv_len;
    struct sockaddr_in empty_sockaddr_in;
    socklen_t sender_len = sizeof(empty_sockaddr_in);

    // wait for incoming data
    if ((recv_len = recvfrom(file_descriptor, received_message, max_buffer_len, 0, (struct sockaddr *) sender, &sender_len)) == -1)
    {
        perror("an error occured while trying to receive data");
        exit(1);
    }

    return (size_t) recv_len;
}

void udp_send(int file_descriptor, unsigned char *message, size_t message_len, char *host, uint16_t port) {
    struct sockaddr_in receiver;

    rmemset((char *) &receiver, 0, sizeof(receiver));
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(port);

    // convert host string to usable format
    if (inet_aton(host , &receiver.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    // send the message using the other send function
    udp_send_sockaddr(file_descriptor, message, message_len, receiver);
}

void udp_send_sockaddr(int file_descriptor, unsigned char *message, size_t message_len, struct sockaddr_in receiver) {
    if (sendto(file_descriptor, message, message_len, 0, (struct sockaddr*) &receiver, sizeof(receiver)) == -1)
    {
        perror("failed to send data");
        exit(1);
    }
}

int udp_init() {
    // the file descriptor of the socket
    int file_desc;

    // create a udp socket
    if ((file_desc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        // creation failed, exit
        perror("The udp socket could not be initialized");
        exit(1);
    }

    return file_desc;
}

void sockaddr_to_host(struct sockaddr_in sockaddr, char* host){
    inet_ntop(AF_INET, &(sockaddr.sin_addr), host, IPV4_STR_LEN);
}




