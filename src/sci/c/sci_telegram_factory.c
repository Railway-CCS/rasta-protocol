#include <sci_telegram_factory.h>
#include <rmemory.h>
#include <sci.h>

sci_telegram * sci_create_base_telegram(protocol_type protocolType, char * sender, char * receiver, unsigned short message_type){
    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));

    telegram->protocol_type = protocolType;
    sci_set_message_type(telegram, message_type);
    sci_set_sender(telegram, sender);
    sci_set_receiver(telegram, receiver);

    rmemset(telegram->payload.data, 0x00, 85);
    telegram->payload.used_bytes = 0;

    return telegram;
}

sci_telegram * sci_create_version_request(protocol_type protocolType, char * sender, char * receiver, unsigned char version){
    sci_telegram * telegram = sci_create_base_telegram(protocolType, sender, receiver, SCI_MESSAGE_TYPE_VERSION_REQUEST);


    telegram->payload.data[0] = version;
    telegram->payload.used_bytes = 1;

    return telegram;
}

sci_telegram * sci_create_version_response(protocol_type protocolType, char * sender, char * receiver,
                                           unsigned char version, sci_version_check_result version_check_result,
                                           unsigned char checksum_len, unsigned char * checksum){
    sci_telegram * telegram = sci_create_base_telegram(protocolType, sender, receiver, SCI_MESSAGE_TYPE_VERSION_RESPONSE);

    telegram->payload.data[0] = version_check_result;
    telegram->payload.data[1] = version;
    telegram->payload.data[2] = checksum_len;

    rmemcpy(&telegram->payload.data[3], checksum, checksum_len);

    telegram->payload.used_bytes = checksum_len + 3;

    return telegram;
}

sci_telegram * sci_create_status_request(protocol_type protocolType, char * sender, char * receiver){
    return sci_create_base_telegram(protocolType, sender, receiver, SCI_MESSAGE_TYPE_STATUS_REQUEST);
}

sci_telegram * sci_create_status_begin(protocol_type protocolType, char * sender, char * receiver){
    return sci_create_base_telegram(protocolType, sender, receiver, SCI_MESSAGE_TYPE_STATUS_BEGIN);
}

sci_telegram * sci_create_status_finish(protocol_type protocolType, char * sender, char * receiver){
    return sci_create_base_telegram(protocolType, sender, receiver, SCI_MESSAGE_TYPE_STATUS_FINISH);
}

sci_parse_result sci_parse_version_request_payload(sci_telegram * version_request, unsigned char * estw_version){
    if(sci_get_message_type(version_request) != SCI_MESSAGE_TYPE_VERSION_REQUEST){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }

    *estw_version = version_request->payload.data[0];

    return SCI_PARSE_SUCCESS;
}

sci_parse_result sci_parse_version_response_payload(sci_telegram * version_response, unsigned char * btp_version, sci_version_check_result * result,
                                                    unsigned char * checksum_len, unsigned char * checksum){
    if(sci_get_message_type(version_response) != SCI_MESSAGE_TYPE_VERSION_RESPONSE){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }
    if(version_response->payload.used_bytes < 3 || version_response->payload.used_bytes > 128){
        return SCI_PARSE_INVALID_PAYLOAD_LENGTH;
    }

    *result = (sci_version_check_result)version_response->payload.data[0];
    *btp_version = version_response->payload.data[1];
    *checksum_len = version_response->payload.data[2];

    if(*checksum_len > (version_response->payload.used_bytes - 3)){
        return SCI_PARSE_INVALID_PAYLOAD_LENGTH;
    }
    // checksum might be empty
    if(*checksum_len) {
        rmemcpy(checksum, &version_response->payload.data[3], *checksum_len);
    }

    return SCI_PARSE_SUCCESS;
}