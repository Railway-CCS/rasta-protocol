#include "../headers/rastafactoryTest.h"
#include "CUnit/Basic.h"
#include "rastafactory.h"

void checkConnectionPacket() {
    rasta_hashing_context_t context;
    context.algorithm = RASTA_ALGO_MD4;
    rasta_md4_set_key(&context, 0, 0, 0, 0);

    for (int sum = 0; sum < 3; sum++) {
        //preperation
        context.hash_length = sum;
        unsigned char ver[4];
        for (int i = 0; i < 4; i++) {
            ver[i] = (unsigned char) i;
        }
        
        
        struct RastaPacket r = createConnectionRequest(1, 2, 3, 5, 7, ver, &context);

        //check standart values
        CU_ASSERT_EQUAL(r.length, 42 + context.hash_length * 8);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 0);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 0);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_CONNREQ);

        //check specific data
        struct RastaConnectionData con = extractRastaConnectionData(r);
        CU_ASSERT_EQUAL(con.send_max, 7);

        for (int i = 0; i < 4; i++) {
            CU_ASSERT_EQUAL(con.version[i], i);
        }

        r = createConnectionResponse(1,2,3,4,5,6,7,ver, &context);

        //check standart values
        CU_ASSERT_EQUAL(r.length, 42 + context.hash_length * 8);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 4);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 6);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_CONNRESP);

        con = extractRastaConnectionData(r);
        CU_ASSERT_EQUAL(con.send_max, 7);

        for (int i = 0; i < 4; i++) {
            CU_ASSERT_EQUAL(con.version[i], i);
        }

        r.data.length -= 1;
        con = extractRastaConnectionData(r);
        CU_ASSERT_EQUAL(getRastafactoryLastError(),RASTA_ERRORS_WRONG_PACKAGE_FORMAT);
    }
}

void checkNormalPacket() {
    rasta_hashing_context_t hashing_context;
    hashing_context.algorithm = RASTA_ALGO_MD4;
    hashing_context.hash_length = RASTA_CHECKSUM_8B;
    rasta_md4_set_key(&hashing_context, 0, 0, 0, 0);


    struct RastaPacket r;
    for (int sum = 0; sum < 3; sum++) {
        hashing_context.hash_length = sum;

        r = createRetransmissionRequest(1,2,3,4,5,6, &hashing_context);
        //check standart values
        CU_ASSERT_EQUAL(r.length, 28 + hashing_context.hash_length * 8);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 4);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 6);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_RETRREQ);

        r = createRetransmissionResponse(1,2,3,4,5,6, &hashing_context);
        //check standart values
        CU_ASSERT_EQUAL(r.length, 28 + hashing_context.hash_length * 8);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 4);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 6);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_RETRRESP);

        r = createHeartbeat(1,2,3,4,5,6, &hashing_context);
        //check standart values
        CU_ASSERT_EQUAL(r.length, 28 + hashing_context.hash_length * 8);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 4);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 6);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_HB);
    }
}

void checkDisconnectionRequest() {
    rasta_hashing_context_t hashing_context;
    hashing_context.algorithm = RASTA_ALGO_MD4;
    hashing_context.hash_length = RASTA_CHECKSUM_8B;
    rasta_md4_set_key(&hashing_context, 0, 0, 0, 0);

    struct RastaPacket r;
    for (int sum = 0; sum < 3; sum++) {
        hashing_context.hash_length = sum;

        struct RastaDisconnectionData data;
        data.reason = 7;
        data.details = 8;
        r = createDisconnectionRequest(1,2,3,4,5,6, data, &hashing_context);
        CU_ASSERT_EQUAL(r.length, 32 + hashing_context.hash_length * 8);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 4);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 6);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_DISCREQ);

        //check special values
        data = extractRastaDisconnectionData(r);
        CU_ASSERT_EQUAL(data.reason, 7);
        CU_ASSERT_EQUAL(data.details, 8);

        r.data.length -= 1;
        data = extractRastaDisconnectionData(r);
        CU_ASSERT_EQUAL(getRastafactoryLastError(),RASTA_ERRORS_WRONG_PACKAGE_FORMAT);

    }
}

void checkMessagePacket() {
    rasta_hashing_context_t hashing_context;
    hashing_context.algorithm = RASTA_ALGO_MD4;
    hashing_context.hash_length = RASTA_CHECKSUM_8B;
    rasta_md4_set_key(&hashing_context, 0, 0, 0, 0);

    //these are presets, feel free to change them
    struct RastaMessageData data;
    //2*2 -> 2 times length field
    //2+2 the length of the data part
    unsigned short message_length = 2*2+2+2;
    allocateRastaMessageData(&data,2);
    allocateRastaByteArray(&data.data_array[0],2);
    allocateRastaByteArray(&data.data_array[1],2);

    CU_ASSERT_EQUAL(data.count,2);
    CU_ASSERT_EQUAL(data.data_array[0].length,2);
    CU_ASSERT_EQUAL(data.data_array[1].length,2);


    data.data_array[0].bytes[0] = 1;
    data.data_array[0].bytes[1] = 2;
    data.data_array[1].bytes[0] = 3;
    data.data_array[1].bytes[1] = 4;


    struct RastaPacket r;
    struct RastaMessageData m;
    for (int sum = 0; sum < 3; sum++) {
        hashing_context.hash_length = sum;
        //check normal message data
        r = createDataMessage(1,2,3,4,5,6,data, &hashing_context);
        //check standart values
        CU_ASSERT_EQUAL(r.length, 28 + hashing_context.hash_length * 8 + message_length);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 4);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 6);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_DATA);

        //check messagedata
        m = extractMessageData(r);
        CU_ASSERT_EQUAL(m.count,2);
        CU_ASSERT_EQUAL(m.data_array[0].length,2);
        CU_ASSERT_EQUAL(m.data_array[1].length,2);

        CU_ASSERT_EQUAL(m.data_array[0].bytes[0],1);
        CU_ASSERT_EQUAL(m.data_array[0].bytes[1],2);
        CU_ASSERT_EQUAL(m.data_array[1].bytes[0],3);
        CU_ASSERT_EQUAL(m.data_array[1].bytes[1],4);

        freeRastaMessageData(&m);


        //check retransmitted message data
        r = createRetransmittedDataMessage(1,2,3,4,5,6,data, &hashing_context);
        //check standart values
        CU_ASSERT_EQUAL(r.length, 28 + hashing_context.hash_length * 8 + message_length);
        CU_ASSERT_EQUAL(r.receiver_id, 1);
        CU_ASSERT_EQUAL(r.sender_id, 2);
        CU_ASSERT_EQUAL(r.sequence_number, 3);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, 4);
        CU_ASSERT_EQUAL(r.timestamp, 5);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, 6);
        CU_ASSERT_EQUAL(r.type, RASTA_TYPE_RETRDATA);

        //check messagedata
        m = extractMessageData(r);
        CU_ASSERT_EQUAL(m.count,2);
        CU_ASSERT_EQUAL(m.data_array[0].length,2);
        CU_ASSERT_EQUAL(m.data_array[1].length,2);

        CU_ASSERT_EQUAL(m.data_array[0].bytes[0],1);
        CU_ASSERT_EQUAL(m.data_array[0].bytes[1],2);
        CU_ASSERT_EQUAL(m.data_array[1].bytes[0],3);
        CU_ASSERT_EQUAL(m.data_array[1].bytes[1],4);

        freeRastaMessageData(&m);
    }

    freeRastaMessageData(&data);
}

void testCreateRedundancyPacket() {
    // create test inner packet
    struct RastaPacket inner_test;
    inner_test.length = 10;
    inner_test.sequence_number = 42;

    // crc opt b = 32bit/4byte width
    struct RastaRedundancyPacket pdu_to_test = createRedundancyPacket(1, inner_test, crc_init_opt_b());

    unsigned short expected_len = 8 + 10 + 4;

    CU_ASSERT_EQUAL(pdu_to_test.reserve, 0x0000);
    CU_ASSERT_EQUAL(pdu_to_test.checksum_correct, 1);
    CU_ASSERT_EQUAL(pdu_to_test.data.sequence_number, 42);
    CU_ASSERT_EQUAL(pdu_to_test.checksum_type.width, crc_init_opt_b().width);
    CU_ASSERT_EQUAL(pdu_to_test.length, expected_len);
}

void testCreateRedundancyPacketNoChecksum() {
    // create test inner packet
    struct RastaPacket inner_test;
    inner_test.length = 10;
    inner_test.sequence_number = 42;

    // crc opt a = no crc / 0 bit width
    struct RastaRedundancyPacket pdu_to_test = createRedundancyPacket(1, inner_test, crc_init_opt_a());

    unsigned short expected_len = 8 + 10 + 0;

    CU_ASSERT_EQUAL(pdu_to_test.reserve, 0x0000);
    CU_ASSERT_EQUAL(pdu_to_test.checksum_correct, 1);
    CU_ASSERT_EQUAL(pdu_to_test.data.sequence_number, 42);
    CU_ASSERT_EQUAL(pdu_to_test.checksum_type.width, crc_init_opt_a().width);
    CU_ASSERT_EQUAL(pdu_to_test.length, expected_len);
}
