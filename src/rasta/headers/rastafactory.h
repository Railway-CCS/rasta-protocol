//
// Created by tobia on 29.11.2017.
//

#ifndef LST_SIMULATOR_RASTAFACTORY_H
#define LST_SIMULATOR_RASTAFACTORY_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "rastamodule.h"
#include <stdint.h>

/**
 * generic struct for the additional data for Connectionrequest and Connectionresponse
 */
struct RastaConnectionData{
    unsigned short send_max;
    char version[4];
};

/**
 * generic struct for the additional data for DisconnectionRequest
 */
struct RastaDisconnectionData {
    unsigned short details;
    unsigned short reason;
};

/**
 * generic struct for the additional data for Datamessage and Retransmitted Datamessage
 */
struct RastaMessageData {
    unsigned int count;
    struct RastaByteArray* data_array;
};

/**
 * returns the last error from a previously called rasta function
 * note: calling this function will reset the errors to none
 * @return the error
 */
rasta_error_type getRastafactoryLastError();

/**
 * Allocates the memory for the rasta message data array. Note: all entrys need to be allocated seperatly (see allocateRastaByteArray in rastamodule.h)
 * @param data
 * @param count
 */
void allocateRastaMessageData(struct RastaMessageData* data, unsigned int count);

/**
 * Frees the MessageData struct and all entries
 * @param data
 */
void freeRastaMessageData(struct RastaMessageData* data);

/**
 * Creates aa connection request package
 * @param receiver_id
 * @param sender_id
 * @param initial_sequence_number
 * @param timestamp
 * @param send_max
 * @param version The protocol version. Should be set to "0303"
 * @return
 */
struct RastaPacket createConnectionRequest(uint32_t receiver_id, uint32_t sender_id, uint32_t initial_sequence_number,
                                           uint32_t timestamp, uint16_t send_max,
                                           const unsigned char version[4], rasta_hashing_context_t * hashing_context);

/**
 * Creates a connection response package
 * @param receiver_id
 * @param sender_id
 * @param initial_sequence_number
 * @param confirmed_sequence_number
 * @param timestamp
 * @param confirmed_timestamp
 * @param send_max
 * @param version The protocol version. Should be set to "0303"
 * @return
 */
struct RastaPacket createConnectionResponse(uint32_t receiver_id, uint32_t sender_id, uint32_t initial_sequence_number, uint32_t confirmed_sequence_number,
                                            uint32_t timestamp, uint32_t confirmed_timestamp, uint16_t send_max,
                                            const unsigned char version[4], rasta_hashing_context_t * hashing_context);

/**
 * Extracts the extra data for connectionrequests(6200) and connectionresponse(6201)
 * @param p
 * @return
 */
struct RastaConnectionData extractRastaConnectionData(struct RastaPacket p);

/**
 * creates a retransmission request package
 * @param receiver_id
 * @param sender_id
 * @param sequence_number
 * @param confirmed_sequence_number
 * @param timestamp
 * @param confirmed_timestamp
 * @return
 */
struct RastaPacket createRetransmissionRequest(uint32_t receiver_id, uint32_t sender_id, uint32_t sequence_number, uint32_t confirmed_sequence_number,
                                               uint32_t timestamp, uint32_t confirmed_timestamp, rasta_hashing_context_t * hashing_context);

/**
 * creates a retransmission response package
 * @param receiver_id
 * @param sender_id
 * @param sequence_number
 * @param confirmed_sequence_number
 * @param timestamp
 * @param confirmed_timestamp
 * @return
 */
struct RastaPacket createRetransmissionResponse(uint32_t receiver_id, uint32_t sender_id, uint32_t sequence_number, uint32_t confirmed_sequence_number,
                                                uint32_t timestamp, uint32_t confirmed_timestamp, rasta_hashing_context_t * hashing_context);

/**
 * creates a disconnection request
 * @param receiver_id
 * @param sender_id
 * @param sequence_number
 * @param confirmed_sequence_number
 * @param timestamp
 * @param confirmed_timestamp
 * @param data defined in 5.4.6
 * @return
 */
struct RastaPacket createDisconnectionRequest(uint32_t receiver_id, uint32_t sender_id, uint32_t sequence_number, uint32_t confirmed_sequence_number,
                                              uint32_t timestamp, uint32_t confirmed_timestamp, struct RastaDisconnectionData data, rasta_hashing_context_t * hashing_context);

/**
 * Extracts the extra data for disconnectionrequest(6216)
 * @param p
 * @return
 */
struct RastaDisconnectionData extractRastaDisconnectionData(struct RastaPacket p);

/**
 * creates a heartbeat package
 * @param receiver_id
 * @param sender_id
 * @param sequence_number
 * @param confirmed_sequence_number
 * @param timestamp
 * @param confirmed_timestamp
 * @return
 */
struct RastaPacket createHeartbeat(uint32_t receiver_id, uint32_t sender_id, uint32_t sequence_number, uint32_t confirmed_sequence_number,
                                   uint32_t timestamp, uint32_t confirmed_timestamp, rasta_hashing_context_t * hashing_context);

/**
 * creates a data message packet
 * @param receiver_id
 * @param sender_id
 * @param sequence_number
 * @param confirmed_sequence_number
 * @param timestamp
 * @param confirmed_timestamp
 * @param data
 * @return
 */
struct RastaPacket createDataMessage(uint32_t receiver_id, uint32_t sender_id, uint32_t sequence_number, uint32_t confirmed_sequence_number,
                                     uint32_t timestamp, uint32_t confirmed_timestamp, struct RastaMessageData data, rasta_hashing_context_t * hashing_context);

/**
 * creates a retransmitted data message packet
 * @param receiver_id
 * @param sender_id
 * @param sequence_number
 * @param confirmed_sequence_number
 * @param timestamp
 * @param confirmed_timestamp
 * @param data
 * @return
 */
struct RastaPacket createRetransmittedDataMessage(uint32_t receiver_id, uint32_t sender_id, uint32_t sequence_number, uint32_t confirmed_sequence_number,
                                                  uint32_t timestamp, uint32_t confirmed_timestamp, struct RastaMessageData data, rasta_hashing_context_t * hashing_context);

/**
 * extracts the additional data for data message and retransmitted data message
 * @param p
 * @return
 */
struct RastaMessageData extractMessageData(struct RastaPacket p);

/**
 * creates a redundancy PDU carrying the specified @p inner_data
 * @param sequence_number the sequence number of the PDU
 * @param inner_data the SR-layer packet that is contained in the PDU
 * @param checksum_type the options for the CRC algorithm that will be used to calculate the checksum
 * @return a RaSTA redundancy layer PDU
 */
struct RastaRedundancyPacket createRedundancyPacket(uint32_t sequence_number, struct RastaPacket inner_data, struct crc_options checksum_type);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTAFACTORY_H
