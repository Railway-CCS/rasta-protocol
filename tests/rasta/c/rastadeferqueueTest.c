#include <CUnit/Basic.h>
#include "../headers/rastadeferqueueTest.h"
#include "rastadeferqueue.h"

void test_deferqueue_init() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    CU_ASSERT_EQUAL(queue_to_test.max_count, 3);
    CU_ASSERT_EQUAL(queue_to_test.count, 0);
}



void test_deferqueue_destroy() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 1;

    deferqueue_add(&queue_to_test, packet, 42);

    deferqueue_destroy(&queue_to_test);

    CU_ASSERT_EQUAL(queue_to_test.max_count, 0);
    CU_ASSERT_EQUAL(queue_to_test.count, 0);
}

void test_deferqueue_add() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 1;
    unsigned long packet_ts = 42;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 2;
    unsigned long packet2_ts = 43;

    deferqueue_add(&queue_to_test, packet, packet_ts);
    deferqueue_add(&queue_to_test, packet2, packet2_ts);

    CU_ASSERT_EQUAL(queue_to_test.count, 2);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].packet.sequence_number, 1);
    CU_ASSERT_EQUAL(queue_to_test.elements[1].packet.sequence_number, 2);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].received_timestamp, packet_ts);
    CU_ASSERT_EQUAL(queue_to_test.elements[1].received_timestamp, packet2_ts);
}

void test_deferqueue_remove() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 1;
    unsigned long packet_ts = 42;


    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 2;
    unsigned long packet2_ts = 43;

    deferqueue_add(&queue_to_test, packet, packet_ts);
    deferqueue_add(&queue_to_test, packet2, packet2_ts);

    deferqueue_remove(&queue_to_test, 1);

    CU_ASSERT_EQUAL(queue_to_test.count, 1);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].packet.sequence_number, 2);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].received_timestamp, packet2_ts);

    deferqueue_remove(&queue_to_test, 2);

    CU_ASSERT_EQUAL(queue_to_test.count, 0);
}

void test_deferqueue_add_full() {
    struct defer_queue queue_to_test = deferqueue_init(1);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 1;
    unsigned long packet_ts = 42;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 2;
    unsigned long packet2_ts = 43;

    deferqueue_add(&queue_to_test, packet, packet_ts);
    deferqueue_add(&queue_to_test, packet2, packet2_ts);

    CU_ASSERT_EQUAL(queue_to_test.count, 1);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].packet.sequence_number, 1);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].received_timestamp, packet_ts);
}

void test_deferqueue_remove_not_in_queue() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 1;
    unsigned long packet_ts = 42;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 2;
    unsigned long packet2_ts = 43;

    deferqueue_add(&queue_to_test, packet, packet_ts);
    deferqueue_add(&queue_to_test, packet2, packet2_ts);

    deferqueue_remove(&queue_to_test, 3);

    CU_ASSERT_EQUAL(queue_to_test.count, 2);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].packet.sequence_number, 1);
    CU_ASSERT_EQUAL(queue_to_test.elements[1].packet.sequence_number, 2);
    CU_ASSERT_EQUAL(queue_to_test.elements[0].received_timestamp, packet_ts);
    CU_ASSERT_EQUAL(queue_to_test.elements[1].received_timestamp, packet2_ts);
}

void test_deferqueue_contains() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 1;
    unsigned long packet_ts = 42;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 2;
    unsigned long packet2_ts = 43;

    deferqueue_add(&queue_to_test, packet, packet_ts);
    deferqueue_add(&queue_to_test, packet2, packet2_ts);

    int res = deferqueue_contains(&queue_to_test, 1);
    CU_ASSERT_EQUAL(res, 1);

    res = deferqueue_contains(&queue_to_test, 2);
    CU_ASSERT_EQUAL(res, 1);

    res = deferqueue_contains(&queue_to_test, 3);
    CU_ASSERT_EQUAL(res, 0);
}

void test_deferqueue_isfull() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 1;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 2;

    deferqueue_add(&queue_to_test, packet, 1);
    deferqueue_add(&queue_to_test, packet2, 2);

    int res = deferqueue_isfull(&queue_to_test);

    CU_ASSERT_EQUAL(res, 0);


    struct RastaRedundancyPacket packet3;
    packet3.sequence_number = 3;

    deferqueue_add(&queue_to_test, packet3, 3);

    res = deferqueue_isfull(&queue_to_test);
    CU_ASSERT_EQUAL(res, 1);
}

void test_deferqueue_smallestseqnr() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 3;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 1;

    struct RastaRedundancyPacket packet3;
    packet3.sequence_number = 2;

    deferqueue_add(&queue_to_test, packet, 1);
    deferqueue_add(&queue_to_test, packet2, 2);
    deferqueue_add(&queue_to_test, packet3, 3);

    int res = deferqueue_smallest_seqnr(&queue_to_test);

    CU_ASSERT_EQUAL(res, 1);
}

void test_deferqueue_get() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 3;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 1;

    struct RastaRedundancyPacket packet3;
    packet3.sequence_number = 2;

    deferqueue_add(&queue_to_test, packet, 1);
    deferqueue_add(&queue_to_test, packet2, 2);
    deferqueue_add(&queue_to_test, packet3, 3);

    struct RastaRedundancyPacket res = deferqueue_get(&queue_to_test, 1);
    CU_ASSERT_EQUAL(res.sequence_number, 1);

    // not in queue, struct should be completely 0s
    res = deferqueue_get(&queue_to_test, 42);
    CU_ASSERT_EQUAL(res.sequence_number, 0);
}

void test_deferqueue_sorted() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 3;
    unsigned long ts_1 = 2;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 1;
    unsigned long ts_2 = 3;

    struct RastaRedundancyPacket packet3;
    packet3.sequence_number = 2;
    unsigned long ts_3 = 1;

    deferqueue_add(&queue_to_test, packet, ts_1);
    deferqueue_add(&queue_to_test, packet2, ts_2);
    deferqueue_add(&queue_to_test, packet3, ts_3);

    CU_ASSERT_EQUAL(queue_to_test.elements[0].received_timestamp, 1);
    CU_ASSERT_EQUAL(queue_to_test.elements[1].received_timestamp, 2);
    CU_ASSERT_EQUAL(queue_to_test.elements[2].received_timestamp, 3);

    // remove the first element: last element is move to first place
    deferqueue_remove(&queue_to_test, 2);

    CU_ASSERT_EQUAL(queue_to_test.elements[0].received_timestamp, 2);
    CU_ASSERT_EQUAL(queue_to_test.elements[1].received_timestamp, 3);
}

void test_deferqueue_clear() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 3;
    unsigned long ts_1 = 2;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 1;
    unsigned long ts_2 = 3;

    deferqueue_add(&queue_to_test, packet, ts_1);
    deferqueue_add(&queue_to_test, packet2, ts_2);

    deferqueue_clear(&queue_to_test);

    CU_ASSERT_EQUAL(queue_to_test.count, 0);
}

void test_deferqueue_get_ts() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 3;
    unsigned long ts_1 = 2;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 1;
    unsigned long ts_2 = 3;

    deferqueue_add(&queue_to_test, packet, ts_1);
    deferqueue_add(&queue_to_test, packet2, ts_2);

    CU_ASSERT_EQUAL(deferqueue_get_ts(&queue_to_test, 3), ts_1);
    CU_ASSERT_EQUAL(deferqueue_get_ts(&queue_to_test, 1), ts_2);
}

void test_deferqueue_get_ts_doesnt_contain() {
    struct defer_queue queue_to_test = deferqueue_init(3);

    struct RastaRedundancyPacket packet;
    packet.sequence_number = 3;
    unsigned long ts_1 = 2;

    struct RastaRedundancyPacket packet2;
    packet2.sequence_number = 1;
    unsigned long ts_2 = 3;

    deferqueue_add(&queue_to_test, packet, ts_1);
    deferqueue_add(&queue_to_test, packet2, ts_2);

    CU_ASSERT_EQUAL(deferqueue_get_ts(&queue_to_test, 8), 0);
}
