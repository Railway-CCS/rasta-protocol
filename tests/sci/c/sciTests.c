#include <sciTests.h>
#include <CUnit/CUnit.h>

#include <sci.h>
#include <sci_telegram_factory.h>
#include <rmemory.h>

void testEncode(){
    unsigned char expected_telegram[] = {
            0x30,
            0x00, 0x11,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x42
    };

    // create the telegram
    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));

    telegram->protocol_type = SCI_PROTOCOL_LS;

    sci_set_message_type(telegram, 0x1100);

    sci_set_sender(telegram, "ab");
    CU_ASSERT_NSTRING_EQUAL(telegram->sender, "ab__________________", SCI_NAME_LENGTH);

    sci_set_receiver(telegram, "cd");
    CU_ASSERT_NSTRING_EQUAL(telegram->receiver, "cd__________________", SCI_NAME_LENGTH);

    sci_payload payload;
    payload.used_bytes = 1;
    payload.data[0] = 0x42;
    telegram->payload = payload;

    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(expected_telegram, res.bytes, res.length));

    rfree(telegram);
}

void testDecode(){
    unsigned char telegram_bytes[] = {
            0x30,
            0x00, 0x11,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x42
    };

    unsigned char message_type[2] = {0x00, 0x11};
    unsigned char payload[1] = {0x42};

    struct RastaByteArray data;
    allocateRastaByteArray(&data, 44);
    rmemcpy(data.bytes, telegram_bytes, 44);

    sci_telegram * telegram = sci_decode_telegram(data);

    CU_ASSERT_PTR_NOT_NULL_FATAL(telegram);

    CU_ASSERT_EQUAL(telegram->protocol_type, SCI_PROTOCOL_LS);
    CU_ASSERT_NSTRING_EQUAL(telegram->sender, "ab__________________", SCI_NAME_LENGTH);
    CU_ASSERT_NSTRING_EQUAL(telegram->receiver, "cd__________________", SCI_NAME_LENGTH);

    CU_ASSERT_EQUAL(0, memcmp(telegram->message_type, message_type, 2));

    CU_ASSERT_EQUAL(telegram->payload.used_bytes, 1);
    CU_ASSERT_EQUAL(0, memcmp(telegram->payload.data, payload, telegram->payload.used_bytes));

    rfree(telegram);
    freeRastaByteArray(&data);
}

void testDecodeInvalid(){
    unsigned char telegram_bytes[] = {
            0x60,
            0x00, 0x11,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x42
    };

    struct RastaByteArray data;
    allocateRastaByteArray(&data, 44);
    rmemcpy(data.bytes, telegram_bytes, 44);

    CU_ASSERT_PTR_NULL(sci_decode_telegram(data));


    unsigned char telegram_bytes2[] = {
            0x30
    };

    allocateRastaByteArray(&data, 1);
    rmemcpy(data.bytes, telegram_bytes2, 1);

    CU_ASSERT_PTR_NULL(sci_decode_telegram(data));

    unsigned char telegram_bytes3[129] = {0};
    allocateRastaByteArray(&data, 129);
    rmemcpy(data.bytes, telegram_bytes3, 129);

    CU_ASSERT_PTR_NULL(sci_decode_telegram(data));

    freeRastaByteArray(&data);
}

void testSetSender(){
    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));

    sci_set_sender(telegram, "Test");

    CU_ASSERT_NSTRING_EQUAL(telegram->sender, "Test________________", SCI_NAME_LENGTH);

    rfree(telegram);
}

void testSetReceiver(){
    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));

    sci_set_receiver(telegram, "Test");

    CU_ASSERT_NSTRING_EQUAL(telegram->receiver, "Test________________", SCI_NAME_LENGTH);

    rfree(telegram);
}

void testGetName(){
    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));
    sci_set_sender(telegram, "Test");
    CU_ASSERT_NSTRING_EQUAL(telegram->sender, "Test________________", SCI_NAME_LENGTH);

    char * name_str = sci_get_name_string(telegram->sender);

    CU_ASSERT_NSTRING_EQUAL(name_str, "Test________________", SCI_NAME_LENGTH + 1);

    rfree(name_str);
    rfree(telegram);
}

void testSetMessageType(){
    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));

    sci_set_message_type(telegram, 0x1100);

    CU_ASSERT_EQUAL(telegram->message_type[0], 0x00);
    CU_ASSERT_EQUAL(telegram->message_type[1], 0x11);

    rfree(telegram);
}

void testCreateVersionRequest(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x24,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x01
    };

    sci_telegram * telegram = sci_create_version_request(SCI_PROTOCOL_LS, "ab", "cd", 0x01);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(res.bytes, expected, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testCreateVersionResponse(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x25,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x02,
            0x02,
            0x02,
            0xDE, 0xAD
    };


    unsigned char checksum[] = {0xDE, 0xAD};

    sci_telegram * telegram = sci_create_version_response(SCI_PROTOCOL_LS, "ab", "cd", 0x02, SCI_VERSION_CHECK_RESULT_VERSIONS_ARE_EQUAL, 2, checksum);
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(res.bytes, expected, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testCreateStatusRequest(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x21,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F
    };

    sci_telegram * telegram = sci_create_status_request(SCI_PROTOCOL_LS, "ab", "cd");
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(res.bytes, expected, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testCreateStatusBegin(){
    unsigned char expected[] = {
            0x30,
            0x00, 0x22,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F
    };

    sci_telegram * telegram = sci_create_status_begin(SCI_PROTOCOL_LS, "ab", "cd");
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(res.bytes, expected, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testCreateStatusFinish(){
    unsigned char expected[] = {
            0x40,
            0x00, 0x23,
            0x61, 0x62, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F,
            0x63, 0x64, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F
    };

    sci_telegram * telegram = sci_create_status_finish(SCI_PROTOCOL_P, "ab", "cd");
    struct RastaByteArray res = sci_encode_telegram(telegram);

    CU_ASSERT_EQUAL(0, memcmp(res.bytes, expected, res.length));

    rfree(telegram);
    freeRastaByteArray(&res);
}

void testGetMessageType(){
    sci_telegram * telegram = rmalloc(sizeof(sci_telegram));

    sci_set_message_type(telegram, 0x1100);
    CU_ASSERT_EQUAL(sci_get_message_type(telegram), 0x1100);

    rfree(telegram);
}

void testParseVersionRequest(){
    sci_telegram * telegram = sci_create_version_request(SCI_PROTOCOL_LS, "ab", "cd", 0x42);
    unsigned char version;
    sci_parse_result result = sci_parse_version_request_payload(telegram, &version);
    CU_ASSERT_EQUAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(version, 0x42);

    sci_set_message_type(telegram, SCI_MESSAGE_TYPE_VERSION_RESPONSE);
    result = sci_parse_version_request_payload(telegram, &version);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);
}

void testParseVersionResponse(){
    unsigned char expected_checksum[] = {0x0A, 0x0B};
    sci_telegram * telegram = sci_create_version_response(SCI_PROTOCOL_LS, "ab", "cd", 0x01,
                                                          SCI_VERSION_CHECK_RESULT_VERSIONS_ARE_EQUAL,
                                                          2, expected_checksum);

    unsigned char version, len;
    unsigned char checksum[2];
    sci_version_check_result version_check_result;

    sci_parse_result result = sci_parse_version_response_payload(telegram, &version, &version_check_result, &len, &checksum[0]);
    CU_ASSERT_EQUAL(result, SCI_PARSE_SUCCESS);
    CU_ASSERT_EQUAL(version, 0x01);
    CU_ASSERT_EQUAL(len, 2);
    CU_ASSERT_EQUAL(version_check_result, SCI_VERSION_CHECK_RESULT_VERSIONS_ARE_EQUAL);
    CU_ASSERT_EQUAL(0, memcmp(checksum, expected_checksum, 2));

    telegram->payload.used_bytes = 3;
    result = sci_parse_version_response_payload(telegram, &version, &version_check_result, &len, &checksum[0]);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_PAYLOAD_LENGTH);

    telegram->payload.used_bytes = 4;
    result = sci_parse_version_response_payload(telegram, &version, &version_check_result, &len, &checksum[0]);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_PAYLOAD_LENGTH);

    sci_set_message_type(telegram, SCI_MESSAGE_TYPE_VERSION_REQUEST);
    result = sci_parse_version_response_payload(telegram, &version, &version_check_result, &len, &checksum[0]);
    CU_ASSERT_EQUAL(result, SCI_PARSE_INVALID_MESSAGE_TYPE);
}