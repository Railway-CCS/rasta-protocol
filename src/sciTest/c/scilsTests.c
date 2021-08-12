#include <scilsTests.h>
#include <CUnit/CUnit.h>

#include <scils_telegram_factory.h>
#include <rmemory.h>
#include <sci.h>

void testSignalAspectDefaults(){
    scils_signal_aspect * signal_aspect = scils_signal_aspect_defaults();

    CU_ASSERT_EQUAL(signal_aspect->main, SCILS_MAIN_OFF);
    CU_ASSERT_EQUAL(signal_aspect->additional, SCILS_ADDITIONAL_OFF);
    CU_ASSERT_EQUAL(signal_aspect->zs3, SCILS_ZS3_OFF);
    CU_ASSERT_EQUAL(signal_aspect->zs3v, SCILS_ZS3_OFF);
    CU_ASSERT_EQUAL(signal_aspect->zs2, SCILS_ZS2_OFF);
    CU_ASSERT_EQUAL(signal_aspect->zs2v, SCILS_ZS2_OFF);
    CU_ASSERT_EQUAL(signal_aspect->deprecation_information, SCILS_DEPRECIATION_INFORMATION_NO_INFORMATION);
    CU_ASSERT_EQUAL(signal_aspect->upstream_driveway_information, SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION);
    CU_ASSERT_EQUAL(signal_aspect->downstream_driveway_information, SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION);
    CU_ASSERT_EQUAL(signal_aspect->dark_switching, SCILS_DARK_SWITCHING_SHOW);

    rfree(signal_aspect);
}

void testCreateShowSignalAspect(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x01,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x01,
            0x05,
            0x0A,
            0xFF,
            0xFF,
            0x1A,
            0x03,
            0x23,
            0x01
    };
    scils_signal_aspect signal_aspect;
    signal_aspect.main = SCILS_MAIN_HP_0;
    signal_aspect.additional = SCILS_ADDITIONAL_ZS_13;
    signal_aspect.zs3 = SCILS_ZS3_INDEX_10;
    signal_aspect.zs3v = SCILS_ZS3_OFF;
    signal_aspect.zs2 = SCILS_ZS2_OFF;
    signal_aspect.zs2v = SCILS_ZS2_LETTER_Z;
    signal_aspect.deprecation_information = SCILS_DEPRECIATION_INFORMATION_TYPE_3;
    signal_aspect.upstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_WAY_3;
    signal_aspect.downstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_WAY_2;
    signal_aspect.dark_switching = SCILS_DARK_SWITCHING_SHOW;

    sci_telegram * telegram = scils_create_show_signal_aspect("ab", "cd", signal_aspect);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected, res.bytes, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testCreateSignalAspectStatus(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x03,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x01,
            0x05,
            0x0A,
            0xFF,
            0xFF,
            0x1A,
            0x01
    };
    scils_signal_aspect signal_aspect;
    signal_aspect.main = SCILS_MAIN_HP_0;
    signal_aspect.additional = SCILS_ADDITIONAL_ZS_13;
    signal_aspect.zs3 = SCILS_ZS3_INDEX_10;
    signal_aspect.zs3v = SCILS_ZS3_OFF;
    signal_aspect.zs2 = SCILS_ZS2_OFF;
    signal_aspect.zs2v = SCILS_ZS2_LETTER_Z;
    signal_aspect.dark_switching = SCILS_DARK_SWITCHING_SHOW;

    sci_telegram * telegram = scils_create_signal_aspect_status("ab", "cd", signal_aspect);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected, res.bytes, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testCreateChangeBrightness(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x02,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x01
    };

    sci_telegram * telegram = scils_create_change_brightness("ab", "cd", SCILS_BRIGHTNESS_DAY);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected, res.bytes, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testCreateBrightnessStatus(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x04,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x02
    };

    sci_telegram * telegram = scils_create_brightness_status("ab", "cd", SCILS_BRIGHTNESS_NIGHT);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected, res.bytes, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testParseShowSignalAspect(){
    scils_signal_aspect signal_aspect;
    signal_aspect.main = SCILS_MAIN_HP_0;
    signal_aspect.additional = SCILS_ADDITIONAL_ZS_13;
    signal_aspect.zs3 = SCILS_ZS3_INDEX_10;
    signal_aspect.zs3v = SCILS_ZS3_OFF;
    signal_aspect.zs2 = SCILS_ZS2_OFF;
    signal_aspect.zs2v = SCILS_ZS2_LETTER_Z;
    signal_aspect.deprecation_information = SCILS_DEPRECIATION_INFORMATION_TYPE_3;
    signal_aspect.upstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_WAY_3;
    signal_aspect.downstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_WAY_2;
    signal_aspect.dark_switching = SCILS_DARK_SWITCHING_SHOW;

    sci_telegram * telegram = scils_create_show_signal_aspect("ab", "cd", signal_aspect);

    scils_signal_aspect parsed;
    sci_parse_result result = scils_parse_show_signal_aspect_payload(telegram, &parsed);

    CU_ASSERT_EQUAL_FATAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(parsed.main, SCILS_MAIN_HP_0);
    CU_ASSERT_EQUAL(parsed.additional, SCILS_ADDITIONAL_ZS_13);
    CU_ASSERT_EQUAL(parsed.zs3, SCILS_ZS3_INDEX_10);
    CU_ASSERT_EQUAL(parsed.zs3v, SCILS_ZS3_OFF);
    CU_ASSERT_EQUAL(parsed.zs2, SCILS_ZS2_OFF);
    CU_ASSERT_EQUAL(parsed.zs2v, SCILS_ZS2_LETTER_Z);
    CU_ASSERT_EQUAL(parsed.deprecation_information, SCILS_DEPRECIATION_INFORMATION_TYPE_3);
    CU_ASSERT_EQUAL(parsed.upstream_driveway_information, SCILS_DRIVE_WAY_INFORMATION_WAY_3);
    CU_ASSERT_EQUAL(parsed.downstream_driveway_information, SCILS_DRIVE_WAY_INFORMATION_WAY_2);
    CU_ASSERT_EQUAL(parsed.dark_switching, SCILS_DARK_SWITCHING_SHOW);

    sci_set_message_type(telegram, SCILS_MESSAGE_TYPE_CHANGE_BRIGHTNESS);
    result = scils_parse_show_signal_aspect_payload(telegram, &parsed);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);

    rfree(telegram);
}

void testParseSignalAspectStatus(){
    scils_signal_aspect signal_aspect;
    signal_aspect.main = SCILS_MAIN_HP_0;
    signal_aspect.additional = SCILS_ADDITIONAL_ZS_13;
    signal_aspect.zs3 = SCILS_ZS3_INDEX_10;
    signal_aspect.zs3v = SCILS_ZS3_OFF;
    signal_aspect.zs2 = SCILS_ZS2_OFF;
    signal_aspect.zs2v = SCILS_ZS2_LETTER_Z;
    signal_aspect.dark_switching = SCILS_DARK_SWITCHING_SHOW;

    sci_telegram * telegram = scils_create_signal_aspect_status("ab", "cd", signal_aspect);
    scils_signal_aspect parsed;
    sci_parse_result result = scils_parse_signal_aspect_status_payload(telegram, &parsed);

    CU_ASSERT_EQUAL_FATAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(parsed.main, SCILS_MAIN_HP_0);
    CU_ASSERT_EQUAL(parsed.additional, SCILS_ADDITIONAL_ZS_13);
    CU_ASSERT_EQUAL(parsed.zs3, SCILS_ZS3_INDEX_10);
    CU_ASSERT_EQUAL(parsed.zs3v, SCILS_ZS3_OFF);
    CU_ASSERT_EQUAL(parsed.zs2, SCILS_ZS2_OFF);
    CU_ASSERT_EQUAL(parsed.zs2v, SCILS_ZS2_LETTER_Z);
    CU_ASSERT_EQUAL(parsed.dark_switching, SCILS_DARK_SWITCHING_SHOW);

    sci_set_message_type(telegram, SCILS_MESSAGE_TYPE_CHANGE_BRIGHTNESS);
    result = scils_parse_signal_aspect_status_payload(telegram, &parsed);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);

    rfree(telegram);
}

void testParseChangeBrightness(){
    sci_telegram * telegram = scils_create_change_brightness("ab", "cd", SCILS_BRIGHTNESS_UNDEFINED);
    scils_brightness brightness;
    sci_parse_result result = scils_parse_change_brightness_payload(telegram, &brightness);

    CU_ASSERT_EQUAL_FATAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(brightness, SCILS_BRIGHTNESS_UNDEFINED);

    sci_set_message_type(telegram, SCILS_MESSAGE_TYPE_SIGNAL_BRIGHTNESS_STATUS);
    result = scils_parse_change_brightness_payload(telegram, &brightness);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);

    rfree(telegram);
}

void testParseBrightnessStatus(){
    sci_telegram * telegram = scils_create_brightness_status("ab", "cd", SCILS_BRIGHTNESS_UNDEFINED);
    scils_brightness brightness;
    sci_parse_result result = scils_parse_brightness_status_payload(telegram, &brightness);

    CU_ASSERT_EQUAL_FATAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(brightness, SCILS_BRIGHTNESS_UNDEFINED);

    sci_set_message_type(telegram, SCILS_MESSAGE_TYPE_CHANGE_BRIGHTNESS);
    result = scils_parse_brightness_status_payload(telegram, &brightness);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);

    rfree(telegram);
}