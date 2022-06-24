#include "rastamodule.h"
#include "rmemory.h"
#include <endian.h>

//
// Created by tobia on 27.11.2017.
//

rasta_error_type rastamodule_lasterror = RASTA_ERRORS_NONE;



rasta_error_type getRastamoduleLastError() {
    rasta_error_type temp = rastamodule_lasterror;
    rastamodule_lasterror = RASTA_ERRORS_NONE;
    return temp;
}
/**
 * Converts a unsigned short into a uchar array in little-endian byte order
 * @param v the ushort
 * @param result the assigned uchar array; length should be 2
 */
void hostShortTole(uint16_t v, unsigned char* result) {
    uint16_t *target = (uint16_t *) result;
    *target = htole16(v);
}

/**
 * Converts a uchar pointer to a ushort in host byte order
 * @param v pointer to 2 bytes in little-endian byte order
 * @return the ushort
 */
uint16_t leShortToHost(const unsigned char *v) {
    const uint16_t *value_network_byte_order = (uint16_t *) v;
    return le16toh(*value_network_byte_order);
}


unsigned int getDataLength(struct RastaPacket packet, rasta_hashing_context_t * hashing_context) {
    //calculated depending on the checksum type
    unsigned int length = packet.length - 28 - hashing_context->hash_length * 8;
    return (length > 0) ? (unsigned int)length : 0;
}


/*
 * Public definitions
 */

/**
 * returns an allocated array with equal length of the data array from the packet
 * @param packet the packet
 * @return
 */
struct RastaByteArray allocateBytes(struct RastaPacket packet, rasta_hashing_context_t * hashing_context) {
    struct RastaByteArray result;
    allocateRastaByteArray(&result,packet.length);
    //check packet length
    unsigned int checksum_len = hashing_context->hash_length * 8;
    if (packet.length < (28+checksum_len)) rastamodule_lasterror = RASTA_ERRORS_PACKAGE_LENGTH_INVALID;
    return result;
}

/**
 * Packs all fields Sn, ts, etc. excluding safety code and data into the result array
 * @param result the array to pack to
 * @param packet the packet to pack
 */
void packFields(struct RastaByteArray result, struct RastaPacket packet) {

    //pack message length
    hostShortTole(packet.length, &result.bytes[0]);

    //pack message type
    uint16_t type = packet.type;

    hostShortTole(type, &result.bytes[2]);

    //pack receiver id
    hostLongToLe(packet.receiver_id, &result.bytes[4]);

    //pack sender id
    hostLongToLe(packet.sender_id, &result.bytes[8]);

    //pack sequence number
    hostLongToLe(packet.sequence_number, &result.bytes[12]);

    //pack confirmed sequence number
    hostLongToLe(packet.confirmed_sequence_number, &result.bytes[16]);

    //pack timestamp
    hostLongToLe(packet.timestamp, &result.bytes[20]);

    //pack confirmed timestamp
    hostLongToLe(packet.confirmed_timestamp, &result.bytes[24]);
}


struct RastaByteArray rastaModuleToBytes(struct RastaPacket packet, rasta_hashing_context_t * hashing_context) {
    struct RastaByteArray result;
    result = allocateBytes(packet, hashing_context);

    if (rastamodule_lasterror != RASTA_ERRORS_NONE) return result;

    packFields(result, packet);

    //pack data
    unsigned int len = getDataLength(packet, hashing_context);
    for (unsigned int i = 0; i < len; i++) {
        result.bytes[28+i] = packet.data.bytes[i];
    }

    //calculate md4 checksum
    unsigned char checksum[16];
    unsigned int checksum_len = hashing_context->hash_length * 8;

    struct RastaByteArray data_to_hash;
    allocateRastaByteArray(&data_to_hash, (unsigned int)(packet.length - checksum_len));
    rmemcpy(data_to_hash.bytes, result.bytes, (unsigned int)(packet.length - checksum_len));

    rasta_calculate_hash(data_to_hash, hashing_context, checksum);

    freeRastaByteArray(&data_to_hash);

    for (unsigned int i = 0; i < checksum_len; i++) {
        result.bytes[28+len+i] = checksum[i];
    }

    return result;
}

struct RastaByteArray rastaModuleToBytesNoChecksum(struct RastaPacket packet, rasta_hashing_context_t * hashing_context) {
    struct RastaByteArray result;
    result = allocateBytes(packet, hashing_context);

    if (rastamodule_lasterror != RASTA_ERRORS_NONE) return result;

    packFields(result, packet);

    //pack data
    unsigned int len = getDataLength(packet, hashing_context);
    /*for (int i = 0; i < len; i++) {
        result.bytes[28+i] = packet.data.bytes[i];
    }*/
    rmemcpy(&result.bytes[28], packet.data.bytes, len);


    //pack checksum
    unsigned int checksum_len = hashing_context->hash_length * 8;
    /*for (int j = 0; j < checksum_len; ++j) {
        result.bytes[28+len+j] = packet.checksum.bytes[j];
    }*/
    rmemcpy(&result.bytes[28 + len], packet.checksum.bytes, checksum_len);

    return result;
}

struct RastaPacket bytesToRastaPacket(struct RastaByteArray data, rasta_hashing_context_t * hashing_context) {
    struct RastaPacket result;

    result.checksum_correct = 1;

    if (data.length < 28 + hashing_context->hash_length * 8) {
        result.length = 0;
        result.checksum_correct = 0;
        rastamodule_lasterror = RASTA_ERRORS_PACKAGE_LENGTH_INVALID;
        return result;
    }

    //length
    result.length = leShortToHost(&data.bytes[0]);



    //type
    result.type = (rasta_conn_type) leShortToHost(&data.bytes[2]);

    //receiver id
    result.receiver_id = leLongToHost(&data.bytes[4]);

    //sender id
    result.sender_id = leLongToHost(&data.bytes[8]);

    //sequence number
    result.sequence_number = leLongToHost(&data.bytes[12]);

    //confirmed sequence number
    result.confirmed_sequence_number = leLongToHost(&data.bytes[16]);

    //timestamp
    result.timestamp = leLongToHost(&data.bytes[20]);

    //confirmed timestamp
    result.confirmed_timestamp = leLongToHost(&data.bytes[24]);

    //data
    result.data.length = 0;

    unsigned int len = getDataLength(result, hashing_context);
    allocateRastaByteArray(&result.data,len);

    for (unsigned int i = 0; i < len; i++) {
        result.data.bytes[i] = data.bytes[28+i];
    }

    //checksum
    unsigned char checksum[16];
    unsigned int checksum_len = hashing_context->hash_length * 8;

    struct RastaByteArray data_to_hash;
    allocateRastaByteArray(&data_to_hash, (unsigned int)(result.length - checksum_len));

    rmemcpy(data_to_hash.bytes, data.bytes, data_to_hash.length);

    rasta_calculate_hash(data_to_hash, hashing_context, checksum);

    freeRastaByteArray(&data_to_hash);


    allocateRastaByteArray(&result.checksum, checksum_len);
    rmemcpy(result.checksum.bytes, &data.bytes[28+len], checksum_len);
    result.checksum_correct = (rmemcmp(&checksum, &data.bytes[28+len], checksum_len) == 0);

    return result;
}


struct RastaByteArray rastaRedundancyPacketToBytes(struct RastaRedundancyPacket packet, rasta_hashing_context_t * hashing_context){
    uint8_t checksum_storage[sizeof(uint32_t)];
    struct RastaByteArray result;
    allocateRastaByteArray(&result, packet.length);

    // pack packet length
    hostShortTole(packet.length, &result.bytes[0]);

    // pack reserve bytes
    hostShortTole(packet.reserve, &result.bytes[2]);

    //pack sequence number
    hostLongToLe(packet.sequence_number, &result.bytes[4]);

    // convert internal RaSTA packet to bytes
    unsigned int internal_packet_len = packet.data.length;
    struct RastaByteArray internal_packet_bytes = rastaModuleToBytes(packet.data, hashing_context);

    // pack data into result
    for (unsigned int i = 0; i < internal_packet_len; ++i) {
        result.bytes[8+i] = internal_packet_bytes.bytes[i];
    }

    // initialize a byte array containing all data except checksum
    // this is needed, as otherwise the 0s/unset bytes in the result byte array will be used in calculating the checksum
    struct RastaByteArray temp_wo_checksum;
    unsigned int len_wo_checksum = (unsigned int)packet.length - (packet.checksum_type.width/8);
    allocateRastaByteArray(&temp_wo_checksum, len_wo_checksum);

    // copy data
    for (unsigned int j = 0; j < len_wo_checksum; ++j) {
        temp_wo_checksum.bytes[j] = result.bytes[j];
    }

    // generate checksum
    unsigned long checksum = crc_calculate(&packet.checksum_type, temp_wo_checksum);

    // pack checksum
    hostLongToLe(checksum, checksum_storage);
    for (int k = 0; k < (packet.checksum_type.width/8); ++k) {
        result.bytes[8+internal_packet_len+k] = checksum_storage[k];
    }

    // free the temporary checksum data
    freeRastaByteArray(&temp_wo_checksum);

    return result;
}

struct RastaRedundancyPacket bytesToRastaRedundancyPacket(struct RastaByteArray data, struct crc_options checksum_type, rasta_hashing_context_t * hashing_context){
    struct RastaRedundancyPacket packet;
    packet.checksum_type = checksum_type;

    //length
    packet.length = leShortToHost(&data.bytes[0]);

    // reserved bytes
    packet.reserve = leShortToHost(&data.bytes[2]);


    //sequence number
    packet.sequence_number = leLongToHost(&data.bytes[4]);

    // length of the carried data (the rasta packet) is total length - 8 bytes of length, reserve and seq nr before
    // and the in the checksum_type specified amount of bytes for the checksum after the data (divided by 8 because
    // the crc length is specified in bits)
    unsigned int data_len = data.length - 8 - (checksum_type.width/8);

    struct RastaByteArray internal_packet_bytes;
    allocateRastaByteArray(&internal_packet_bytes, data_len);

    // copy internal data bytes
    for (unsigned int i = 0; i < data_len; ++i) {
        internal_packet_bytes.bytes[i] = data.bytes[8+i];
    }

    // convert to rasta packet
    packet.data = bytesToRastaPacket(internal_packet_bytes, hashing_context);

    // free the packet bytes
    freeRastaByteArray(&internal_packet_bytes);

    // calculate the checksum of the received data
    struct RastaByteArray data_wo_checksum;
    unsigned  int data_wo_checksum_len = data.length - (checksum_type.width/8);
    allocateRastaByteArray(&data_wo_checksum, data_wo_checksum_len);

    // copy data
    for (unsigned int j = 0; j < data_wo_checksum_len; ++j) {
        data_wo_checksum.bytes[j] = data.bytes[j];
    }

    // calculate the actual checksum
    unsigned long calculated_checksum = crc_calculate(&checksum_type, data_wo_checksum);

    // free data
    freeRastaByteArray(&data_wo_checksum);

    // checksum check
    packet.checksum_correct = 1;

    if (data.length == data_wo_checksum_len){
        // no checksum was used, nothing to check
        return packet;
    }

    // convert the previously calculated checksum into byte array for comparison with data checksum
    unsigned char data_checksum[4];
    hostLongToLe(calculated_checksum, data_checksum);

    packet.checksum_correct = (rmemcmp(data_checksum, &data.bytes[8+data_len], (checksum_type.width / 8)) == 0);


    return packet;
}
