#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "connection_queue.h"
#include "http.h"

#define BUFSIZE 512
#define LISTEN_QUEUE_LEN 5
#define N_THREADS 5

int keep_going = 1;
const char *serve_dir;

void handle_sigint(int signo) {
    keep_going = 0;
}

 void *start_routine(void *arg) {
 while(keep_going) {
            /*Dequeue to get the file descriptor to use for the read and write functions*/
            int dequeue_val = connection_dequeue(arg);
            if(dequeue_val == -1) {
                return NULL;
            }

            /*Converted requested source name to file path
            Get the directory and snprintf it with the server directory*/
            char resource_name[BUFSIZE];
            int request_val = read_http_request(dequeue_val, resource_name);
            if (request_val == -1) {
                perror("Read_request");
                return NULL;
            }
            
            /*Use snprintf to copy the resource_name and given server directory
            to get the resource path to use for the write response*/
            char resource_path[BUFSIZE];

            snprintf(resource_path, BUFSIZE, "%s%s", serve_dir, resource_name);

            int response_val = write_http_response(dequeue_val, resource_path);
            if (response_val == -1) {
                perror("Write_request");
                return NULL;   
            }
            close(dequeue_val);
        } 
        return NULL;   
}

int main(int argc, char **argv) {
    connection_queue_t connection;

    // First command is directory to serve, second command is port
    if (argc != 3) {
        printf("Usage: %s <directory> <port>\n", argv[0]);
        return 1;
    }

    serve_dir = argv[1];
    const char *port = argv[2];

    /*Signal handling*/
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

    sigset_t odd_sig;
    sigset_t new_sig;

    /*Fill the new_sig and error check if it works*/
    if (sigfillset(&new_sig) == -1) {
        perror("sigfillset");
        return 1;
    }

    /*Block all the signals before calling the threads*/
    int promask_val = sigprocmask(SIG_BLOCK, &new_sig, &odd_sig);
    if (promask_val == -1) {
        perror("sigpromask");
        freeaddrinfo(server);
        close(sock_val);       
        return 1;
    }

    /*Initalize the connection queue struct*/
    int init_val = connection_queue_init(&connection);
    if (init_val == -1) {
        perror("connection_queue_init");
        connection_queue_free(&connection);
        freeaddrinfo(server);
        close(sock_val);
        return 1;
    }

    /*Create fixed number of worker threads*/
    pthread_t thread[N_THREADS];
    for (int i = 0; i < N_THREADS; i++) {
        int create_val = pthread_create(thread + i, NULL, start_routine, &connection);
        if (create_val == -1) {
            perror("pthread_create");
            freeaddrinfo(server);
            close(sock_val);
            return 1;
        }
    }
    
    /*Restore the old signal*/
    promask_val = sigprocmask(SIG_SETMASK, &odd_sig, NULL);
    if (promask_val == -1) {
        perror("sigpromask");
        freeaddrinfo(server);
       close(sock_val);       
        return 1;
    }

    /*Loop through all threads in queue to call the accept function and place the 
    file descriptor into the queue if not full*/
    while (keep_going) {
        int accept_val = accept(sock_val, NULL, NULL);
            if (accept_val == -1) {
                if (errno == EINTR) {
                    freeaddrinfo(server);
                    close(sock_val);
                    return 1;
                }
            }

        int enqueue_val = connection_enqueue(&connection, accept_val);
        if (enqueue_val == -1) { 
            perror("connection_enqueue");
            connection_queue_shutdown(&connection);
            return 1;
        }
    }

    /*Cleanup*/
    connection_queue_shutdown(&connection);
    for (int i = 0; i < N_THREADS; i++) {
        int join_val = pthread_join(thread[i], NULL);
        if (join_val == -1) {
            perror("pthread_join");
            return 1;
        }
    }
    close(sock_val);
    connection_queue_free(&connection);
    freeaddrinfo(server);
    return 0;
}
