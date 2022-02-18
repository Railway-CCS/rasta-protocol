#ifndef LST_SIMULATOR_RASTADEFERQUEUETEST_H
#define LST_SIMULATOR_RASTADEFERQUEUETEST_H

/**
 * test if the initialization is working
 */
void test_deferqueue_init();

/**
 * test if adding elements to the queue works
 */
void test_deferqueue_add();

/**
 * test if adding to a already full queue is working
 */
void test_deferqueue_add_full();

/**
 * test if removing elements is working
 */
void test_deferqueue_remove();

/**
 * test if removing elements is working if the element is not in the queue
 */
void test_deferqueue_remove_not_in_queue();

/**
 * test if the contains function is working
 */
void test_deferqueue_contains();

/**
 * test if the isfull function is working
 */
void test_deferqueue_isfull();

/**
 * test if the search for smallest seq. nr. is working
 */
void test_deferqueue_smallestseqnr();

/**
 * test if the cleanup is working
 */
void test_deferqueue_destroy();

/**
 * test the get function
 */
void test_deferqueue_get();

/**
 * test if the array is sorted after add and delete
 */
void test_deferqueue_sorted();

/**
 * test if the array is empty after calling clear
 */
void test_deferqueue_clear();

/**
 * test if the retrieval of a timestamp is working
 */
void test_deferqueue_get_ts();

/**
 * test if the retrieval of timestamp is working as expected when the seq nr is not in the queue
 */
void test_deferqueue_get_ts_doesnt_contain();



#endif //LST_SIMULATOR_RASTADEFERQUEUETEST_H
