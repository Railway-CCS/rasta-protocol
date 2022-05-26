#include <scils_telegram_factory.h>
#include <sci_telegram_factory.h>
#include <rmemory.h>
#include <sci.h>
#include <string.h>

scils_signal_aspect * scils_signal_aspect_defaults(){
    scils_signal_aspect * signal_aspect = rmalloc(sizeof(scils_signal_aspect));

    signal_aspect->main = SCILS_MAIN_OFF;
    signal_aspect->additional = SCILS_ADDITIONAL_OFF;
    signal_aspect->zs3 = SCILS_ZS3_OFF;
    signal_aspect->zs3v = SCILS_ZS3_OFF;
    signal_aspect->zs2 = SCILS_ZS2_OFF;
    signal_aspect->zs2v = SCILS_ZS2_OFF;
    signal_aspect->deprecation_information = SCILS_DEPRECIATION_INFORMATION_NO_INFORMATION;
    signal_aspect->upstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION;
    signal_aspect->downstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION;
    signal_aspect->dark_switching = SCILS_DARK_SWITCHING_SHOW;

    return signal_aspect;
}

sci_telegram * scils_create_show_signal_aspect(char * sender, char * receiver, scils_signal_aspect signal_aspect){
    sci_telegram * telegram = sci_create_base_telegram(SCI_PROTOCOL_LS, sender, receiver,
                                                       SCILS_MESSAGE_TYPE_SHOW_SIGNAL_ASPECT);

    telegram->payload.used_bytes = 9;
    telegram->payload.data[0] = signal_aspect.main;
    telegram->payload.data[1] = signal_aspect.additional;
    telegram->payload.data[2] = signal_aspect.zs3;
    telegram->payload.data[3] = signal_aspect.zs3v;
    telegram->payload.data[4] = signal_aspect.zs2;
    telegram->payload.data[5] = signal_aspect.zs2v;
    telegram->payload.data[6] = signal_aspect.deprecation_information;

    unsigned char driveway_info = (unsigned char)(signal_aspect.downstream_driveway_information << 4);
    driveway_info |= signal_aspect.upstream_driveway_information;

    telegram->payload.data[7] = driveway_info;
    telegram->payload.data[8] = signal_aspect.dark_switching;

    return telegram;
}

sci_telegram * scils_create_change_brightness(char * sender, char * receiver, scils_brightness brightness){
    sci_telegram * telegram = sci_create_base_telegram(SCI_PROTOCOL_LS, sender, receiver,
                                                       SCILS_MESSAGE_TYPE_CHANGE_BRIGHTNESS);

    telegram->payload.used_bytes = 1;
    telegram->payload.data[0] = brightness;

    return telegram;
}

sci_telegram * scils_create_signal_aspect_status(char * sender, char * receiver, scils_signal_aspect signal_aspect){
    sci_telegram * telegram = sci_create_base_telegram(SCI_PROTOCOL_LS, sender, receiver,
                                                       SCILS_MESSAGE_TYPE_SIGNAL_ASPECT_STATUS);

    telegram->payload.used_bytes = 18;
    telegram->payload.data[0] = signal_aspect.main;
    telegram->payload.data[1] = signal_aspect.additional;
    telegram->payload.data[2] = signal_aspect.zs3;
    telegram->payload.data[3] = signal_aspect.zs3v;
    telegram->payload.data[4] = signal_aspect.zs2;
    telegram->payload.data[5] = signal_aspect.zs2v;
    telegram->payload.data[6] = signal_aspect.deprecation_information;
    // route information not applicable
    telegram->payload.data[7] = signal_aspect.route_information;
    telegram->payload.data[8] = signal_aspect.dark_switching;
    memcpy(&telegram->payload.data[9],signal_aspect.nationally_specified_information,9);

    return telegram;
}

sci_telegram * scils_create_brightness_status(char * sender, char * receiver, scils_brightness brightness){
    sci_telegram * telegram = sci_create_base_telegram(SCI_PROTOCOL_LS, sender, receiver,
                                                       SCILS_MESSAGE_TYPE_SIGNAL_BRIGHTNESS_STATUS);

    telegram->payload.used_bytes = 1;
    telegram->payload.data[0] = brightness;

    return telegram;
}

sci_parse_result scils_parse_show_signal_aspect_payload(sci_telegram * telegram, scils_signal_aspect * signal_aspect){
    if(sci_get_message_type(telegram) != SCILS_MESSAGE_TYPE_SHOW_SIGNAL_ASPECT){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }

    signal_aspect->main = (scils_main)telegram->payload.data[0];
    signal_aspect->additional= (scils_additional)telegram->payload.data[1];
    signal_aspect->zs3= (scils_zs3)telegram->payload.data[2];
    signal_aspect->zs3v= (scils_zs3)telegram->payload.data[3];
    signal_aspect->zs2= (scils_zs2)telegram->payload.data[4];
    signal_aspect->zs2v= (scils_zs2)telegram->payload.data[5];
    signal_aspect->deprecation_information= (scils_deprecation_information)telegram->payload.data[6];

    unsigned char driveway = telegram->payload.data[7];
    signal_aspect->upstream_driveway_information = (scils_driveway_information)(unsigned char)((driveway & 0x0F));
    signal_aspect->downstream_driveway_information = (scils_driveway_information)(unsigned char)((driveway & 0xF0) >> 4);

    signal_aspect->dark_switching = (scils_dark_switching)telegram->payload.data[8];

    return SCI_PARSE_SUCCESS;
}

sci_parse_result scils_parse_signal_aspect_status_payload(sci_telegram * telegram, scils_signal_aspect * signal_aspect){
    if(sci_get_message_type(telegram) != SCILS_MESSAGE_TYPE_SIGNAL_ASPECT_STATUS){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }

    signal_aspect->main = (scils_main)telegram->payload.data[0];
    signal_aspect->additional= (scils_additional)telegram->payload.data[1];
    signal_aspect->zs3= (scils_zs3)telegram->payload.data[2];
    signal_aspect->zs3v= (scils_zs3)telegram->payload.data[3];
    signal_aspect->zs2= (scils_zs2)telegram->payload.data[4];
    signal_aspect->zs2v= (scils_zs2)telegram->payload.data[5];
    signal_aspect->deprecation_information = (scils_deprecation_information)telegram->payload.data[6];
    signal_aspect->route_information = (scils_route_information)telegram->payload.data[7];
    signal_aspect->dark_switching = (scils_dark_switching)telegram->payload.data[8];
    memcpy(signal_aspect->nationally_specified_information,&telegram->payload.data[9],9);

    return SCI_PARSE_SUCCESS;
}

sci_parse_result scils_parse_change_brightness_payload(sci_telegram * telegram, scils_brightness * brightness){
    if(sci_get_message_type(telegram) != SCILS_MESSAGE_TYPE_CHANGE_BRIGHTNESS){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }

    *brightness = (scils_brightness)telegram->payload.data[0];

    return SCI_PARSE_SUCCESS;
}

sci_parse_result scils_parse_brightness_status_payload(sci_telegram * telegram, scils_brightness * brightness){
    if(sci_get_message_type(telegram) != SCILS_MESSAGE_TYPE_SIGNAL_BRIGHTNESS_STATUS){
        return SCI_PARSE_INVALID_MESSAGE_TYPE;
    }

    *brightness = (scils_brightness)telegram->payload.data[0];

    return SCI_PARSE_SUCCESS;
}