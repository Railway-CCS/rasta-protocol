#include <scip.h>
#include <sci_telegram_factory.h>
#include <sci.h>
#include <rmemory.h>
#include <memory.h>
#include <rasta_new.h>

/**
 * Tries to send a SCI telegram to the receiver using the underlying RaSTA instance
 * @param p the SCI-P instance
 * @param telegram the telegram to send
 * @return 0 if success, error code otherwise
 */
sci_return_code send_telegram(scip_t * p, sci_telegram * telegram){
    struct RastaByteArray data = sci_encode_telegram(telegram);

    char * sci_name = sci_get_name_string(telegram->receiver);

    unsigned long rastaId;
    int res = hashmap_get(p->sciNamesToRastaIds, sci_name, (void **) &rastaId);
    rfree(sci_name);

    if (res == MAP_MISSING){
        // SCI name not in map
        return UNKNOWN_SCI_NAME;
    }

    struct RastaMessageData messageData;
    allocateRastaMessageData(&messageData, 1);
    messageData.data_array[0] = data;

    sr_send(p->rasta_handle, rastaId, messageData);

    freeRastaMessageData(&messageData);
    return SUCCESS;
}
scip_t * scip_init(struct rasta_handle * handle, char * sciName){
    scip_t * scip = rmalloc(sizeof(scip_t));

    scip->rasta_handle = handle;
    scip->sciName = rmalloc((unsigned int)strlen(sciName));
    strcpy(scip->sciName, sciName);

    // initialize map
    scip->sciNamesToRastaIds = hashmap_new();

    // initialize notifications to NULL
    scip->notifications.on_change_location_received = NULL;
    scip->notifications.on_location_status_received = NULL;
    scip->notifications.on_status_begin_received = NULL;
    scip->notifications.on_status_finish_received = NULL;
    scip->notifications.on_status_request_received = NULL;
    scip->notifications.on_timeout_received = NULL;
    scip->notifications.on_version_request_received = NULL;
    scip->notifications.on_version_response_received = NULL;

    return scip;
}

void scip_cleanup(scip_t * p){
    hashmap_free(p->sciNamesToRastaIds);
    rfree(p->sciName);
    rfree(p);
}

sci_return_code scip_send_version_request(scip_t *p, char *receiver, unsigned char estw_version){
    sci_telegram * telegram = sci_create_version_request(SCI_PROTOCOL_P, p->sciName, receiver, estw_version);

    return send_telegram(p, telegram);
}

sci_return_code scip_send_version_response(scip_t *p, char *receiver, unsigned char btp_version,
                                           sci_version_check_result result,
                                           unsigned char checksum_len, unsigned char *checksum){
    sci_telegram * telegram = sci_create_version_response(SCI_PROTOCOL_P, p->sciName, receiver, btp_version, result,
                                                          checksum_len, checksum);

    return send_telegram(p, telegram);
}

sci_return_code scip_send_status_request(scip_t *p, char *receiver){
    sci_telegram * telegram = sci_create_status_request(SCI_PROTOCOL_P, p->sciName, receiver);

    return send_telegram(p, telegram);
}

sci_return_code scip_send_status_begin(scip_t *p, char *receiver){
    sci_telegram * telegram = sci_create_status_begin(SCI_PROTOCOL_P, p->sciName, receiver);

    return send_telegram(p, telegram);
}

sci_return_code scip_send_status_finish(scip_t *p, char *receiver){
    sci_telegram * telegram = sci_create_status_finish(SCI_PROTOCOL_P, p->sciName, receiver);

    return send_telegram(p, telegram);
}

sci_return_code scip_send_change_location(scip_t *p, char *receiver, scip_point_target_location new_location){
    sci_telegram * telegram = scip_create_change_location_telegram(p->sciName, receiver, new_location);

    return send_telegram(p, telegram);
}

sci_return_code scip_send_location_status(scip_t *p, char *receiver, scip_point_location current_location){
    sci_telegram * telegram = scip_create_location_status_telegram(p->sciName, receiver, current_location);

    return send_telegram(p, telegram);
}

sci_return_code scip_send_timeout(scip_t *p, char *receiver){
    sci_telegram * telegram = scip_create_timeout_telegram(p->sciName, receiver);

    return send_telegram(p, telegram);
}

/**
 * Parses a received version request telegram and calls the notification
 * @param p the SCI-P instance
 * @param telegram the version request
 */
void handle_version_request(scip_t * p, sci_telegram * telegram){
    unsigned char estw_version;
    sci_parse_result result = sci_parse_version_request_payload(telegram, &estw_version);

    if (result == SCI_PARSE_SUCCESS){
        if (p->notifications.on_version_request_received != NULL){
            (*p->notifications.on_version_request_received)(p, telegram->sender, estw_version);
        }
    }
}

/**
 * Parses a received version response telegram and calls the notification
 * @param p the SCI-P instance
 * @param telegram the version request
 */
void handle_version_response(scip_t * p, sci_telegram * telegram){
    unsigned char btp_version, checksum_len;
    sci_version_check_result version_check_result;
    unsigned char checksum[128];

    sci_parse_result result = sci_parse_version_response_payload(telegram, &btp_version, &version_check_result,
                                                                 &checksum_len, &checksum[0]);

    if (result == SCI_PARSE_SUCCESS){
        if (p->notifications.on_version_response_received != NULL){
            (*p->notifications.on_version_response_received)(p, telegram->sender, btp_version, version_check_result,
                                                             checksum_len, checksum);
        }
    }
}

/**
 * Parses a received change location telegram and calls the notification
 * @param p the SCI-P instance
 * @param telegram the change location telegram
 */
void handle_change_location(scip_t * p, sci_telegram * telegram){
    scip_point_target_location location;
    sci_parse_result result = scip_parse_change_location_payload(telegram, &location);

    if (result == SCI_PARSE_SUCCESS){
        if (p->notifications.on_change_location_received != NULL){
            (*p->notifications.on_change_location_received)(p, telegram->sender, location);
        }
    }
}

/**
 * Parses a received location status telegram and calls the notification
 * @param p the SCI-P instance
 * @param telegram the location status telegram
 */
void handle_location_status(scip_t * p, sci_telegram * telegram){
    scip_point_location location;
    sci_parse_result result = scip_parse_location_status_payload(telegram, &location);

    if (result == SCI_PARSE_SUCCESS){
        if (p->notifications.on_location_status_received != NULL){
            (*p->notifications.on_location_status_received)(p, telegram->sender, location);
        }
    }
}

void scip_on_rasta_receive(scip_t * p, rastaApplicationMessage message){
    sci_telegram * parsed = sci_decode_telegram(message.appMessage);


    if (parsed == NULL){
        // parsing error -> it's not a SCI telegram
        return;
    }

    if (parsed->protocol_type != SCI_PROTOCOL_P){
        // not a SCI-P telegram
        return;
    }

    // add SCI name <-> RaSTA ID relation to map if not already in there
    scip_register_sci_name(p, parsed->sender, message.id);

    // handle the received telegram
    switch (sci_get_message_type(parsed)){
        case SCI_MESSAGE_TYPE_VERSION_REQUEST:
            handle_version_request(p, parsed);
            break;
        case SCI_MESSAGE_TYPE_VERSION_RESPONSE:
            handle_version_response(p, parsed);
            break;
        case SCI_MESSAGE_TYPE_STATUS_REQUEST:
            // call the notification handler if not null
            if (p->notifications.on_status_request_received != NULL){
                (*p->notifications.on_status_request_received)(p, parsed->sender);
            }
            break;
        case SCI_MESSAGE_TYPE_STATUS_BEGIN:
            // call the notification handler if not null
            if (p->notifications.on_status_begin_received!= NULL){
                (*p->notifications.on_status_begin_received)(p, parsed->sender);
            }
            break;
        case SCI_MESSAGE_TYPE_STATUS_FINISH:
            // call the notification handler if not null
            if (p->notifications.on_status_finish_received != NULL){
                (*p->notifications.on_status_finish_received)(p, parsed->sender);
            }
            break;
        case SCIP_MESSAGE_TYPE_CHANGE_LOCATION:
            handle_change_location(p, parsed);
            break;
        case SCIP_MESSAGE_TYPE_LOCATION_STATUS:
            handle_location_status(p, parsed);
            break;
        case SCIP_MESSAGE_TYPE_TIMEOUT:
            // call the notification handler if not null
            if (p->notifications.on_timeout_received != NULL){
                (*p->notifications.on_timeout_received)(p, parsed->sender);
            }
            break;
        default:
            return;
    }
}

void scip_register_sci_name(scip_t * p, char * sci_name, unsigned long rasta_id){
    sci_telegram tmp;
    sci_set_sender(&tmp, sci_name);

    char * key = sci_get_name_string(tmp.sender);

    unsigned long rastaId;
    int res = hashmap_get(p->sciNamesToRastaIds, key, (void **) &rastaId);
    if (res == MAP_MISSING){
        // SCI name not in map -> add to map
        res = hashmap_put(p->sciNamesToRastaIds, key, (any_t) rasta_id);
        if (res != MAP_OK){
            // something went wrong
            rfree(key);
            return;
        }
    }
}