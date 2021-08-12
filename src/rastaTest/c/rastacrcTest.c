#include <CUnit/Basic.h>
#include "rastacrc.h"

#define TEST_VAL "123456789"

#define OPT_B_EXPECTED 0x0E7C650A
#define OPT_C_EXPECTED 0xE3069283
#define OPT_D_EXPECTED 0x2189
#define OPT_E_EXPECTED 0xBB3D

void test_opt_b(){
    struct RastaByteArray data_to_test;
    allocateRastaByteArray(&data_to_test, 9);
    data_to_test.bytes = (unsigned char*)TEST_VAL;

    struct crc_options options_b = crc_init_opt_b();
    crc_generate_table(&options_b);

    unsigned long res = crc_calculate(&options_b, data_to_test);

    CU_ASSERT_EQUAL(res, OPT_B_EXPECTED);
}

void test_opt_c(){
    struct RastaByteArray data_to_test;
    allocateRastaByteArray(&data_to_test, 9);
    data_to_test.bytes = (unsigned char*)TEST_VAL;

    struct crc_options options_c = crc_init_opt_c();
    crc_generate_table(&options_c);

    unsigned long res = crc_calculate(&options_c, data_to_test);

    CU_ASSERT_EQUAL(res, OPT_C_EXPECTED);
}

void test_opt_d(){
    struct RastaByteArray data_to_test;
    allocateRastaByteArray(&data_to_test, 9);
    data_to_test.bytes = (unsigned char*)TEST_VAL;

    struct crc_options options_d = crc_init_opt_d();
    crc_generate_table(&options_d);

    unsigned long res = crc_calculate(&options_d, data_to_test);

    CU_ASSERT_EQUAL(res, OPT_D_EXPECTED);
}

void test_opt_e(){
    struct RastaByteArray data_to_test;
    allocateRastaByteArray(&data_to_test, 9);
    data_to_test.bytes = (unsigned char*)TEST_VAL;

    struct crc_options options_e = crc_init_opt_e();
    crc_generate_table(&options_e);

    unsigned long res = crc_calculate(&options_e, data_to_test);

    CU_ASSERT_EQUAL(res, OPT_E_EXPECTED);
}

void test_without_gen_table(){
    struct RastaByteArray data_to_test;
    allocateRastaByteArray(&data_to_test, 9);
    data_to_test.bytes = (unsigned char*)TEST_VAL;

    struct crc_options options_b = crc_init_opt_b();

    unsigned long res = crc_calculate(&options_b, data_to_test);

    CU_ASSERT_EQUAL(res, OPT_B_EXPECTED);
}