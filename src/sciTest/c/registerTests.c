#include "registerTests.h"

int suite_init(void) {
    return 0;
}

int suite_clean(void) {
    return 0;
}

void gradle_cunit_register() {
    CU_pSuite sci_suite = CU_add_suite("SCI protocol tests", suite_init, suite_clean);

    // Tests for the common SCI functions
    CU_add_test(sci_suite, "testEncode", testEncode);
    CU_add_test(sci_suite, "testDecode", testDecode);
    CU_add_test(sci_suite, "testDecodeInvalid", testDecodeInvalid);
    CU_add_test(sci_suite, "testSetSender", testSetSender);
    CU_add_test(sci_suite, "testSetReceiver", testSetReceiver);
    CU_add_test(sci_suite, "testGetName", testGetName);
    CU_add_test(sci_suite, "testSetMessageType", testSetMessageType);
    CU_add_test(sci_suite, "testGetMessageType", testGetMessageType);

    // Tests for creation of common SCI telegrams
    CU_add_test(sci_suite, "testCreateVersionRequest", testCreateVersionRequest);
    CU_add_test(sci_suite, "testCreateVersionResponse", testCreateVersionResponse);
    CU_add_test(sci_suite, "testCreateStatusRequest", testCreateStatusRequest);
    CU_add_test(sci_suite, "testCreateStatusBegin", testCreateStatusBegin);
    CU_add_test(sci_suite, "testCreateStatusFinish", testCreateStatusFinish);

    // Tests for parsing of SCI telegram payloads
    CU_add_test(sci_suite, "testParseVersionRequest", testParseVersionRequest);
    CU_add_test(sci_suite, "testParseVersionResponse", testParseVersionResponse);

    // Tests for creating and parsing SCI-P specific telegrams
    CU_add_test(sci_suite, "testCreateChangeLocation", testCreateChangeLocation);
    CU_add_test(sci_suite, "testCreateLocationStatus", testCreateLocationStatus);
    CU_add_test(sci_suite, "testCreateTimeout", testCreateTimeout);
    CU_add_test(sci_suite, "testParseChangeLocation", testParseChangeLocation);
    CU_add_test(sci_suite, "testParseLocationStatus", testParseLocationStatus);

    // Tests for creating and parsing SCI-LS specific telegrams
    CU_add_test(sci_suite, "testSignalAspectDefaults", testSignalAspectDefaults);
    CU_add_test(sci_suite, "testCreateShowSignalAspect", testCreateShowSignalAspect);
    CU_add_test(sci_suite, "testCreateSignalAspectStatus", testCreateSignalAspectStatus);
    CU_add_test(sci_suite, "testCreateChangeBrightness", testCreateChangeBrightness);
    CU_add_test(sci_suite, "testCreateBrightnessStatus", testCreateBrightnessStatus);
    CU_add_test(sci_suite, "testParseShowSignalAspect", testParseShowSignalAspect);
    CU_add_test(sci_suite, "testParseSignalAspectStatus", testParseSignalAspectStatus);
    CU_add_test(sci_suite, "testParseChangeBrightness", testParseChangeBrightness);
    CU_add_test(sci_suite, "testParseBrightnessStatus", testParseBrightnessStatus);
}

#ifdef WITH_CMAKE
int main () {
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    gradle_cunit_register();

    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    CU_cleanup_registry();
    return ret;
}
#endif
