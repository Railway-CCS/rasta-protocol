//
// Created by tobia on 28.11.2017.
//

#include "../headers/rastamoduleTest.h"
#include "rastamodule.h"

#include "CUnit/Basic.h"

void testConversion(){
    rasta_hashing_context_t context;
    context.algorithm = RASTA_ALGO_MD4;
    rasta_md4_set_key(&context, 1, 2, 3, 4);

    //check
    for (int i = 0; i <=2; i++) {
        context.hash_length = i;

        struct RastaPacket r;
        r.length = 30 + i*8;
        r.type = RASTA_TYPE_CONNRESP;
        r.sender_id = 12345;
        r.receiver_id = 54321;
        r.sequence_number = 1357;
        r.confirmed_sequence_number = 7531;
        r.timestamp = 2468;
        r.confirmed_timestamp = 8642;
        allocateRastaByteArray(&r.data, 2);

        r.data.bytes[0] = 0x11;
        r.data.bytes[1] = 0x22;

        struct RastaByteArray data;
        data = rastaModuleToBytes(r, &context);

        CU_ASSERT_EQUAL(getRastamoduleLastError(), RASTA_ERRORS_NONE);

        struct RastaPacket s;
        s = bytesToRastaPacket(data, &context);
        CU_ASSERT_EQUAL(getRastamoduleLastError(), RASTA_ERRORS_NONE);

        CU_ASSERT_EQUAL(r.length, s.length);
        CU_ASSERT_EQUAL(r.type, s.type);
        CU_ASSERT_EQUAL(r.sender_id, s.sender_id);
        CU_ASSERT_EQUAL(r.receiver_id, s.receiver_id);
        CU_ASSERT_EQUAL(r.sequence_number, s.sequence_number);
        CU_ASSERT_EQUAL(r.confirmed_sequence_number, s.confirmed_sequence_number);
        CU_ASSERT_EQUAL(r.timestamp, s.timestamp);
        CU_ASSERT_EQUAL(r.confirmed_timestamp, s.confirmed_timestamp);

        CU_ASSERT_EQUAL(r.data.length, s.data.length);

        for (int n = 0; n < r.data.length; n++) {
            CU_ASSERT_EQUAL(r.data.bytes[n], s.data.bytes[n]);
        }

        //check if checksum is correct
        CU_ASSERT_EQUAL(s.checksum_correct, 1);



        //manipulate data
        data.bytes[8] = 0x43;
        s = bytesToRastaPacket(data, &context);

        if (i != 0) CU_ASSERT_EQUAL(s.checksum_correct, 0) else CU_ASSERT_EQUAL(s.checksum_correct, 1);


        //check data failure
        r.length = 2;
        s = bytesToRastaPacket(rastaModuleToBytes(r, &context), &context);

        CU_ASSERT_EQUAL(getRastamoduleLastError(), RASTA_ERRORS_PACKAGE_LENGTH_INVALID);
    }


}

void testRedundancyConversionWithCrcChecksumCorrect() {
    rasta_hashing_context_t context;
    context.algorithm = RASTA_ALGO_MD4;
    context.hash_length = RASTA_CHECKSUM_8B;
    rasta_md4_set_key(&context, 0, 0, 0 ,0);

    struct RastaRedundancyPacket packet_to_test;
    struct RastaPacket r;
    r.length = 38;
    r.type = 6543;
    r.sender_id = 12345;
    r.receiver_id = 54321;
    r.sequence_number = 1357;
    r.confirmed_sequence_number = 7531;
    r.timestamp = 2468;
    r.confirmed_timestamp = 8642;
    allocateRastaByteArray(&r.data,2);

    r.data.bytes[0] = 0x11;
    r.data.bytes[1] = 0x22;

    packet_to_test.length = 50;
    packet_to_test.reserve = 0;
    packet_to_test.sequence_number = 1;
    packet_to_test.checksum_type = crc_init_opt_b();
    packet_to_test.data = r;

    struct RastaByteArray convertedToBytes;
    convertedToBytes = rastaRedundancyPacketToBytes(packet_to_test, &context);

    struct RastaRedundancyPacket convertedFromBytes;
    convertedFromBytes = bytesToRastaRedundancyPacket(convertedToBytes, crc_init_opt_b(), &context);

    CU_ASSERT_EQUAL(convertedFromBytes.length, packet_to_test.length);
    CU_ASSERT_EQUAL(convertedFromBytes.reserve, packet_to_test.reserve);

    CU_ASSERT_EQUAL(convertedFromBytes.sequence_number, packet_to_test.sequence_number);
    CU_ASSERT_EQUAL(convertedFromBytes.checksum_correct, 1);

    // internal package
    CU_ASSERT_EQUAL(convertedFromBytes.data.length, r.length);
    CU_ASSERT_EQUAL(convertedFromBytes.data.type,r.type);
    CU_ASSERT_EQUAL(convertedFromBytes.data.sender_id,r.sender_id);
    CU_ASSERT_EQUAL(convertedFromBytes.data.receiver_id,r.receiver_id);
    CU_ASSERT_EQUAL(convertedFromBytes.data.sequence_number,r.sequence_number);
    CU_ASSERT_EQUAL(convertedFromBytes.data.confirmed_sequence_number,r.confirmed_sequence_number);
    CU_ASSERT_EQUAL(convertedFromBytes.data.timestamp,r.timestamp);
    CU_ASSERT_EQUAL(convertedFromBytes.data.confirmed_timestamp,r.confirmed_timestamp);


    CU_ASSERT_EQUAL(convertedFromBytes.data.data.length, r.data.length);

    for (int i = 0; i < r.data.length; i++) {
        CU_ASSERT_EQUAL(convertedFromBytes.data.data.bytes[i], r.data.bytes[i]);
    }

    //check if internal packet checksum is correct
    CU_ASSERT_EQUAL(convertedFromBytes.data.checksum_correct,1);
}

void testRedundancyConversionWithoutChecksum() {
    rasta_hashing_context_t context;
    context.algorithm = RASTA_ALGO_MD4;
    context.hash_length = RASTA_CHECKSUM_8B;
    rasta_md4_set_key(&context, 0, 0, 0 ,0);

    struct RastaRedundancyPacket packet_to_test;
    struct RastaPacket r;
    r.length = 38;
    r.type = 6543;
    r.sender_id = 12345;
    r.receiver_id = 54321;
    r.sequence_number = 1357;
    r.confirmed_sequence_number = 7531;
    r.timestamp = 2468;
    r.confirmed_timestamp = 8642;
    allocateRastaByteArray(&r.data,2);

    r.data.bytes[0] = 0x11;
    r.data.bytes[1] = 0x22;

    packet_to_test.length = 50;
    packet_to_test.reserve = 0;
    packet_to_test.sequence_number = 1;
    packet_to_test.checksum_type = crc_init_opt_a();
    packet_to_test.data = r;

    struct RastaByteArray convertedToBytes;
    convertedToBytes = rastaRedundancyPacketToBytes(packet_to_test, &context);

    struct RastaRedundancyPacket convertedFromBytes;
    convertedFromBytes = bytesToRastaRedundancyPacket(convertedToBytes, crc_init_opt_a(), &context);

    CU_ASSERT_EQUAL(convertedFromBytes.length, packet_to_test.length);
    CU_ASSERT_EQUAL(convertedFromBytes.reserve, packet_to_test.reserve);
    CU_ASSERT_EQUAL(convertedFromBytes.sequence_number, packet_to_test.sequence_number);
    CU_ASSERT_EQUAL(convertedFromBytes.checksum_correct, 1);

    // internal package
    CU_ASSERT_EQUAL(convertedFromBytes.data.length, r.length);
    CU_ASSERT_EQUAL(convertedFromBytes.data.type,r.type);
    CU_ASSERT_EQUAL(convertedFromBytes.data.sender_id,r.sender_id);
    CU_ASSERT_EQUAL(convertedFromBytes.data.receiver_id,r.receiver_id);
    CU_ASSERT_EQUAL(convertedFromBytes.data.sequence_number,r.sequence_number);
    CU_ASSERT_EQUAL(convertedFromBytes.data.confirmed_sequence_number,r.confirmed_sequence_number);
    CU_ASSERT_EQUAL(convertedFromBytes.data.timestamp,r.timestamp);
    CU_ASSERT_EQUAL(convertedFromBytes.data.confirmed_timestamp,r.confirmed_timestamp);

    CU_ASSERT_EQUAL(convertedFromBytes.data.data.length, r.data.length);

    for (int i = 0; i < r.data.length; i++) {
        CU_ASSERT_EQUAL(convertedFromBytes.data.data.bytes[i], r.data.bytes[i]);
    }

    //check if internal packet checksum is correct
    CU_ASSERT_EQUAL(convertedFromBytes.data.checksum_correct,1);
}

void testRedundancyConversionIncorrectChecksum() {
    rasta_hashing_context_t context;
    context.algorithm = RASTA_ALGO_MD4;
    context.hash_length = RASTA_CHECKSUM_8B;
    rasta_md4_set_key(&context, 0, 0, 0 ,0);

    struct RastaRedundancyPacket packet_to_test;
    struct RastaPacket r;
    r.length = 38;
    r.type = 6543;
    r.sender_id = 12345;
    r.receiver_id = 54321;
    r.sequence_number = 1357;
    r.confirmed_sequence_number = 7531;
    r.timestamp = 2468;
    r.confirmed_timestamp = 8642;
    allocateRastaByteArray(&r.data,2);

    r.data.bytes[0] = 0x11;
    r.data.bytes[1] = 0x22;

    packet_to_test.length = 50;
    packet_to_test.reserve = 0;
    packet_to_test.sequence_number = 1;
    packet_to_test.checksum_type = crc_init_opt_b();
    packet_to_test.data = r;

    struct RastaByteArray convertedToBytes;
    convertedToBytes = rastaRedundancyPacketToBytes(packet_to_test, &context);

    // simulate error in packet transmission
    convertedToBytes.bytes[16] = 0x42;

    struct RastaRedundancyPacket convertedFromBytes;
    convertedFromBytes = bytesToRastaRedundancyPacket(convertedToBytes, crc_init_opt_b(), &context);

    //check if internal packet checksum is incorrect
    CU_ASSERT_EQUAL(convertedFromBytes.data.checksum_correct,0);
}

