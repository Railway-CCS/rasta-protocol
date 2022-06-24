#include "../headers/blake2test.h"
#include <rastablake2.h>
#include <CUnit/Basic.h>
#include <rmemory.h>

unsigned char* hexstr_to_char(const char* hexstr)
{
    size_t len = strlen(hexstr);
    size_t final_len = len / 2;
    unsigned char* chrs = (unsigned char*)malloc((final_len+1) * sizeof(*chrs));
    for (unsigned int i=0, j=0; j<final_len; i+=2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;
    chrs[final_len] = '\0';
    return chrs;
}



void testBlake2Hash(){
    // assure that blake itself is working correctly
    CU_ASSERT_EQUAL(0, blake2b_selftest());

    unsigned char * result = rmalloc(64 * sizeof(unsigned char));

    unsigned char * test_vect = hexstr_to_char("00");
    unsigned char* key = hexstr_to_char("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f");
    unsigned char* expected_hash = hexstr_to_char("961f6dd1e4dd30f63901690c512e78e4b45e4742ed197c3c5e45c549fd25f2e4187b0bc9fe30492b16b0d0bc4ef9b0f34c7003fac09a5ef1532e69430234cebd");

    generateBlake2(test_vect, 1 , key, 64, 8, result);

    CU_ASSERT_EQUAL(0, memcmp(expected_hash, result, 64));

    rfree(key);
    rfree(test_vect);
    rfree(expected_hash);
    rfree(result);
}

/**
 * Testing procedure copied from RFC 7693
 * https://tools.ietf.org/html/rfc7693
 */

static void selftest_seq(uint8_t *out, size_t len, uint32_t seed) {
    size_t i;
    uint32_t t, a, b;

    a = 0xDEAD4BAD * seed;              // prime
    b = 1;

    for (i = 0; i < len; i++) {         // fill the buf
        t = a + b;
        a = b;
        b = t;
        out[i] = (t >> 24) & 0xFF;
    }
}
// BLAKE2b self-test validation. Return 0 when OK.

int blake2b_selftest() {
    // grand hash of hash results
    const uint8_t blake2b_res[32] = {
            0xC2, 0x3A, 0x78, 0x00, 0xD9, 0x81, 0x23, 0xBD,
            0x10, 0xF5, 0x06, 0xC6, 0x1E, 0x29, 0xDA, 0x56,
            0x03, 0xD7, 0x63, 0xB8, 0xBB, 0xAD, 0x2E, 0x73,
            0x7F, 0x5E, 0x76, 0x5A, 0x7B, 0xCC, 0xD4, 0x75
    };
    // parameter sets
    const size_t b2b_md_len[4] = {20, 32, 48, 64};
    const size_t b2b_in_len[6] = {0, 3, 128, 129, 255, 1024};

    size_t i, j, outlen, inlen;
    uint8_t in[1024], md[64], key[64];
    blake2b_ctx ctx;

    // 256-bit hash for testing
    if (blake2b_init(&ctx, 32, NULL, 0))
        return -1;

    for (i = 0; i < 4; i++) {
        outlen = b2b_md_len[i];
        for (j = 0; j < 6; j++) {
            inlen = b2b_in_len[j];

            selftest_seq(in, inlen, inlen);     // unkeyed hash
            blake2b(md, outlen, NULL, 0, in, inlen);
            blake2b_update(&ctx, md, outlen);   // hash the hash

            selftest_seq(key, outlen, outlen);  // keyed hash
            blake2b(md, outlen, key, outlen, in, inlen);
            blake2b_update(&ctx, md, outlen);   // hash the hash
        }
    }

    // compute and compare the hash of hashes
    blake2b_final(&ctx, md);
    for (i = 0; i < 32; i++) {
        if (md[i] != blake2b_res[i])
            return -1;
    }

    return 0;
}
