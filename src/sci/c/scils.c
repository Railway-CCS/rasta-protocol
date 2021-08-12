#include <scils.h>
#include <rmemory.h>
#include <memory.h>
#include <rastafactory.h>
#include <rasta_new.h>
#include <sci.h>

/**
 * Tries to send a SCI telegram to the receiver using the underlying RaSTA instance
 * @param ls the SCI-LS instance
 * @param telegram the telegram to send
 * @return 0 if success, error code otherwise
 */
sci_return_code scils_send_telegram(scils_t * ls, sci_telegram * telegram){
    struct RastaByteArray data = sci_encode_telegram(telegram);

    char * sci_name = sci_get_name_string(telegram->receiver);

    unsigned long rastaId;
    int res = hashmap_get(ls->sciNamesToRastaIds, sci_name, (void **) &rastaId);
    rfree(sci_name);

    if (res == MAP_MISSING){
        // SCI name not in map
        return UNKNOWN_SCI_NAME;
    }

    struct RastaMessageData messageData;
    allocateRastaMessageData(&messageData, 1);
    messageData.data_array[0] = data;

    sr_send(ls->rasta_handle, rastaId, messageData);

    freeRastaMessageData(&messageData);
    return SUCCESS;
}

scils_t * scils_init(struct rasta_handle * handle, char * sciName){
    scils_t * scils = rmalloc(sizeof(scils_t));

    scils->rasta_handle = handle;
    scils->sciName = rmalloc((unsigned int)strlen(sciName));
    strcpy(scils->sciName, sciName);

    // initialize map
    scils->sciNamesToRastaIds = hashmap_new();

    // initialize notifications to NULL
    scils->notifications.on_status_begin_received = NULL;
    scils->notifications.on_status_finish_received = NULL;
    scils->notifications.on_status_request_received = NULL;
    scils->notifications.on_version_request_received = NULL;
    scils->notifications.on_version_response_received = NULL;
    scils->notifications.on_show_signal_aspect_received = NULL;
    scils->notifications.on_signal_aspect_status_received = NULL;
    scils->notifications.on_change_brightness_received = NULL;
    scils->notifications.on_brightness_status_received = NULL;

    return scils;
}

void scils_cleanup(scils_t * ls){
    hashmap_free(ls->sciNamesToRastaIds);
    rfree(ls->sciName);
    rfree(ls);
}

sci_return_code scils_send_version_request(scils_t *ls, char *receiver, unsigned char estw_version){
    sci_telegram * telegram = sci_create_version_request(SCI_PROTOCOL_LS, ls->sciName, receiver, estw_version);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_version_response(scils_t *ls, char *receiver, unsigned char btp_version,
                                            sci_version_check_result result,
                                            unsigned char checksum_len, unsigned char *checksum){
    sci_telegram * telegram = sci_create_version_response(SCI_PROTOCOL_LS, ls->sciName, receiver, btp_version, result,
                                                          checksum_len, checksum);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_status_request(scils_t *ls, char *receiver){
    sci_telegram * telegram = sci_create_status_request(SCI_PROTOCOL_LS, ls->sciName, receiver);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_status_begin(scils_t *ls, char *receiver){
    sci_telegram * telegram = sci_create_status_begin(SCI_PROTOCOL_LS, ls->sciName, receiver);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_status_finish(scils_t *ls, char *receiver){
    sci_telegram * telegram = sci_create_status_finish(SCI_PROTOCOL_LS, ls->sciName, receiver);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_show_signal_aspect(scils_t * ls, char * receiver, scils_signal_aspect signal_aspect){
    sci_telegram * telegram = scils_create_show_signal_aspect(ls->sciName, receiver, signal_aspect);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_signal_aspect_status(scils_t * ls, char * receiver, scils_signal_aspect signal_aspect){
    sci_telegram * telegram = scils_create_signal_aspect_status(ls->sciName, receiver, signal_aspect);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_change_brightness(scils_t * ls, char * receiver, scils_brightness brightness){
    sci_telegram * telegram = scils_create_change_brightness(ls->sciName, receiver, brightness);

    return scils_send_telegram(ls, telegram);
}

sci_return_code scils_send_brightness_status(scils_t * ls, char * receiver, scils_brightness brightness){
    sci_telegram * telegram = scils_create_brightness_status(ls->sciName, receiver, brightness);

    return scils_send_telegram(ls, telegram);
}

/**
 * Parses a received version request telegram and calls the notification
 * @param ls the SCI-LS instance
 * @param telegram the version request
 */
void scils_handle_version_request(scils_t * ls, sci_telegram * telegram){
    unsigned char estw_version;
    sci_parse_result result = sci_parse_version_request_payload(telegram, &estw_version);

    if (result == SCI_PARSE_SUCCESS){
        if (ls->notifications.on_version_request_received != NULL){
            (*ls->notifications.on_version_request_received)(ls, telegram->sender, estw_version);
        }
    }
}

/**
 * Parses a received version response telegram and calls the notification
 * @param ls the SCI-LS instance
 * @param telegram the version request
 */
void scils_handle_version_response(scils_t * ls, sci_telegram * telegram){
    unsigned char btp_version, checksum_len;
    sci_version_check_result version_check_result;
    unsigned char checksum[128];

    sci_parse_result result = sci_parse_version_response_payload(telegram, &btp_version, &version_check_result,
                                                                 &checksum_len, &checksum[0]);

    if (result == SCI_PARSE_SUCCESS){
        if (ls->notifications.on_version_response_received != NULL){
            (*ls->notifications.on_version_response_received)(ls, telegram->sender, btp_version, version_check_result,
                                                             checksum_len, checksum);
        }
    }
}

/**
 * Parses a received show signal aspect telegram and calls the notification.
 * @param ls the SCI-LS instance
 * @param telegram the show signal aspect command
 */
void handle_show_signal_aspect(scils_t * ls, sci_telegram * telegram){
    scils_signal_aspect signal_aspect;

    sci_parse_result result = scils_parse_show_signal_aspect_payload(telegram, &signal_aspect);

    if (result == SCI_PARSE_SUCCESS){
        if (ls->notifications.on_show_signal_aspect_received != NULL){
            (*ls->notifications.on_show_signal_aspect_received)(ls, telegram->sender, signal_aspect);
        }
    }
}

/**
 * Parses a received signal aspect status telegram and calls the notification.
 * @param ls the SCI-LS instance
 * @param telegram the signal aspect status
 */
void handle_signal_aspect_status(scils_t * ls, sci_telegram * telegram){
    scils_signal_aspect signal_aspect;

    sci_parse_result result = scils_parse_signal_aspect_status_payload(telegram, &signal_aspect);

    if (result == SCI_PARSE_SUCCESS){
        if (ls->notifications.on_signal_aspect_status_received != NULL){
            (*ls->notifications.on_signal_aspect_status_received)(ls, telegram->sender, signal_aspect);
        }
    }
}

/**
 * Parses a received change brightness telegram and calls the notification.
 * @param ls the SCI-LS instance
 * @param telegram the change brightness command
 */
void handle_change_brightness(scils_t * ls, sci_telegram * telegram){
    scils_brightness brightness;

    sci_parse_result result = scils_parse_change_brightness_payload(telegram, &brightness);

    if (result == SCI_PARSE_SUCCESS){
        if (ls->notifications.on_change_brightness_received != NULL){
            (*ls->notifications.on_change_brightness_received)(ls, telegram->sender, brightness);
        }
    }
}

/**
 * Parses a received brightness status telegram and calls the notification.
 * @param ls the SCI-LS instance
 * @param telegram the brightness status
 */
void handle_brightness_status(scils_t * ls, sci_telegram * telegram){
    scils_brightness brightness;

    sci_parse_result result = scils_parse_brightness_status_payload(telegram, &brightness);

    if (result == SCI_PARSE_SUCCESS){
        if (ls->notifications.on_brightness_status_received != NULL){
            (*ls->notifications.on_brightness_status_received)(ls, telegram->sender, brightness);
        }
    }
}

void scils_on_rasta_receive(scils_t * ls, rastaApplicationMessage message){
    sci_telegram * parsed = sci_decode_telegram(message.appMessage);

    if (parsed == NULL){
        // parsing error -> it's not a SCI telegram
        return;
    }

    if (parsed->protocol_type != SCI_PROTOCOL_LS){
        // not a SCI-LS telegram
        return;
    }

    // add SCI name <-> RaSTA ID relation to map if not already in there
    scils_register_sci_name(ls, parsed->sender, message.id);

    // handle the received telegram
    switch (sci_get_message_type(parsed)){
        case SCI_MESSAGE_TYPE_VERSION_REQUEST:
            scils_handle_version_request(ls, parsed);
            break;
        case SCI_MESSAGE_TYPE_VERSION_RESPONSE:
            scils_handle_version_response(ls, parsed);
            break;
        case SCI_MESSAGE_TYPE_STATUS_REQUEST:
            // call the notification handler if not null
            if (ls->notifications.on_status_request_received != NULL){
                (*ls->notifications.on_status_request_received)(ls, parsed->sender);
            }
            break;
        case SCI_MESSAGE_TYPE_STATUS_BEGIN:
            // call the notification handler if not null
            if (ls->notifications.on_status_begin_received!= NULL){
                (*ls->notifications.on_status_begin_received)(ls, parsed->sender);
            }
            break;
        case SCI_MESSAGE_TYPE_STATUS_FINISH:
            // call the notification handler if not null
            if (ls->notifications.on_status_finish_received != NULL){
                (*ls->notifications.on_status_finish_received)(ls, parsed->sender);
            }
            break;
        case SCILS_MESSAGE_TYPE_SHOW_SIGNAL_ASPECT:
            handle_show_signal_aspect(ls, parsed);
            break;
        case SCILS_MESSAGE_TYPE_SIGNAL_ASPECT_STATUS:
            handle_signal_aspect_status(ls, parsed);
            break;
        case SCILS_MESSAGE_TYPE_CHANGE_BRIGHTNESS:
            handle_change_brightness(ls, parsed);
            break;
        case SCILS_MESSAGE_TYPE_SIGNAL_BRIGHTNESS_STATUS:
            handle_brightness_status(ls, parsed);
            break;
        default:
            return;
    }
}

void scils_register_sci_name(scils_t * ls, char * sci_name, unsigned long rasta_id){
    sci_telegram tmp;
    sci_set_sender(&tmp, sci_name);

    char * key = sci_get_name_string(tmp.sender);

    unsigned long rastaId;
    int res = hashmap_get(ls->sciNamesToRastaIds, key, (void **) &rastaId);
    if (res == MAP_MISSING){
        // SCI name not in map -> add to map
        res = hashmap_put(ls->sciNamesToRastaIds, key, (any_t) rasta_id);
        if (res != MAP_OK){
            // something went wrong
            rfree(key);
            return;
        }
    }
}