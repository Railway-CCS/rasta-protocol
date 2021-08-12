#include <scipTests.h>
#include <CUnit/CUnit.h>

#include <scip_telegram_factory.h>
#include <rmemory.h>
#include <sci.h>

void testCreateChangeLocation(){
    unsigned char expected[] = {
            0x40,
            0x00, 0x01,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x01
    };

    sci_telegram * telegram = scip_create_change_location_telegram("ab", "cd", POINT_LOCATION_CHANGE_TO_RIGHT);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected, res.bytes, res.length));

    freeRastaByteArray(&res);
    rfree(telegram);
}

void testCreateLocationStatus(){
    unsigned char expected[] = {
            0x40,
            0x00, 0x0B,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x04
    };

    sci_telegram * telegram = scip_create_location_status_telegram("ab", "cd", POINT_BUMPED);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected, res.bytes, res.length));

    freeRastaByteArray(&res);
    rfree(telegram);
}

void testCreateTimeout(){
    unsigned char expected[] = {
            0x40,
            0x00, 0x0C,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F
    };

    sci_telegram * telegram = scip_create_timeout_telegram("ab", "cd");
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected, res.bytes, res.length));

    freeRastaByteArray(&res);
    rfree(telegram);
}

void testParseChangeLocation(){
    sci_telegram * telegram = scip_create_change_location_telegram("ab", "cd", POINT_LOCATION_CHANGE_TO_RIGHT);

    scip_point_target_location location;
    sci_parse_result result = scip_parse_change_location_payload(telegram, &location);

    CU_ASSERT_EQUAL_FATAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(location, POINT_LOCATION_CHANGE_TO_RIGHT);

    sci_set_message_type(telegram, SCIP_MESSAGE_TYPE_TIMEOUT);
    result = scip_parse_change_location_payload(telegram, &location);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);
}

void testParseLocationStatus(){
    sci_telegram * telegram = scip_create_location_status_telegram("ab", "cd", POINT_NO_TARGET_LOCATION);

    scip_point_location location;
    sci_parse_result result = scip_parse_location_status_payload(telegram, &location);

    CU_ASSERT_EQUAL_FATAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(location, POINT_NO_TARGET_LOCATION);

    sci_set_message_type(telegram, SCIP_MESSAGE_TYPE_TIMEOUT);
    result = scip_parse_location_status_payload(telegram, &location);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);
}