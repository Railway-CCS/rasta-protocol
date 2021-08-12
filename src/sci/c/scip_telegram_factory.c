#include <scip_telegram_factory.h>

sci_telegram * scip_create_change_location_telegram(char *sender, char *receiver, scip_point_target_location location){
    sci_telegram * telegram = sci_create_base_telegram(SCI_PROTOCOL_P, sender, receiver, SCIP_MESSAGE_TYPE_CHANGE_LOCATION);

    telegram->payload.data[0] = location;
    telegram->payload.used_bytes = 1;

    return telegram;
}

sci_telegram * scip_create_location_status_telegram(char *sender, char *receiver, scip_point_location location){
    sci_telegram * telegram = sci_create_base_telegram(SCI_PROTOCOL_P, sender, receiver, SCIP_MESSAGE_TYPE_LOCATION_STATUS);

    telegram->payload.data[0] = location;
    telegram->payload.used_bytes = 1;

    return telegram;
}

sci_telegram * scip_create_timeout_telegram(char *sender, char *receiver){
    return sci_create_base_telegram(SCI_PROTOCOL_P, sender, receiver, SCIP_MESSAGE_TYPE_TIMEOUT);
}

sci_parse_result scip_parse_change_location_payload(sci_telegram * telegram, scip_point_target_location * location){
    if(sci_get_message_type(telegram) != SCIP_MESSAGE_TYPE_CHANGE_LOCATION){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }

    *location = (scip_point_target_location)telegram->payload.data[0];

    return SCI_PARSE_SUCCESS;
}

sci_parse_result scip_parse_location_status_payload(sci_telegram * telegram, scip_point_location * location){
    if(sci_get_message_type(telegram) != SCIP_MESSAGE_TYPE_LOCATION_STATUS){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }

    *location = (scip_point_location)telegram->payload.data[0];

    return SCI_PARSE_SUCCESS;
}