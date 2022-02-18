//
// Created by Johannes on 29.11.2017.
//

#include <CUnit/Basic.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <rastamd4.h>
#include <rmemory.h>
#include "../headers/rastamd4Test.h"

void testMD4function(){

    unsigned char MD4result1[8];

    unsigned char MD4result2[8];


    srand(time(NULL));
    unsigned int len = rand() % 3 + 30;

    unsigned char* data;
    data = rmalloc(len);

    for (int i = 0; i < len; i++) {
        data[i] = rand() % 255;
    }

    generateMD4(data, len, 1, MD4result1);
    generateMD4(data, len, 1, MD4result2);

    for (int i = 0; i < 8; i++) {
        CU_ASSERT_EQUAL(MD4result1[i], MD4result2[i]);
    }

    unsigned char MD4result3[16];

    unsigned char MD4result4[16];

    generateMD4(data, len, 2, MD4result3);
    generateMD4(data, len, 2, MD4result4);

    rfree(data);

    for (int i = 0; i < 16; i++) {
        CU_ASSERT_EQUAL(MD4result3[i], MD4result4[i]);
    }


    //Vergleichen ob überhaupt richtiger Hashwert erzeugt wird
    unsigned char MD4result5[16];

    // MD4("") = 31d6cfe0d16ae931b73c59d7e0c089c0 source: Test Suite https://tools.ietf.org/html/rfc1320
    unsigned char hashOfEmptyString[16] = {
        0x31,
        0xd6,
        0xcf,
        0xe0,
        0xd1,
        0x6a,
        0xe9,
        0x31,
        0xb7,
        0x3c,
        0x59,
        0xd7,
        0xe0,
        0xc0,
        0x89,
        0xc0
    };

    generateMD4("", 0, 2, MD4result5);

    for (int i = 0; i < 16; i++) {
        CU_ASSERT_EQUAL(MD4result5[i], hashOfEmptyString[i]);
    }
}

void testRastaMD4Sample() {

    //example data
    unsigned char data[28];

    data[0] = 0x24;
    data[1] = 0x00;
    data[2] = 0x4c;
    data[3] = 0x18;
    data[4] = 0x3f;
    data[5] = 0xb4;
    data[6] = 0x96;
    data[7] = 0x00;
    data[8] = 0xce;
    data[9] = 0xca;
    data[10] = 0x23;
    data[11] = 0x00;
    data[12] = 0x56;
    data[13] = 0x44;
    data[14] = 0x33;
    data[15] = 0x22;
    data[16] = 0x66;
    data[17] = 0x55;
    data[18] = 0x44;
    data[19] = 0x33;
    data[20] = 0x57;
    data[21] = 0x01;
    data[22] = 0x00;
    data[23] = 0x00;
    data[24] = 0xcb;
    data[25] = 0x00;
    data[26] = 0x00;
    data[27] = 0x00;

    //correct full MD4
    unsigned char md4[16];

    md4[0] = 0x83;
    md4[1] = 0xf0;
    md4[2] = 0xd0;
    md4[3] = 0x52;
    md4[4] = 0x40;
    md4[5] = 0x6b;
    md4[6] = 0xf4;
    md4[7] = 0x92;
    md4[8] = 0xf8;
    md4[9] = 0x9f;
    md4[10] = 0x8d;
    md4[11] = 0x1e;
    md4[12] = 0x9b;
    md4[13] = 0x89;
    md4[14] = 0xc9;
    md4[15] = 0x8d;

    //std context

    MD4_CONTEXT context = md4InitContext(0x67452301,0xefcdab89,0x98badcfe,0x10325476);

    //holder for md4
    unsigned char calc_md4[16];

    //generate half md4
    generateMD4WithVector(data,28,1,&context,calc_md4);

    //check half
    for (int i = 0; i < 8; i++) {
        CU_ASSERT_EQUAL(md4[i],calc_md4[i]);
    }

    context = md4InitContext(0x67452301,0xefcdab89,0x98badcfe,0x10325476);
    //generate full md4
    generateMD4WithVector(data,28,2,&context,calc_md4);

    //check full
    for (int i = 0; i < 16; i++) {
        CU_ASSERT_EQUAL(md4[i],calc_md4[i]);
    }
}
