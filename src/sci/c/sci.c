#include <sci.h>
#include <memory.h>
#include <rmemory.h>
#include <rastafactory.h>

void sci_set_sender(sci_telegram * telegram, char * sender_name){
    size_t name_len = strlen(sender_name);
    // we will set the sender field in the telegram to all underscores and just copy the name_len bytes to the beginning
    rmemset(telegram->sender, SCI_NAME_PADDING_CHAR, SCI_NAME_LENGTH);
    rmemcpy(telegram->sender, sender_name, (unsigned int)name_len);
}

void sci_set_receiver(sci_telegram * telegram, char * receiver_name){
    size_t name_len = strlen(receiver_name);
    // we will set the receiver field in the telegram to all underscores and just copy the name_len bytes to the beginning
    rmemset(telegram->receiver, SCI_NAME_PADDING_CHAR, SCI_NAME_LENGTH);
    rmemcpy(telegram->receiver, receiver_name, (unsigned int)name_len);
}

char * sci_get_name_string(char * name_field){
    char * name_str = rmalloc(SCI_NAME_LENGTH + 1);
    rmemset(name_str, 0x00, SCI_NAME_LENGTH + 1);
    rmemcpy(name_str, name_field, SCI_NAME_LENGTH);

    return name_str;
}

void sci_set_message_type(sci_telegram * telegram, unsigned short message_type){
    unsigned char tmp[2];
    shortToBytes(message_type, tmp);
    rmemcpy(telegram->message_type, tmp, 2);
}

struct RastaByteArray sci_encode_telegram(sci_telegram * telegram){
    struct RastaByteArray encoded_telegram;
    allocateRastaByteArray(&encoded_telegram, SCI_TELEGRAM_LENGTH_WITHOUT_PAYLOAD + telegram->payload.used_bytes);

    // pack the SCI protocol type
    encoded_telegram.bytes[0] = telegram->protocol_type;

    // pack the message type
    rmemcpy(&encoded_telegram.bytes[1], telegram->message_type, 2);

    // pack the sender
    rmemcpy(&encoded_telegram.bytes[3], telegram->sender, SCI_NAME_LENGTH);

    // pack the receiver
    rmemcpy(&encoded_telegram.bytes[23], telegram->receiver, SCI_NAME_LENGTH);

    if (telegram->payload.used_bytes > 0){
        // pack the payload
        rmemcpy(&encoded_telegram.bytes[43], telegram->payload.data, telegram->payload.used_bytes);
    }

    return encoded_telegram;
}

sci_telegram * sci_decode_telegram(struct RastaByteArray data){
    if(data.length > SCI_MAX_TELEGRAM_LENGTH || data.length < SCI_TELEGRAM_LENGTH_WITHOUT_PAYLOAD){
        // size does not match
        return NULL;
    }

    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));


    // check if a valid protocol was provided
    if (!(data.bytes[0] == SCI_PROTOCOL_P || data.bytes[0] == SCI_PROTOCOL_LS)){
        // invalid protocol
        rfree(telegram);
        return NULL;
    }

    // copy the basic data into the telegram
    telegram->protocol_type = data.bytes[0];
    rmemcpy(telegram->message_type, &data.bytes[1], 2);
    rmemcpy(telegram->sender, &data.bytes[3], SCI_NAME_LENGTH);
    rmemcpy(telegram->receiver, &data.bytes[23], SCI_NAME_LENGTH);

    // copy the payload
    unsigned int payload_size = data.length - SCI_TELEGRAM_LENGTH_WITHOUT_PAYLOAD;
    if (payload_size > 0){
        // the telegram contains a payload
        sci_payload payload;
        payload.used_bytes = payload_size;
        rmemcpy(payload.data, &data.bytes[43], payload_size);

        telegram->payload = payload;
    }

    return telegram;
}

unsigned short sci_get_message_type(sci_telegram * telegram){
    return bytesToShort(telegram->message_type);
}
