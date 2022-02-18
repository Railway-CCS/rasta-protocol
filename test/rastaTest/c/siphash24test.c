#include "../headers/siphash24test.h"
#include <rastasiphash24.h>
#include <CUnit/Basic.h>
#include <rmemory.h>

size_t lengths[4] = {8, 16, 4, 8};


void testSipHash24(){
    uint8_t in[64], out[16], k[16];
    int i;
    int fails = 0;

    for (i = 0; i < 16; ++i)
        k[i] = i;

    for (int version = 0; version < 2; ++version) {
        for (i = 0; i < 64; ++i) {
            in[i] = i;
            int len = lengths[version];
            if (version < 2){
                siphash(in, i, k, out, len);
            } else{
                halfsiphash(in, i, k, out, len);
            }
            const uint8_t *v = NULL;
            switch (version) {
                case 0:
                    v = (uint8_t *)vectors_sip64;
                    break;
                case 1:
                    v = (uint8_t *)vectors_sip128;
                    break;
                case 2:
                    v = (uint8_t *)vectors_hsip32;
                    break;
                case 3:
                    v = (uint8_t *)vectors_hsip64;
                    break;
                default:
                    break;
            }

            if (memcmp(out, v + (i * len), len)) {
                printf("fail for %d bytes\n", i);
                fails++;
            }
        }

        CU_ASSERT_EQUAL(0, fails);
        fails = 0;
    }
}