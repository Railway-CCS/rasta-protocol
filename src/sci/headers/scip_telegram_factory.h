#ifndef LST_SIMULATOR_SCIP_TELEGRAM_FACTORY_H
#define LST_SIMULATOR_SCIP_TELEGRAM_FACTORY_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <sci.h>
#include <sci_telegram_factory.h>

/**
 * Message type of a SCI-P change location command
 */
#define SCIP_MESSAGE_TYPE_CHANGE_LOCATION 0x0100
/**
 * Message type of a SCI-P location status message
 */
#define SCIP_MESSAGE_TYPE_LOCATION_STATUS 0x0B00
/**
 * Message type of a SCI-P timeout message
 */
#define SCIP_MESSAGE_TYPE_TIMEOUT 0x0C00

/**
 * Enumeration with the allowed results for a location change command
 */
typedef enum {
    POINT_LOCATION_CHANGE_TO_RIGHT = 0x01,
    POINT_LOCATION_CHANGE_TO_LEFT = 0x02
} scip_point_target_location;

/**
 * Enumeration with the allowed results for a location status message
 */
typedef enum {
    POINT_LOCATION_RIGHT = 0x01,
    POINT_LOCATION_LEFT = 0x02,
    POINT_NO_TARGET_LOCATION = 0x03,
    POINT_BUMPED = 0x04
} scip_point_location;

/**
 * Creates a SCI-P change location telegram
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param location the target location of the point
 * @return a change location telegram
 */
sci_telegram * scip_create_change_location_telegram(char *sender, char *receiver, scip_point_target_location location);

/**
 * Creates a SCI-P location status telegram
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param location the location of the point
 * @return a location status telegram
 */
sci_telegram * scip_create_location_status_telegram(char *sender, char *receiver, scip_point_location location);

/**
 * Creates a SCI-P timeout telegram
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @return a timeout telegram
 */
sci_telegram * scip_create_timeout_telegram(char *sender, char *receiver);

/**
 * Tries to parse the payload of a change location telegram. The values will be written to the corresponding pointers.
 * @param telegram the change location telegram
 * @param location value: the point's target location
 * @return 0 if success, error code otherwise
 */
sci_parse_result scip_parse_change_location_payload(sci_telegram * telegram, scip_point_target_location * location);

/**
 * Tries to parse the payload of a location status telegram. The values will be written to the corresponding pointers.
 * @param telegram the location status telegram
 * @param location value: the point's location
 * @return 0 if success, error code otherwise
 */
sci_parse_result scip_parse_location_status_payload(sci_telegram * telegram, scip_point_location * location);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_SCIP_TELEGRAM_FACTORY_H
