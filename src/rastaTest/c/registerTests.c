
#include "registerTests.h"

// INCLUDE TESTS
#include "rastamoduleTest.h"
#include "rastamd4Test.h"
#include "rastacrcTest.h"
#include "rastafactoryTest.h"
#include "dictionarytest.h"
#include "rastadeferqueueTest.h"
#include "configtest.h"
#include "rastalisttest.h"
#include "fifotest.h"
#include "blake2test.h"

int suite_init(void) {
    return 0;
}

int suite_clean(void) {
    return 0;
}

void gradle_cunit_register() {
    CU_pSuite pSuiteMath = CU_add_suite("port tests", suite_init, suite_clean);
    CU_add_test(pSuiteMath, "testConversion", testConversion);

    //MD4 tests
    CU_add_test(pSuiteMath, "testMD4function", testMD4function);
    CU_add_test(pSuiteMath, "testRastaMD4Sample", testRastaMD4Sample);

    // Tests for the crc module
    CU_add_test(pSuiteMath, "test_opt_b", test_opt_b);
    CU_add_test(pSuiteMath, "test_opt_c", test_opt_c);
    CU_add_test(pSuiteMath, "test_opt_d", test_opt_d);
    CU_add_test(pSuiteMath, "test_opt_e", test_opt_e);
    CU_add_test(pSuiteMath, "test_without_gen_table", test_without_gen_table);

    //Tests for rastafactory
    CU_add_test(pSuiteMath, "checkConnectionPacket", checkConnectionPacket);
    CU_add_test(pSuiteMath, "checkNormalPacket", checkNormalPacket);
    CU_add_test(pSuiteMath, "checkDisconnectionRequest", checkDisconnectionRequest);
    CU_add_test(pSuiteMath, "checkMessagePacket", checkMessagePacket);


    // Tests for the Redundancy layer factory and model
    CU_add_test(pSuiteMath, "testRedundancyConversionWithCrcChecksumCorrect", testRedundancyConversionWithCrcChecksumCorrect);
    CU_add_test(pSuiteMath, "testRedundancyConversionWithoutChecksum", testRedundancyConversionWithoutChecksum);
    CU_add_test(pSuiteMath, "testRedundancyConversionIncorrectChecksum", testRedundancyConversionIncorrectChecksum);
    CU_add_test(pSuiteMath, "testCreateRedundancyPacket", testCreateRedundancyPacket);
    CU_add_test(pSuiteMath, "testCreateRedundancyPacketNoChecksum", testCreateRedundancyPacketNoChecksum);

    //Test for dictionary
    CU_add_test(pSuiteMath, "testDictionary", testDictionary);

    //Test for config
    CU_add_test(pSuiteMath, "check_std_config", check_std_config);
    CU_add_test(pSuiteMath, "check_var_config", check_var_config);


    // Tests for the defer queue
    CU_add_test(pSuiteMath, "test_deferqueue_init", test_deferqueue_init);
    CU_add_test(pSuiteMath, "test_deferqueue_add", test_deferqueue_add);
    CU_add_test(pSuiteMath, "test_deferqueue_add_full", test_deferqueue_add_full);
    CU_add_test(pSuiteMath, "test_deferqueue_remove", test_deferqueue_remove);
    CU_add_test(pSuiteMath, "test_deferqueue_remove_not_in_queue", test_deferqueue_remove_not_in_queue);
    CU_add_test(pSuiteMath, "test_deferqueue_contains", test_deferqueue_contains);
    CU_add_test(pSuiteMath, "test_deferqueue_smallestseqnr", test_deferqueue_smallestseqnr);
    CU_add_test(pSuiteMath, "test_deferqueue_destroy", test_deferqueue_destroy);
    CU_add_test(pSuiteMath, "test_deferqueue_isfull", test_deferqueue_isfull);
    CU_add_test(pSuiteMath, "test_deferqueue_get", test_deferqueue_get);
    CU_add_test(pSuiteMath, "test_deferqueue_sorted", test_deferqueue_sorted);
    CU_add_test(pSuiteMath, "test_deferqueue_get_ts", test_deferqueue_get_ts);
    CU_add_test(pSuiteMath, "test_deferqueue_clear", test_deferqueue_clear);
    CU_add_test(pSuiteMath, "test_deferqueue_get_ts_doesnt_contain", test_deferqueue_get_ts_doesnt_contain);

    //tests for rastalist
    CU_add_test(pSuiteMath, "check_rastalist", check_rastalist);

    // Tests for the FIFO
    CU_add_test(pSuiteMath, "test_push", test_push);
    CU_add_test(pSuiteMath, "test_pop", test_pop);

    // Tests for BLAKE2 hashes
    CU_add_test(pSuiteMath, "testBlake2Hash", testBlake2Hash);
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
