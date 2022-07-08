#include <stdlib.h>
#include "rmemory.h"
#include "rastadeferqueue.h"

/**
 * finds the index of a given element inside the given queue.
 * The sequence_number is used as the unique identifier
 * @param queue the queue that will be searched
 * @param seq_nr the sequence number to be located
 * @return -1 if there is no element with the specified @p seq_nr, index of the element otherwise
 */
int find_index(struct defer_queue * queue, unsigned long seq_nr){
    unsigned int index = 0;

    // naive implementation of search. performance shouldn't be an issue as the amount of messages in the queue is small
    while ( index < queue->max_count && queue->elements[index].packet.sequence_number != seq_nr ) ++index;

    return ( index == queue->max_count ? -1 : (int)index );
}

int cmpfkt(const void * a, const void * b){
    long long a_ts = (long long)((struct rasta_redundancy_packet_wrapper*)a)->received_timestamp;
    long long b_ts = (long long)((struct rasta_redundancy_packet_wrapper*)b)->received_timestamp;

    return (int) (a_ts - b_ts);
}

/**
 * sorts the elements in the queue in ascending time (first element has oldest timestamp)
 * @param queue the queue that will be sorted
 */
void sort(struct defer_queue * queue){
    qsort(queue->elements, queue->count, sizeof(struct rasta_redundancy_packet_wrapper), cmpfkt);
}

struct defer_queue deferqueue_init(unsigned int n_max){
    struct defer_queue queue;

    // allocate the array
    queue.elements = rmalloc(n_max * sizeof(struct rasta_redundancy_packet_wrapper));

    // set count to 0
    queue.count = 0;

    // set max count
    queue.max_count = n_max;

    // init the mutex
    pthread_mutex_init(&queue.mutex, NULL);

    return queue;
}

int deferqueue_isfull(struct defer_queue * queue){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    int result = (queue->count == queue->max_count);

    // free lock
    pthread_mutex_unlock(&queue->mutex);

    return result;
}

void deferqueue_add(struct defer_queue * queue, struct RastaRedundancyPacket packet, unsigned long recv_ts){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    if((queue->count == queue->max_count)){
        // queue full, return
        pthread_mutex_unlock(&queue->mutex);
        return;
    }

    struct rasta_redundancy_packet_wrapper element;
    element.packet = packet;
    element.received_timestamp = recv_ts;

    // add element to the end
    queue->elements[queue->count] = element;

    // increase count
    queue->count = queue->count + 1;

    // sort array
    sort(queue);

    // free lock
    pthread_mutex_unlock(&queue->mutex);
}

void deferqueue_remove(struct defer_queue * queue, unsigned long seq_nr){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    int index = find_index(queue, seq_nr);
    if(index == -1){
        pthread_mutex_unlock(&queue->mutex);
        // element not in queue
        return;
    }

    // free element with sequence number seq_nr
    freeRastaByteArray(&queue->elements[index].packet.data.data);

    if(index != ((int)queue->count - 1)){
        // element to delete isn't at the last position
        // to be able to add the next element to the last position without overriding something
        // the currently last element is moved to the index where the deleted element is located
        queue->elements[index] = queue->elements[queue->count-1];
    }

    // decrease counter
    queue->count = queue->count -1;

    // sort the array
    sort(queue);

    // free lock
    pthread_mutex_unlock(&queue->mutex);
}

int deferqueue_contains(struct defer_queue * queue, unsigned long seq_nr){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    int result = (find_index(queue, seq_nr) != -1);

    // free lock
    pthread_mutex_unlock(&queue->mutex);

    return result;
}

void deferqueue_destroy(struct defer_queue * queue){
    rfree(queue->elements);

    queue->count = 0;
    queue->max_count= 0;

    pthread_mutex_destroy(&queue->mutex);
}

int deferqueue_smallest_seqnr(struct defer_queue * queue){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    int index = 0;

    // largest number possible
    unsigned long smallest = 0xFFFFFFFF;

    // naive implementation of search. performance shouldn't be an issue as the amount of messages in the queue is small
    for (unsigned int i = 0; i < queue->max_count; ++i) {
        if(queue->elements[i].packet.sequence_number < smallest){
            smallest = queue->elements[i].packet.sequence_number;
            index = i;
        }
    }

    // free lock
    pthread_mutex_unlock(&queue->mutex);

    return index;
}

struct RastaRedundancyPacket deferqueue_get(struct defer_queue * queue, unsigned long seq_nr){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    int index = find_index(queue, seq_nr);

    if(index == -1){
        pthread_mutex_unlock(&queue->mutex);
        // element not in queue, return uninitialized struct
        return (const struct RastaRedundancyPacket){ 0 };
    }

    struct RastaRedundancyPacket result = queue->elements[index].packet;

    // free lock
    pthread_mutex_unlock(&queue->mutex);

    // return element at index
    return result;
}

unsigned long deferqueue_get_ts(struct defer_queue * queue, unsigned long seq_nr){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    int index = find_index(queue, seq_nr);

    if(index == -1){
        pthread_mutex_unlock(&queue->mutex);
        // element not in queue, return uninitialized struct
        return 0;
    }

    unsigned long result = queue->elements[index].received_timestamp;

    // free lock
    pthread_mutex_unlock(&queue->mutex);

    // return element at index
    return result;
}

void deferqueue_clear(struct defer_queue * queue){
    // acquire lock
    pthread_mutex_lock(&queue->mutex);

    // just set count to 0, elements that are in the queue will be overridden
    queue->count = 0;

    // free lock
    pthread_mutex_unlock(&queue->mutex);
}
