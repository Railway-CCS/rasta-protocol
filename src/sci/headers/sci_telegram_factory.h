#ifndef LST_SIMULATOR_SCI_TELEGRAM_FACTORY_H
#define LST_SIMULATOR_SCI_TELEGRAM_FACTORY_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <sci.h>

/**
 * Message type of a SCI version request
 */
#define SCI_MESSAGE_TYPE_VERSION_REQUEST 0x0024
/**
 * Message type of a SCI version response
 */
#define SCI_MESSAGE_TYPE_VERSION_RESPONSE 0x0025
/**
 * Message type of a SCI status request
 */
#define SCI_MESSAGE_TYPE_STATUS_REQUEST 0x0021
/**
 * Message type of a SCI status begin message
 */
#define SCI_MESSAGE_TYPE_STATUS_BEGIN 0x0022
/**
 * Message type of a SCI status finish message
 */
#define SCI_MESSAGE_TYPE_STATUS_FINISH 0x0023

/**
 * Return codes for functions that parse telegram payloads
 */
typedef enum {
    SCI_PARSE_INVALID_PAYLOAD_LENGTH = 0x01,
    SCI_PARSE_INVALID_MESSAGE_TYPE = 0x02,
    SCI_PARSE_SUCCESS = 0x00
}sci_parse_result;

/**
 * Creates a SCI version request telegram.
 * @param protocolType The SCI protocol
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param version the ESTW version
 * @return a version request telegram
 */
sci_telegram * sci_create_version_request(protocol_type protocolType, char * sender, char * receiver, unsigned char version);

/**
 * Creates a SCI version response telegram.
 * @param protocolType The SCI protocol
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param version the BTP version
 * @param version_check_result the result of the version check
 * @param checksum_len the length of the checksum data
 * @param checksum the checksum data
 * @return a version response telegram
 */
sci_telegram * sci_create_version_response(protocol_type protocolType, char * sender, char * receiver,
                                           unsigned char version, sci_version_check_result version_check_result,
                                           unsigned char checksum_len, unsigned char * checksum);

/**
 * Creates a SCI status request.
 * @param protocolType The SCI protocol
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @return a status request telegram
 */
sci_telegram * sci_create_status_request(protocol_type protocolType, char * sender, char * receiver);

/**
 * Creates a SCI status begin telegram.
 * @param protocolType the SCI protocol
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @return a status begin telegram
 */
sci_telegram * sci_create_status_begin(protocol_type protocolType, char * sender, char * receiver);

/**
 * Creates a SCI status finish telegram.
 * @param protocolType the SCI protocol
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @return a status finish telegram
 */
sci_telegram * sci_create_status_finish(protocol_type protocolType, char * sender, char * receiver);

/**
 * Creates a base telegram, sets the payload to all 0s
 * @param protocolType the SCI protocol
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param message_type the message type of the telegram
 * @return a basic SCI telegram
 */
sci_telegram * sci_create_base_telegram(protocol_type protocolType, char * sender, char * receiver, unsigned short message_type);

/**
 * Tries to parse the payload of a version request telegram. The values will be written to the corresponding pointers.
 * @param version_request the version request telegram
 * @param estw_version value: the version of the ESTW
 * @return 0 if success, error code otherwise
 */
sci_parse_result sci_parse_version_request_payload(sci_telegram * version_request, unsigned char * estw_version);

/**
 * Tries to parse the payload of a version response telegram. The values will be written to the corresponding pointers.
 * @param version_response the version response telegram
 * @param btp_version value: BTP version
 * @param result value: version check result
 * @param checksum_len value: checksum length
 * @param checksum value: checksum data with length checksum_len
 * @return 0 if success, error code otherwise
 */
sci_parse_result sci_parse_version_response_payload(sci_telegram * version_response, unsigned char * btp_version, sci_version_check_result * result,
                                       unsigned char * checksum_len, unsigned char * checksum);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_SCI_TELEGRAM_FACTORY_H
