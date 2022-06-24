//
// Created by tobia on 27.11.2017.
//

#ifndef LST_SIMULATOR_RASTAMODULE_H
#define LST_SIMULATOR_RASTAMODULE_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "rastautil.h"
#include "rastacrc.h"
#include <stdint.h>
#include <rastahashing.h>

#define RASTA_CHECKSUM_VALID 1
#define RASTA_CHECKSUM_INVALID 0

typedef enum {
    /**
     * no errors
     */
    RASTA_ERRORS_NONE = 0,
    /**
     * the data field of the rasta packet is corrupted
     */
    RASTA_ERRORS_WRONG_PACKAGE_FORMAT = 1,
    /**
     * package length is set wrong
     */
    RASTA_ERRORS_PACKAGE_LENGTH_INVALID = 2
}rasta_error_type;

/**
 * type of a rasta packet
 */
typedef enum {
    /**
     * Connectionrequest
     */
    RASTA_TYPE_CONNREQ = 6200,
    /**
     *  ConnectionResponse
     */
    RASTA_TYPE_CONNRESP =6201,
    /**
     * Retransmissionrequest
     */
    RASTA_TYPE_RETRREQ = 6212,
    /**
     * Retransmissionresponse
     */
    RASTA_TYPE_RETRRESP = 6213,
    /**
     * Disconnectionrequest
     */
    RASTA_TYPE_DISCREQ = 6216,
    /**
     * Heartbeat
     */
    RASTA_TYPE_HB = 6220,
    /**
     * Datamessage
     */
    RASTA_TYPE_DATA = 6240,
    /**
     * Retransmitted Datamessage
     */
    RASTA_TYPE_RETRDATA = 6241
}rasta_conn_type;

/**
 * returns 1 if the machine is BigEndian and 0 else
 * @return
 */
int isBigEndian();

/**
 * sets the checksumtype global
 * @param type choose between 8B, 16B or no checksum
 * @param a a part of starting vector
 * @param b b part of starting vector
 * @param c c part of starting vector
 * @param d d part of starting vector
 */
//void setMD4checksum(rasta_checksum_type type, MD4_u32plus a, MD4_u32plus b, MD4_u32plus c, MD4_u32plus d);


/**
 * function to return the stored md4 type
 * @return
 */
//rasta_checksum_type getMD4checksumType();
/**
 * returns the last error from a previously called rasta function
 * note: calling this function will reset the errors to none
 * @return the error
 */
rasta_error_type getRastamoduleLastError();

/**
 * Converts a unsigned short into a uchar array
 * @param v the ushort
 * @param result the assigned uchar array; length should be 2
 */
void hostShortTole(uint16_t v, unsigned char* result);

/**
 * Converts a uchar pointer to a ushort in host byte order
 * @param v pointer to 2 bytes in little-endian byte order
 * @return the ushort
 */
uint16_t leShortToHost(const unsigned char* v);

/**
 * Converts a unsigned long into a uchar array
 * @param v the uchar array
 * @param result the assigned uchar array; length should be 4
 */
void hostLongToLe(uint32_t v, unsigned char* result);

/**
 * Converts a uchar array to a ulong
 * @param v the uchar array
 * @return the ulong
 */
uint32_t leLongToHost(const unsigned char v[4]);


/**
 * Struct representing a generic rasta packet
 */
struct RastaPacket {
    /**
     * the length of the packet
     * NOTE: this field should never be set manually. Use the functions in rastafactory to create RastaPackets
     */

    unsigned short length;
    /**
     *  the package type
     */
    rasta_conn_type type;

    uint32_t receiver_id;
    uint32_t sender_id;

    uint32_t sequence_number;
    uint32_t confirmed_sequence_number;

    //type could change later
    uint32_t timestamp;
    uint32_t confirmed_timestamp;

    struct RastaByteArray data;
    struct RastaByteArray checksum;

    //1 if the checksum is correct, 0 if it's not
    //NOTE: this field is only set, if you use the "bytestoRastaPacket" function
    int checksum_correct;
};

/**
 * representation of a RaSTA packet in the Redundancy layer as in 6.3.1
 */
struct RastaRedundancyPacket {
    /**
     * the overall length of the redundancy packet
     */
    uint16_t length;

    /**
     * reserved bytes for future use
     */
    uint16_t reserve;

    /**
     * pdu sequence number
     */
    uint32_t sequence_number;

    /**
     * the carried data
     */
    struct RastaPacket data;

    /**
     * 1 if the CRC checksum correct, 0 otherwise
     * Will be 1 of no checksum was used
     */
    int checksum_correct;

    /**
     * the parameters of the checksum that is used
     */
    struct crc_options checksum_type;
};

/**
 * Accepts a rasta packet and converts it into an allocated bytearray
 * @param packet the packet
 * @return the bytearray
 */
struct RastaByteArray rastaModuleToBytes(struct RastaPacket packet, rasta_hashing_context_t * hashing_context);

/**
 * Accepts a rasta packet and converts it into an allocated bytearray without calculating the safety code
 * @param packet the packet
 * @return the bytearray
 */
struct RastaByteArray rastaModuleToBytesNoChecksum(struct RastaPacket packet, rasta_hashing_context_t * hashing_context);

/**
 * Accepts a byte array and converts it into a rasta packet while checking the md4 checksum
 * @param data the data
 * @return if length = 0, the data packet was to short. If checksum_correct=0, the packed should be discarded
 */
struct RastaPacket bytesToRastaPacket(struct RastaByteArray data, rasta_hashing_context_t * hashing_context);


/**
 * Accepts a RaSTA redundancy layer packet and converts it into a byte array
 * @param packet the redundancy layer packet that will be converted
 * @param hashing_context the hashing parameters that are used for the SR layer hash
 * @return an already allocated RastaByteArray (no need to free it yourself)
 */
struct RastaByteArray rastaRedundancyPacketToBytes(struct RastaRedundancyPacket packet, rasta_hashing_context_t * hashing_context);

/**
 * Accepts a byte array and converts it into a RaSTA redundancy layer packet
 * This function will check whether the CRC checksum is correct and set the flag RastaRedundancyPacket#checksum_correct
 * @param data the byte array which contains the packet
 * @param checksum_type the options that were used to generate the checksum in the @p data byte array
 * @param hashing_context the hashing parameters that are used for the SR layer hash
 * @return a RaSTA Redundancy layer packet containing all data that was in the @p data byte array
 */
struct RastaRedundancyPacket bytesToRastaRedundancyPacket(struct RastaByteArray data, struct crc_options checksum_type, rasta_hashing_context_t * hashing_context);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTAMODULE_H
