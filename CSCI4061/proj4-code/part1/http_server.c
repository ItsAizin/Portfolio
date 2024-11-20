#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"

#define BUFSIZE 512
#define LISTEN_QUEUE_LEN 5

int keep_going = 1;

void handle_sigint(int signo) {
    keep_going = 0;
}

int main(int argc, char **argv) {
    // First command is directory to serve, second command is port
    if (argc != 3) {
        printf("Usage: %s <directory> <port>\n", argv[0]);
        return 1;
    }

    const char *serve_dir = argv[1];
    const char *port = argv[2];

    struct sigaction sac;
    sac.sa_handler = handle_sigint;
    if (sigfillset(&sac.sa_mask) == -1 || sigaction(SIGINT, &sac, NULL) == -1) {
        perror("sigfillset");
        return 1;
    }

    /*SIGACTION FLAGS FOR INTERRUPT*/
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *server;

    /*Converts address and port to request protocol*/
    int ret_val = getaddrinfo(NULL, port, &hints, &server);
    if (ret_val != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(ret_val));
        return 1;
    }

    /*Set up socket to allow sending and receiving bytes*/
    int sock_val = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (sock_val == -1) {
        perror("socket");
        freeaddrinfo(server);
        return 1;
    }

    /*Assigns a locak socket address to socket identified descriptor socket*/
    /*is the server and server length correct?*/
    int bind_val = bind(sock_val, server->ai_addr, server->ai_addrlen);
    if (bind_val == -1) {
        perror("bind");
        freeaddrinfo(server);
        close(sock_val);
        return 1;
    }

    /*Mark a connection-mode socket*/
    int listen_val = listen(sock_val, LISTEN_QUEUE_LEN);
    if (listen_val == -1) {
        perror("listen");
        freeaddrinfo(server);
        close(sock_val);
        return 1;
    }

    /*Loop through all connections in queue to extract the connection
    read the client to get the name of the resource, and write the resource
    to the socket*/
    while (keep_going) {
        /*Extract the first connection on the que of pending connections*/
        int accept_val = accept(sock_val, NULL, NULL);
            if (accept_val == -1) {
                perror("accept");
                freeaddrinfo(server);
                close(sock_val);
                return 1;
            }
            /*Converted requested source name to file path
            Get the directory and snprintf it with the server directory*/
            char resource_name[BUFSIZE];
            int request_val = read_http_request(accept_val, resource_name);
            if (request_val == -1) {
                perror("write_http_response");
                close(sock_val);
                return 1;
            }
            
            /*Use snprintf to copy the resource_name and given server directory
            to get the resource path to use for the write response*/
            char resource_path[BUFSIZE];

            snprintf(resource_path, BUFSIZE, "%s%s", serve_dir, resource_name);

            int response_val = write_http_response(accept_val, resource_path);
            if (response_val == -1) {
                perror("write_http_response");
                close(sock_val);
                return 1;   
            }
        close(accept_val);
    }
    close(sock_val);
    freeaddrinfo(server);
    return 0;
}
