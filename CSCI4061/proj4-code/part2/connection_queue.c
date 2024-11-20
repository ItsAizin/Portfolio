#include <stdio.h>
#include <string.h>
#include "connection_queue.h"

/*What does the shutdown do
what inialized value should client_fds should do*/
int connection_queue_init(connection_queue_t *queue) {
    queue->length = 0;
    queue->read_idx = 0;
    queue->write_idx = 1;
    queue->shutdown = 1;

    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->queue_full, NULL);
    pthread_cond_init(&queue->queue_empty, NULL);

    return 0;
}

int connection_enqueue(connection_queue_t *queue, int connection_fd) {
    /*Obtain the lock and when the queue is at the maximum length, then wait
    until their is an availability in the queue.*/
    pthread_mutex_lock(&queue->lock);
    while (queue->length == CAPACITY) {
        pthread_cond_wait(&queue->queue_full, &queue->lock);
    }

    /*Will iterate through the client_fds and add the connection_fd
    if the current index in client_fds is NULL*/
    queue->client_fds[queue->write_idx] = connection_fd;
    queue->length++;
    queue->write_idx++;

    if(queue->write_idx == CAPACITY) {
        queue->write_idx = 0;
    }

    pthread_cond_signal(&queue->queue_empty);
    pthread_mutex_unlock(&queue->lock);
    return 0;
}

int connection_dequeue(connection_queue_t *queue) {
    /*Obtain the lock and when the queue is at the minimum length length, then wait
    until their is an availability in the queue.*/
    pthread_mutex_lock(&queue->lock);
    while (queue->length == 0) {
        pthread_cond_wait(&queue->queue_empty, &queue->lock);
    }

    /*Will iterate through the client_fds and add the connection_fd
    if the current index in client_fds is NULL*/
    int fd_ret = queue->client_fds[queue->read_idx];
    queue->length--;
    queue->read_idx++;

    if (queue->read_idx == CAPACITY) {
        queue->read_idx = 0;
    }

    pthread_cond_signal(&queue->queue_full);
    pthread_mutex_unlock(&queue->lock);

    return fd_ret;
}

int connection_queue_shutdown(connection_queue_t *queue) {
    /*Obtain the lock and when the queue is at the maximum length, then wait
    until their is an availability in the queue.*/
    pthread_mutex_lock(&queue->lock);
    while (queue->length == CAPACITY) {
        pthread_cond_wait(&queue->queue_full, &queue->lock);
    }


    /*Iterate through all threads in queue*/
    while (queue->shutdown) {
        /*If we reach to the end of the queue, switch shutdown variable to 0. Else,
        call pthread_cond_broadcast to unblock any blocked threads in queue*/
        int i = 0;
        if (i == queue->length) { 
            queue->shutdown = 0;
        } else {
            int broadcast_val = pthread_cond_broadcast(&queue->queue_full);
            if (broadcast_val == -1) { 
                perror("pthread_cond_broadcast");
                return -1;
            }
            
        }
    }

    pthread_cond_signal(&queue->queue_empty);
    pthread_mutex_unlock(&queue->lock);

    return 0;
}

int connection_queue_free(connection_queue_t *queue) {
    /*Obtain the lock and when the queue is at the maximum length, then wait
    until their is an availability in the queue.*/
    pthread_mutex_lock(&queue->lock);
    while (queue->length == CAPACITY) {
        pthread_cond_wait(&queue->queue_full, &queue->lock);
    }

    /*Iterate through all available threads in queue and clear any allocated memory. 
    Afterwards, it sets the length to zero*/
    for (int i = 0; i < queue->length; i++) {
        int destroy_val = pthread_cond_destroy(&queue->queue_full);
        if (destroy_val == -1) {
            perror("pthread_cond_destory");
            return -1;
        }
    }
    
    queue->length = 0;
    pthread_cond_signal(&queue->queue_empty);
    pthread_mutex_unlock(&queue->lock);
    return 0;
}
