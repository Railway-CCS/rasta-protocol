#ifndef LST_SIMULATOR_SCI_H
#define LST_SIMULATOR_SCI_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <rastautil.h>
/**
 * Maximum length of a SCI telegram in bytes
 */

#define SCI_MAX_TELEGRAM_LENGTH 128

/**
 * Maximum length of a SCI telegram without the payload
 */
#define SCI_TELEGRAM_LENGTH_WITHOUT_PAYLOAD 43

/**
 * The ASCII character that is used for padding the SCI names. This is an underscore.
 */
#define SCI_NAME_PADDING_CHAR 0x5F

/**
 * Length of the SCI names including padding in bytes.
 */
#define SCI_NAME_LENGTH 20

/**
 * Current version of the SCI implementation
 */
#define SCI_VERSION 0x01

typedef struct {
    /**
     * The payload data. Not all 85 bytes might be used. see used_bytes for the amount of used bytes.
     */
    unsigned char data[85];

    /**
     * The amount of bytes in the data that are used
     */
    unsigned int used_bytes;
}sci_payload;

typedef enum {
    /**
     * Protocol type byte for the SCI-P protocol
     */
    SCI_PROTOCOL_P = 0x40,

    /**
     * Protocol type byte for the SCI-LS protocol
     */
    SCI_PROTOCOL_LS = 0x30
}protocol_type;

typedef enum {
    UNKNOWN_SCI_NAME = 0x01,
    INVALID_SCI_NAME = 0x02,
    SUCCESS = 0x00
}sci_return_code;

typedef struct {
    /**
     * The identifier of the SCI protocol, for example 0x40 for SCI-P.
     */
    protocol_type protocol_type;
    /**
     * The message type.
     */
    unsigned char message_type[2];
    /**
     * The SCI name of the telegrams origin. Note that the ID is padded with underscores (0x5F)
     */
    char sender [20];
    /**
     * The SCI name of the telegrams receiver. Note that the ID is padded with underscores (0x5F)
     */
    char receiver [20];
    /**
     * The payload of the telegram.
     */
    sci_payload payload;
}sci_telegram;

/**
 * Enumeration with the allowed results for a BTP version check
 */
typedef enum {
    SCI_VERSION_CHECK_RESULT_NOT_ALLOWED_TO_USE = 0x00,
    SCI_VERSION_CHECK_RESULT_VERSIONS_ARE_NOT_EQUAL = 0x01,
    SCI_VERSION_CHECK_RESULT_VERSIONS_ARE_EQUAL = 0x02
} sci_version_check_result;

/**
 * Sets the sender ID in the telegram and performs padding.
 * @param telegram the telegram whose sender is set
 * @param sender_name the name of the sender entity without padding
 */
void sci_set_sender(sci_telegram * telegram, char * sender_name);
/**
 * Sets the receiver ID in the telegram and performs padding.
 * @param telegram the telegram whose receiver is set
 * @param receiver_name the name of the receiver entity without padding
 */
void sci_set_receiver(sci_telegram * telegram, char * receiver_name);

/**
 * Sets the message type in the telegram. Note that the byte order is reversed.
 * @param telegram the telegram whose message type is set
 * @param message_type the message type of the telegram
 */
void sci_set_message_type(sci_telegram * telegram, unsigned short message_type);

/**
 * Gets the message type of the telegram
 * @param telegram the telegram
 * @return the message type as ushort
 */
unsigned short sci_get_message_type(sci_telegram * telegram);

/**
 * Converts the sender or receiver name into a string, i.e. makes it null-terminated.
 * It will allocate memory for the string, so you need to free it after usage.
 * @param name_field the field to be converted (sender or receiver of a telegram)
 * @return a null-terminated string which contains the SCI name
 */
char * sci_get_name_string(char * name_field);

/**
 * Encodes the given telegram into an byte array.
 * @param telegram the telegram that will be encoded
 * @return the byte array that represent the SCI telegram
 */
struct RastaByteArray sci_encode_telegram(sci_telegram * telegram);

/**
 * Tries to decode a SCI telegram from a byte array. If the given byte array cannot be parsed, i.e. it is not a valid
 * SCI telegram, NULL will be returned.
 * @param data the byte array that will be parsed
 * @return the parsed telegram, or if the byte array does not contain a valid telegram NULL
 */
sci_telegram * sci_decode_telegram(struct RastaByteArray data);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_SCI_H
