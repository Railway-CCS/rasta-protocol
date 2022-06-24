#include <fifo.h>
#include "fifo.h"
#include "rmemory.h"
#include <stdlib.h>

fifo_t * fifo_init(unsigned int max_size){
    fifo_t * fifo = rmalloc(sizeof(fifo_t));

    fifo->size = 0;
    fifo->max_size = max_size;
    fifo->head = NULL;
    fifo->tail = NULL;

    return fifo;
}

void * fifo_pop(fifo_t * fifo){
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

    return res;
}

void fifo_push(fifo_t * fifo, void * element){
    if (element == NULL){
        return;
    }

    if (fifo->size == fifo->max_size){
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
}

unsigned int fifo_get_size(fifo_t * fifo){
    unsigned int size = fifo->size;

    return size;
}

void fifo_destroy(fifo_t * fifo){
    for (unsigned int i = 0; i < fifo->size; ++i) {
        fifo_pop(fifo);
    }

    rfree(fifo);
}
