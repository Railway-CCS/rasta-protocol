#ifndef LST_SIMULATOR_SCILS_TELEGRAM_FACTORY_H
#define LST_SIMULATOR_SCILS_TELEGRAM_FACTORY_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <sci.h>
#include <sci_telegram_factory.h>

/**
 * Message type of a show signal aspect telegram.
 */
#define SCILS_MESSAGE_TYPE_SHOW_SIGNAL_ASPECT 0x0001

/**
 * Message type of a change brightness telegram.
 */
#define SCILS_MESSAGE_TYPE_CHANGE_BRIGHTNESS 0x0002

/**
 * Message type of a signal aspect status telegram.
 */
#define SCILS_MESSAGE_TYPE_SIGNAL_ASPECT_STATUS 0x0003

/**
 * Message type of a brightness status telegram
 */
#define SCILS_MESSAGE_TYPE_SIGNAL_BRIGHTNESS_STATUS 0x0004

/**
 * Enumeration with allowed values for main concept parameter
 */
typedef enum {
    SCILS_MAIN_HP_0 = 0x01,
    SCILS_MAIN_HP_0_PLUS_SH1 = 0x02,
    SCILS_MAIN_HP_0_WITH_DRIVING_INDICATOR = 0x03,
    SCILS_MAIN_KS_1 = 0x04,
    SCILS_MAIN_KS_1_FLASHING = 0x05,
    SCILS_MAIN_KS_1_FLASHING_WITH_ADDITIONAL_LIGHT = 0x06,
    SCILS_MAIN_KS_2 = 0x07,
    SCILS_MAIN_KS_2_WITH_ADDITIONAL_LIGHT = 0x08,
    SCILS_MAIN_SH_1 = 0x09,
    SCILS_MAIN_ID_LIGHT = 0x0A,
    SCILS_MAIN_HP_0_HV = 0xA0,
    SCILS_MAIN_HP_1 = 0xA1,
    SCILS_MAIN_HP_2 = 0xA2,
    SCILS_MAIN_VR_0 = 0xB0,
    SCILS_MAIN_VR_1 = 0xB1,
    SCILS_MAIN_VR_2 = 0xB2,
    SCILS_MAIN_OFF = 0xFF
} scils_main;

/**
 * Enumeration with allowed values for additional concept parameter
 */
typedef enum {
    SCILS_ADDITIONAL_ZS_1 = 0x01,
    SCILS_ADDITIONAL_ZS_7 = 0x02,
    SCILS_ADDITIONAL_ZS_8 = 0x03,
    SCILS_ADDITIONAL_ZS_6 = 0x04,
    SCILS_ADDITIONAL_ZS_13 = 0x05,
    SCILS_ADDITIONAL_OFF = 0xFF
} scils_additional;

/**
 * Enumeration with allowed values for zs3 and zs3v parameter
 */
typedef enum {
    SCILS_ZS3_INDEX_1 = 0x01,
    SCILS_ZS3_INDEX_2 = 0x02,
    SCILS_ZS3_INDEX_3 = 0x03,
    SCILS_ZS3_INDEX_4 = 0x04,
    SCILS_ZS3_INDEX_5 = 0x05,
    SCILS_ZS3_INDEX_6 = 0x06,
    SCILS_ZS3_INDEX_7 = 0x07,
    SCILS_ZS3_INDEX_8 = 0x08,
    SCILS_ZS3_INDEX_9 = 0x09,
    SCILS_ZS3_INDEX_10 = 0x0A,
    SCILS_ZS3_INDEX_11 = 0x0B,
    SCILS_ZS3_INDEX_12 = 0x0C,
    SCILS_ZS3_INDEX_13 = 0x0D,
    SCILS_ZS3_INDEX_14 = 0x0E,
    SCILS_ZS3_INDEX_15 = 0x0F,
    SCILS_ZS3_OFF =  0xFF
} scils_zs3;

/**
 * Enumeration with allowed values for zs2 and zs2v parameter
 */
typedef enum {
    SCILS_ZS2_LETTER_A = 0x01,
    SCILS_ZS2_LETTER_B = 0x02,
    SCILS_ZS2_LETTER_C = 0x03,
    SCILS_ZS2_LETTER_D = 0x04,
    SCILS_ZS2_LETTER_E = 0x05,
    SCILS_ZS2_LETTER_F = 0x06,
    SCILS_ZS2_LETTER_G = 0x07,
    SCILS_ZS2_LETTER_H = 0x08,
    SCILS_ZS2_LETTER_I = 0x09,
    SCILS_ZS2_LETTER_J = 0x0A,
    SCILS_ZS2_LETTER_K = 0x0B,
    SCILS_ZS2_LETTER_L = 0x0C,
    SCILS_ZS2_LETTER_M = 0x0D,
    SCILS_ZS2_LETTER_N = 0x0E,
    SCILS_ZS2_LETTER_O = 0x0F,
    SCILS_ZS2_LETTER_P = 0x10,
    SCILS_ZS2_LETTER_Q = 0x11,
    SCILS_ZS2_LETTER_R = 0x12,
    SCILS_ZS2_LETTER_S = 0x13,
    SCILS_ZS2_LETTER_T = 0x14,
    SCILS_ZS2_LETTER_U = 0x15,
    SCILS_ZS2_LETTER_V = 0x16,
    SCILS_ZS2_LETTER_W = 0x17,
    SCILS_ZS2_LETTER_X = 0x18,
    SCILS_ZS2_LETTER_Y = 0x19,
    SCILS_ZS2_LETTER_Z = 0x1A,
    SCILS_ZS2_OFF = 0xFF
} scils_zs2;

/**
 * Enumeration with allowed values for depreciation information parameter
 */
typedef enum {
    SCILS_DEPRECIATION_INFORMATION_TYPE_1 = 0x01,
    SCILS_DEPRECIATION_INFORMATION_TYPE_2 = 0x02,
    SCILS_DEPRECIATION_INFORMATION_TYPE_3 = 0x03,
    SCILS_DEPRECIATION_INFORMATION_NO_INFORMATION = 0xFF
} scils_deprecation_information;

/**
 * Enumeration with allowed values for drive way information (lower and higher bytes) parameter
 */
typedef enum {
    SCILS_DRIVE_WAY_INFORMATION_WAY_1 = 0x1,
    SCILS_DRIVE_WAY_INFORMATION_WAY_2 = 0x2,
    SCILS_DRIVE_WAY_INFORMATION_WAY_3 = 0x3,
    SCILS_DRIVE_WAY_INFORMATION_WAY_4 = 0x4,
    SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION = 0xF
} scils_driveway_information;

/**
 * Enumeration with allowed values for dark signal aspect parameter
 */
typedef enum {
    SCILS_DARK_SWITCHING_SHOW = 0x01,
    SCILS_DARK_SWITCHING_DARK = 0xFF
} scils_dark_switching;

/**
 * Enumeration with allowed values for luminosity parameter
 */
typedef enum {
    SCILS_BRIGHTNESS_DAY = 0x01,
    SCILS_BRIGHTNESS_NIGHT = 0x02,
    SCILS_BRIGHTNESS_UNDEFINED = 0xFF // Only allowed in telegram: Message Configured Luminosity
} scils_brightness;
/**
 * Enumeration with supported values for route information parameter
 */
typedef enum{
    SCILS_ROUTE_NOT_APPLICABLE=0xFF
} scils_route_information;

/**
 * Representation of a signal aspect configuration.
 */
typedef struct {
    scils_main main;
    scils_additional additional;
    scils_zs3 zs3;
    scils_zs3 zs3v;
    scils_zs2 zs2;
    scils_zs2 zs2v;
    scils_deprecation_information deprecation_information;
    scils_driveway_information upstream_driveway_information;
    scils_driveway_information downstream_driveway_information;
    scils_route_information route_information;
    scils_dark_switching dark_switching;
    uint8_t nationally_specified_information[9];

}scils_signal_aspect;

/**
 * Returns an initialized SCI-LS signal aspect configuration. Brightness is set to DAY, dark switching to DARK, all other
 * values to OFF / UNDEFINED / NO_INFORMATION. The memory for the signal aspect is allocated, so you are responsible for
 * freeing it.
 * @return a signal aspect with default values
 */
scils_signal_aspect * scils_signal_aspect_defaults();

/**
 * Creates a SCI-LS show signal aspect telegram.
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param signal_aspect the signal aspect configuration
 * @return a show signal aspect telegram
 */
sci_telegram * scils_create_show_signal_aspect(char * sender, char * receiver, scils_signal_aspect signal_aspect);

/**
 * Creates a SCI-LS change brightness telegram.
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param brightness
 * @return a change brightness telegram
 */
sci_telegram * scils_create_change_brightness(char * sender, char * receiver, scils_brightness brightness);

/**
 * Creates a SCI-LS signal aspect status telegram.
 * @param sender the sender name without underscores
 * @param receiver the receiver name without underscores
 * @param signal_aspect the signal aspect
 * @return a signal aspect status telegram
 */
sci_telegram * scils_create_signal_aspect_status(char * sender, char * receiver, scils_signal_aspect signal_aspect);

/**
 * Creates a SCI-LS brightness status telegram.
 * @param sender
 * @param receiver
 * @param brightness
 * @return a brightness status telegram
 */
sci_telegram * scils_create_brightness_status(char * sender, char * receiver, scils_brightness brightness);

/**
 * Tries to parse the payload of a show signal aspect telegram. The values will be written to the corresponding pointers.
 * @param telegram the show signal aspect telegram
 * @param signal_aspect value: the signal aspect to display
 * @return 0 if success, error code otherwise
 */
sci_parse_result scils_parse_show_signal_aspect_payload(sci_telegram * telegram, scils_signal_aspect * signal_aspect);

/**
 * Tries to parse the payload of a signal aspect status telegram. The values will be written to the corresponding pointers.
 * @param telegram the show signal aspect telegram
 * @param signal_aspect value: the displayed signal aspect
 * @return 0 if success, error code otherwise
 */
sci_parse_result scils_parse_signal_aspect_status_payload(sci_telegram * telegram, scils_signal_aspect * signal_aspect);

/**
 * Tries to parse the payload of a change brightness telegram. The values will be written to the corresponding pointers.
 * @param telegram the change brightness telegram
 * @param brightness value
 * @return 0 if success, error code otherwise
 */
sci_parse_result scils_parse_change_brightness_payload(sci_telegram * telegram, scils_brightness * brightness);

/**
 * Tries to parse the payload of a brightness status telegram. The values will be written to the corresponding pointers.
 * @param telegram the brightness status telegram
 * @param brightness value
 * @return 0 if success, error code otherwise
 */
sci_parse_result scils_parse_brightness_status_payload(sci_telegram * telegram, scils_brightness * brightness);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_SCILS_TELEGRAM_FACTORY_H
