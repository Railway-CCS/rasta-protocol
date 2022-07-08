#include "fifo.h"

#include "rmemory.h"

fifo_t * fifo_init(unsigned int max_size){
    fifo_t * fifo = rmalloc(sizeof(fifo_t));

    fifo->size = 0;
    fifo->max_size = max_size;
    fifo->head = NULL;
    fifo->tail = NULL;

    pthread_mutex_init(&fifo->lock, NULL);

    return fifo;
}

void * fifo_pop(fifo_t * fifo){
    pthread_mutex_lock(&fifo->lock);
    void * res= NULL;

    if (fifo->size > 0 && fifo->head != NULL && fifo->tail != NULL){
        struct fifo_element * element = fifo->head;
        res = element->data;
        fifo->head = fifo->head->next;

        if (fifo->size == 1){
            fifo->tail = NULL;
        }

        rfree(element);
        fifo->size--;
    }

    pthread_mutex_unlock(&fifo->lock);
    return res;
}

void fifo_push(fifo_t * fifo, void * element){
    if (element == NULL){
        return;
    }

    pthread_mutex_lock(&fifo->lock);

    if (fifo->size == fifo->max_size){
        pthread_mutex_unlock(&fifo->lock);
        return;
    }

    struct fifo_element * new_entry = rmalloc(sizeof(struct fifo_element));
    new_entry->data = element;
    new_entry->next = NULL;

    if (fifo->size == 0){
        fifo->head = new_entry;
        fifo->tail = fifo->head;
    } else {
        fifo->tail->next = new_entry;
        fifo->tail = new_entry;
    }

    fifo->size++;

    pthread_mutex_unlock(&fifo->lock);
}

unsigned int fifo_get_size(fifo_t * fifo){
    pthread_mutex_lock(&fifo->lock);
    unsigned int size = fifo->size;
    pthread_mutex_unlock(&fifo->lock);

    return size;
}

void fifo_destroy(fifo_t * fifo){
    while (fifo->size != 0) {
        fifo_pop(fifo);
    }

    pthread_mutex_destroy(&fifo->lock);
    rfree(fifo);
}
