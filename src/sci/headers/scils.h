#ifndef LST_SIMULATOR_SCILS_H
#define LST_SIMULATOR_SCILS_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <sci.h>
#include <hashmap.h>
#include <scils_telegram_factory.h>
#include <rasta_new.h>

/**
 * define struct as type here to allow usage in notification pointers
 */
typedef struct scils_t scils_t;

/**
 * Pointer to a function that will be called when a version request telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the ESTW version.
 */
typedef void(*scils_on_version_request_received_ptr)(scils_t *, char *, unsigned char);

/**
 * Pointer to a function that will be called when a version response telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the BTP version
 * 4th parameter is the version check result
 * 5th parameter is the checksum length
 * 6th parameter is the checksum
 */
typedef void(*scils_on_version_response_received_ptr)(scils_t *, char *, unsigned char, sci_version_check_result, unsigned char,
                                                     unsigned char *);

/**
 * Pointer to a function that will be called when a status request telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 */
typedef void(*scils_on_status_request_received_ptr)(scils_t *, char*);

/**
 * Pointer to a function that will be called when a status begin telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 */
typedef void(*scils_on_status_begin_received_ptr)(scils_t *, char*);

/**
 * Pointer to a function that will be called when a status finish telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 */
typedef void(*scils_on_status_finish_received_ptr)(scils_t *, char*);

/**
 * Pointer to a function that will be called when a show signal aspect telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the signal aspect to display
 */
typedef void(*scils_on_show_signal_aspect_received_ptr)(scils_t *, char *, scils_signal_aspect);

/**
 * Pointer to a function that will be called when a signal aspect status telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the signal aspect that is displayed
 */
typedef void(*scils_on_signal_aspect_status_received_ptr)(scils_t *, char *, scils_signal_aspect);

/**
 * Pointer to a function that will be called when a change brightness telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the brightness to display
 */
typedef void(*scils_on_change_brightness_received_ptr)(scils_t *, char *, scils_brightness);

/**
 * Pointer to a function that will be called when a brightness status telegram is received.
 * 1st parameter is the scils instance.
 * 2nd parameter is the sender name.
 * 3rd parameter is the displayed brightness
 */
typedef void(*scils_on_brightness_status_received_ptr)(scils_t *, char *, scils_brightness);

/**
 * Representation of notifications/events for a SCI-LS instance
 */
typedef struct {
    /**
     * Invoked when a version request is received.
     */
    scils_on_version_request_received_ptr on_version_request_received;
    /**
     * Invoked when a version response is received.
     */
    scils_on_version_response_received_ptr on_version_response_received;
    /**
     * Invoked when a status request is received.
     */
    scils_on_status_request_received_ptr on_status_request_received;
    /**
     * Invoked when a status begin is received.
     */
    scils_on_status_begin_received_ptr on_status_begin_received;
    /**
     * Invoked when a status finish is received.
     */
    scils_on_status_finish_received_ptr on_status_finish_received;
    /**
     * Invoked when a show signal aspect is received.
     */
    scils_on_show_signal_aspect_received_ptr on_show_signal_aspect_received;
    /**
     * Invoked when a signal aspect status is received.
     */
    scils_on_signal_aspect_status_received_ptr on_signal_aspect_status_received;
    /**
     * Invoked when change brightness is received.
     */
    scils_on_change_brightness_received_ptr on_change_brightness_received;
    /**
     * Invoked when a brighness status is received.
     */
    scils_on_brightness_status_received_ptr on_brightness_status_received;
}scils_notification_ptr;

/**
 * Representation of a SCI-LS instance.
 */
struct scils_t{
    /**
     * The SCI name of this SCI-LS instance.
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

    scils_notification_ptr notifications;
};

/**
 * Initializes a SCI-LS instance on top of the RaSTA handle.
 * @param handle the underlying RaSTA handle
 * @param sciName the SCI name of the instance
 * @return an initialized SCI instance
 */
scils_t * scils_init(struct rasta_handle * handle, char * sciName);

/**
 * Cleans up and frees the memory of the SCI-LS instance.
 * @param ls the SCI-LS instance
 */
void scils_cleanup(scils_t * ls);

/**
 * Sends a version request to the specified receiver.
 * @param p the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @param estw_version the ESTW version
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_version_request(scils_t *ls, char *receiver, unsigned char estw_version);

/**
 * Sends a version response to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @param btp_version the version of the BTP
 * @param result the result of the version check
 * @param checksum_len the length of the checksum
 * @param checksum the checksum data
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_version_response(scils_t *ls, char *receiver, unsigned char btp_version,
                                           sci_version_check_result result,
                                           unsigned char checksum_len, unsigned char *checksum);

/**
 * Sends a status request to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_status_request(scils_t *ls, char *receiver);

/**
 * Sends a status begin to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_status_begin(scils_t *ls, char *receiver);

/**
 * Sends a status finish to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_status_finish(scils_t *ls, char *receiver);

/**
 * Sends a show signal aspect command to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @param signal_aspect the signal aspect to display
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_show_signal_aspect(scils_t * ls, char * receiver, scils_signal_aspect signal_aspect);

/**
 * Sends a signal aspect status to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @param signal_aspect the displayed signal aspect
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_signal_aspect_status(scils_t * ls, char * receiver, scils_signal_aspect signal_aspect);

/**
 * Sends a change brightness command to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @param brightness the brightness to display
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_change_brightness(scils_t * ls, char * receiver, scils_brightness brightness);

/**
 * Sends a brightness status to the specified receiver.
 * @param ls the used SCI-LS instance
 * @param receiver the SCI name of the receiver
 * @param brightness the displayed brightness
 * @return 0 if the operation was successful, error code otherwise
 */
sci_return_code scils_send_brightness_status(scils_t * ls, char * receiver, scils_brightness brightness);

/**
 * Handles received messages from the underlying RaSTA instance.
 * You need to call this function inside RaSTA's onReceiver Notification in order to receive SCI-LS telegrams!
 * @param message the received RaSTA application message
 */
void scils_on_rasta_receive(scils_t * ls, rastaApplicationMessage message);

/**
 * Registers the relation between a SCI name and a RaSTA ID.
 * @param ls the SCI-LS instance
 * @param sci_name the SCI name of the remote
 * @param rasta_id the RaSTA ID of the remote
 */
void scils_register_sci_name(scils_t * ls, char * sci_name, unsigned long rasta_id);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_SCILS_H
