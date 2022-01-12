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
    if (list->size >= list->actual_size) {
        rastalist_change_size(list, list->actual_size * 2);
    }

    list->data[list->size] = item;
    list->size++;
    unsigned int return_size = list->size -1;

    return return_size;

}

void rastalist_remove(struct RastaList *list, unsigned int id) {

    if (id >= list->size){
        return;
    }

    for (unsigned int i = id; i < list->size -1; i++) {
        list->data[i] = list->data[i+1];

    }

    list->size--;
}

unsigned int rastalist_count(struct RastaList *list) {
    unsigned int size = list->size;
    return size;
}

struct rasta_connection * rastalist_getConnection(struct RastaList *list, unsigned int id) {
    struct rasta_connection* con = 0;
    if (id < list->size){
        con=&list->data[id];
    }
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

    for (unsigned int i = 0; i < list->size; i++) {
        if (list->data[i].remote_id == remote_id) {
            id = i;
            break;
        }
    }

    return id;
}



struct RastaList rastalist_create(unsigned int initial_size) {
    if (initial_size < 2) initial_size = 2;
    struct RastaList result;
    result.size = 0;
    result.actual_size = initial_size;

    result.data = rmalloc(sizeof(struct rasta_connection) * initial_size);

    return result;
}

void rastalist_free(struct RastaList* list) {

    if (list->actual_size != 0) {
        list->actual_size = 0;
        list->size = 0;

        rfree(list->data);

    }
}

