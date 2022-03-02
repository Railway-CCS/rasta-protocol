#include <CUnit/Basic.h>
#include <fifo.h>
#include <rmemory.h>
#include <rastautil.h>
#include "fifotest.h"

void test_push(){
    fifo_t * fifo = fifo_init(3);
    CU_ASSERT_EQUAL(fifo_get_size(fifo), 0);
    CU_ASSERT_EQUAL(fifo->head, NULL);
    CU_ASSERT_EQUAL(fifo->tail, NULL);

    struct RastaByteArray elem;
    allocateRastaByteArray(&elem, 10);
    rmemcpy(elem.bytes, "Hello", 6);

    fifo_push(fifo, &elem);

    CU_ASSERT_EQUAL(fifo_get_size(fifo), 1);
    CU_ASSERT_EQUAL(fifo->head->data, &elem);
    CU_ASSERT_EQUAL(fifo->tail->data, &elem);
    CU_ASSERT_EQUAL(fifo->head->next, NULL);
    CU_ASSERT_EQUAL(fifo->tail->next, NULL);

    char * test_str = rmalloc(10);
    rmemcpy(test_str, "Hello", 6);

    fifo_push(fifo, test_str);

    CU_ASSERT_EQUAL(fifo_get_size(fifo), 2);
    CU_ASSERT_EQUAL(fifo->head->data, &elem);
    CU_ASSERT_EQUAL(fifo->tail->data, test_str);
    CU_ASSERT_EQUAL(fifo->head->next->data, test_str);

    struct fifo_element * struct_elem  = rmalloc(sizeof(struct fifo_element));
    fifo_push(fifo, struct_elem);

    CU_ASSERT_EQUAL(fifo_get_size(fifo), 3);
    CU_ASSERT_EQUAL(fifo->head->data, &elem);
    CU_ASSERT_EQUAL(fifo->tail->data, struct_elem);
    CU_ASSERT_EQUAL(fifo->head->next->data, test_str);
    CU_ASSERT_EQUAL(fifo->tail->next, NULL);

    fifo_push(fifo, &elem);

    CU_ASSERT_EQUAL(fifo_get_size(fifo), 3);

    fifo_destroy(fifo);
    rfree(test_str);
    rfree(struct_elem);
}

void test_pop(){
    fifo_t * fifo = fifo_init(3);
    CU_ASSERT_EQUAL(fifo_get_size(fifo), 0);
    CU_ASSERT_EQUAL(fifo->head, NULL);
    CU_ASSERT_EQUAL(fifo->tail, NULL);

    int elem = 42;

    fifo_push(fifo, &elem);

    char * test_str = rmalloc(10);
    rmemcpy(test_str, "Hello", 6);

    fifo_push(fifo, test_str);

    struct fifo_element * struct_elem  = rmalloc(sizeof(struct fifo_element));
    fifo_push(fifo, struct_elem);

    int res = *(int *)fifo_pop(fifo);

    CU_ASSERT_EQUAL(fifo_get_size(fifo), 2);
    CU_ASSERT_EQUAL(res, elem);
    CU_ASSERT_EQUAL(fifo->head->data, test_str);
    CU_ASSERT_EQUAL(fifo->tail->data, struct_elem);
    CU_ASSERT_EQUAL(fifo->head->next, fifo->tail);
    CU_ASSERT_EQUAL(fifo->tail->next, NULL);

    char * res_str = (char *)fifo_pop(fifo);

    CU_ASSERT_EQUAL(fifo_get_size(fifo), 1);
    CU_ASSERT_EQUAL(res_str, test_str);
    CU_ASSERT_EQUAL(fifo->head->data, struct_elem);
    CU_ASSERT_EQUAL(fifo->tail->data, struct_elem);
    CU_ASSERT_EQUAL(fifo->head->next, NULL);
    CU_ASSERT_EQUAL(fifo->tail->next, NULL);

    struct fifo_element * res_struct = (struct fifo_element *)fifo_pop(fifo);

    CU_ASSERT_EQUAL(fifo_get_size(fifo), 0);
    CU_ASSERT_EQUAL(res_struct, struct_elem);
    CU_ASSERT_EQUAL(fifo->head, NULL);
    CU_ASSERT_EQUAL(fifo->tail, NULL);

    void * emtpy_pop_res = fifo_pop(fifo);
    CU_ASSERT_EQUAL(emtpy_pop_res, NULL);

    fifo_destroy(fifo);
    rfree(test_str);
    rfree(struct_elem);
}