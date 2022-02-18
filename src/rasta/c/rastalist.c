//
// Created by tobia on 05.02.2018.
//

#include "rastalist.h"
#include "rmemory.h"


/**
 * reallocates memory for the data structures
 * @param list
 * @param size
 */
void rastalist_change_size(struct RastaList *list, unsigned int size) {
    list->data = rrealloc(list->data, size * sizeof(struct rasta_connection));
    list->actual_size = size;
}




int rastalist_addConnection(struct RastaList *list, struct rasta_connection item) {
    pthread_mutex_lock(&list->list_lock);

    if (list->size >= list->actual_size) {
        rastalist_change_size(list, list->actual_size * 2);
    }

    list->data[list->size] = item;
    list->size++;
    unsigned int return_size = list->size -1;

    pthread_mutex_unlock(&list->list_lock);

    return return_size;

}

void rastalist_remove(struct RastaList *list, unsigned int id) {
    pthread_mutex_lock(&list->list_lock);

    if (id >= list->size){
        pthread_mutex_unlock(&list->list_lock);
        return;
    }

    for (unsigned int i = id; i < list->size -1; i++) {
        list->data[i] = list->data[i+1];

    }

    list->size--;
    pthread_mutex_unlock(&list->list_lock);
}

unsigned int rastalist_count(struct RastaList *list) {
    pthread_mutex_lock(&list->list_lock);
    unsigned int size = list->size;
    pthread_mutex_unlock(&list->list_lock);
    return size;
}

struct rasta_connection * rastalist_getConnection(struct RastaList *list, unsigned int id) {
    struct rasta_connection* con = 0;
    pthread_mutex_lock(&list->list_lock);
    if (id < list->size){
        con=&list->data[id];
    }
    pthread_mutex_unlock(&list->list_lock);
    return con;
}

struct rasta_connection * rastalist_getConnectionByRemote(struct RastaList *list, unsigned long remote_id) {
    int id = rastalist_getConnectionId(list,remote_id);
    if (id != -1) {
        return rastalist_getConnection(list, (unsigned int)id);
    }
    else return 0;

}

int rastalist_getConnectionId(struct RastaList *list, unsigned long remote_id) {
    int id = -1;

    pthread_mutex_lock(&list->list_lock);
    for (unsigned int i = 0; i < list->size; i++) {
        if (list->data[i].remote_id == remote_id) {
            id = i;
            break;
        }
    }
    pthread_mutex_unlock(&list->list_lock);

    return id;
}



struct RastaList rastalist_create(unsigned int initial_size) {
    if (initial_size < 2) initial_size = 2;
    struct RastaList result;
    result.size = 0;
    result.actual_size = initial_size;

    result.data = rmalloc(sizeof(struct rasta_connection) * initial_size);

    pthread_mutex_init(&result.list_lock, NULL);

    return result;
}

void rastalist_free(struct RastaList* list) {

    if (list->actual_size != 0) {
        list->actual_size = 0;
        list->size = 0;

        rfree(list->data);

    }

    pthread_mutex_destroy(&list->list_lock);
}

