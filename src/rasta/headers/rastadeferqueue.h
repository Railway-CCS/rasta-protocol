/**
 * a simple implementation of the defer queue which is used in the redundancy layer
 * This implementation uses an arraylist for storage of the elements
 * the sequence number is used as an unique identifier
 */

#ifndef LST_SIMULATOR_RASTADEFERQUEUE_H
#define LST_SIMULATOR_RASTADEFERQUEUE_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "rastamodule.h"

/**
 * representation of a RaSTA redundancy layer PDU with the corresponding time it was received
 * this is used as the element type in the defer queue
 */
struct rasta_redundancy_packet_wrapper{
    struct RastaRedundancyPacket packet;
    unsigned long received_timestamp;
};

/**
 * representation of the defer queue
 */
struct defer_queue{
    /**
     * the stored elements
     */
    struct rasta_redundancy_packet_wrapper * elements;

    /**
     * the amount of elements that are currently in the queue
     */
    unsigned int count;

    /**
     * maximum number of elements in the queue
     */
    unsigned int max_count;
};

/**
 * initializes an empty queue with the given amount of elements
 * @param n_max maximum amount of elements the queue can hold
 * @return an initialized defer queue
 */
struct defer_queue deferqueue_init(unsigned int n_max);

/**
 * frees the memory for all elements
 * @param queue the queue that will be destroyed
 */
void deferqueue_destroy(struct defer_queue * queue);

/**
 * adds an element to the queue if the queue is not full and the element isn't already in the queue.
 * The sequence_number of the element is used as an unique identifier
 * @param queue the queue where the @p element will be added
 * @param packet the element that will be added
 * @param recv_ts the timestamp when the @p element was received
 */
void deferqueue_add(struct defer_queue * queue, struct RastaRedundancyPacket packet, unsigned long recv_ts);

/**
 * removes the given element from the queue if it exists.
 * @param queue the queue where the element is removed
 * @param seq_nr the element with this sequence_number will be removed
 */
void deferqueue_remove(struct defer_queue * queue, unsigned long seq_nr);

/**
 * checks if the queue contains the element with the given sequence_number
 * @param queue the queue that will be searched
 * @param seq_nr the sequence_number of the checked element
 * @return 1 if the element if in the queue, 0 otherwise
 */
int deferqueue_contains(struct defer_queue * queue, unsigned long seq_nr);

/**
 * checks if the given @p queue is full
 * @param queue the queue that is used
 * @return 1 if the queue is full, 0 otherwise
 */
int deferqueue_isfull(struct defer_queue * queue);

/**
 * finds the element with the smallest sequence_number
 * @param queue the queue that will be searched
 * @return the index of the element with the smallest sequence_number
 */
int deferqueue_smallest_seqnr(struct defer_queue * queue);

/**
 * gets the element with the specified sequence_number out of in the defer queue
 * @param queue the queue that will be used
 * @param seq_nr the sequence_number that is searched for
 * @return the element with the given @p seq_nr if the element exists in the queue.
 * Otherwise an uninitialized struct will be returned! check with contains beforehand
 */
struct RastaRedundancyPacket deferqueue_get(struct defer_queue * queue, unsigned long seq_nr);

/**
 * gets the timestamp of the element with the specified sequence_number out of in the defer queue
 * @param queue the queue that will be used
 * @param seq_nr the sequence_number that is searched for
 * @return the timestamp of the element with the given @p seq_nr if the element exists in the queue.
 * Otherwise 0
 */
unsigned long deferqueue_get_ts(struct defer_queue * queue, unsigned long seq_nr);

/**
 * removes all elements from the @p queue
 * @param queue the queue that is cleared
 */
void deferqueue_clear(struct defer_queue * queue);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTADEFERQUEUE_H
