
#ifndef LST_SIMULATOR_SCIP_H
#define LST_SIMULATOR_SCIP_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <rastahandle.h>
#include <hashmap.h>
#include <sci.h>
#include <scip_telegram_factory.h>
#include <rasta_new.h>

/**
 * define struct as type here to allow usage in notification pointers
 */
typedef struct scip_t scip_t;

/**
 * Pointer to a function that will be called when a version request telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the ESTW version.
 */
typedef void(*scip_on_version_request_received_ptr)(scip_t *, char *, unsigned char);

/**
 * Pointer to a function that will be called when a version response telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the BTP version
 * 4th parameter is the version check result
 * 5th parameter is the checksum length
 * 6th parameter is the checksum
 */
typedef void(*scip_on_version_response_received_ptr)(scip_t *, char *, unsigned char, sci_version_check_result, unsigned char,
                                            unsigned char *);

/**
 * Pointer to a function that will be called when a status request telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 */
typedef void(*scip_on_status_request_received_ptr)(scip_t *, char*);

/**
 * Pointer to a function that will be called when a status begin telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 */
typedef void(*scip_on_status_begin_received_ptr)(scip_t *, char*);

/**
 * Pointer to a function that will be called when a status finish telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 */
typedef void(*scip_on_status_finish_received_ptr)(scip_t *, char*);

/**
 * Pointer to a function that will be called when a change location telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the target location of the point
 */
typedef void(*scip_on_change_location_received_ptr)(scip_t*, char*, scip_point_target_location);

/**
 * Pointer to a function that will be called when a change location telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the current location of the point
 */
typedef void(*scip_on_location_status_received_ptr)(scip_t*, char*, scip_point_location);

/**
 * Pointer to a function that will be called when a timeout telegram is received.
 * 1st parameter is the scip instance.
 * 2nd parameter is the sender name.
 */
typedef void(*scip_on_timeout_received_ptr)(scip_t*, char*);

/**
 * Representation of notifications/events for a SCI-P instance
 */
typedef struct {
    /**
     * Invoked when a version request is received.
     */
    scip_on_version_request_received_ptr on_version_request_received;
    /**
     * Invoked when a version response is received.
     */
    scip_on_version_response_received_ptr on_version_response_received;
    /**
     * Invoked when a status request is received.
     */
    scip_on_status_request_received_ptr on_status_request_received;
    /**
     * Invoked when a status begin is received.
     */
    scip_on_status_begin_received_ptr on_status_begin_received;
    /**
     * Invoked when a status finish is received.
     */
    scip_on_status_finish_received_ptr on_status_finish_received;
    /**
     * Invoked when change location is received.
     */
    scip_on_change_location_received_ptr on_change_location_received;
    /**
     * Invoked when a location status is received.
     */
    scip_on_location_status_received_ptr on_location_status_received;
    /**
     * Invoked when a timeout is received.
     */
    scip_on_timeout_received_ptr on_timeout_received;
}scip_notification_ptr;

/**
 * Representation of a SCI-P instance.
 */
struct scip_t{
    /**
     * The SCI name of this SCI-P instance.
     */
    char * sciName;
    /**
     * The underlying RaSTA handle
     */
    struct rasta_handle * rasta_handle;
    /**
     * Used for mapping RaSTA IDs to SCI names
     */
    map_t sciNamesToRastaIds;
    scip_notification_ptr notifications;
};

/**
 * Initializes a SCI-P instance on top of the RaSTA handle.
 * @param handle the underlying RaSTA handle
 * @param sciName the SCI name of the instance
 * @return an initialized SCI instance
 */
scip_t * scip_init(struct rasta_handle * handle, char * sciName);

/**
 * Cleans up and frees the memory of the SCI-P instance.
 * @param p the SCI-P instance
 */
void scip_cleanup(scip_t * p);

/**
 * Sends a version request to the specified receiver.
 * @param p the used SCI-P instance
 * @param receiver the SCI name of the receiver
 * @param estw_version the ESTW version
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_version_request(scip_t *p, char *receiver, unsigned char estw_version);

/**
 * Sends a version response to the specified receiver.
 * @param p the used SCI-P instance
 * @param receiver the SCI name of the receiver
 * @param btp_version the version of the BTP
 * @param result the result of the version check
 * @param checksum_len the length of the checksum
 * @param checksum the checksum data
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_version_response(scip_t *p, char *receiver, unsigned char btp_version,
                                           sci_version_check_result result,
                                           unsigned char checksum_len, unsigned char *checksum);

/**
 * Sends a status request to the specified receiver.
 * @param p the used SCI-P instance
 * @param receiver the SCI name of the receiver
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_status_request(scip_t *p, char *receiver);

/**
 * Sends a status begin to the specified receiver.
 * @param p the used SCI-P instance
 * @param receiver the SCI name of the receiver
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_status_begin(scip_t *p, char *receiver);

/**
 * Sends a status finish to the specified receiver.
 * @param p the used SCI-P instance
 * @param receiver the SCI name of the receiver
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_status_finish(scip_t *p, char *receiver);

/**
 * Sends a change location command to the specified receiver.
 * @param p the used SCI-P instance
 * @param receiver the SCI name of the receiver
 * @param new_location the target location of the point
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_change_location(scip_t *p, char *receiver, scip_point_target_location new_location);

/**
 * Sends a location status to the specified receiver.
 * @param p the used SCI-P instance
 * @param receiver the SCI name of the receiver
 * @param current_location the point's location
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_location_status(scip_t *p, char *receiver, scip_point_location current_location);

/**
 * Sends a timeout to the specified receiver
 * @param p the SCI-P instance
 * @param receiver the SCI name of the receiver
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scip_send_timeout(scip_t *p, char *receiver);

/**
 * Handles received messages from the underlying RaSTA instance.
 * You need to call this function inside RaSTA's onReceiver Notification in order to receive SCI-P telegrams!
 * @param message the received RaSTA application message
 */
void scip_on_rasta_receive(scip_t * p, rastaApplicationMessage message);

/**
 * Registers the relation between a SCI name and a RaSTA ID.
 * @param p the SCI-P instance
 * @param sci_name the SCI name of the remote
 * @param rasta_id the RaSTA ID of the remote
 */
void scip_register_sci_name(scip_t * p, char * sci_name, unsigned long rasta_id);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_SCIP_H
