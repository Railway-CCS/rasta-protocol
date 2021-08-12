//
// Created by tobia on 15.01.2018.
//

#include <stdio.h>
#include "../headers/configtest.h"
#include <CUnit/Basic.h>
#include "config.h"
#include "string.h"

void check_std_config() {
    //remove old file
    remove("config.cfg");

    //write empty file
    FILE *f = fopen("config.cfg", "w");
    fclose(f);

    //load the empty file as config
    struct RastaConfig cfg = config_load("config.cfg");

    //check that there are no entries in dictionary
    CU_ASSERT_EQUAL(cfg.dictionary.size,0);

    //check sending standarts
    CU_ASSERT_EQUAL(cfg.values.sending.t_max,1800);
    CU_ASSERT_EQUAL(cfg.values.sending.t_h,300);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_type, RASTA_CHECKSUM_8B);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_a,0x67452301);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_b,0xefcdab89);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_c,0x98badcfe);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_d,0x10325476);
    CU_ASSERT_EQUAL(cfg.values.sending.send_max, 20);
    CU_ASSERT_EQUAL(cfg.values.sending.mwa, 10);
    CU_ASSERT_EQUAL(cfg.values.sending.max_packet, 3);
    CU_ASSERT_EQUAL(cfg.values.sending.diag_window, 5000);

    //check redundancy
    CU_ASSERT_EQUAL(cfg.values.redundancy.connections.count,0);
    CU_ASSERT_EQUAL(cfg.values.redundancy.crc_type.width,0);
    CU_ASSERT_EQUAL(cfg.values.redundancy.t_seq, 100);
    CU_ASSERT_EQUAL(cfg.values.redundancy.n_diagnose, 200);
    CU_ASSERT_EQUAL(cfg.values.redundancy.n_deferqueue_size, 4);

    //cechk general
    CU_ASSERT_EQUAL(cfg.values.general.rasta_network,0);
    CU_ASSERT_EQUAL(cfg.values.general.rasta_id,0);


}

void check_var_config() {
    //remove old file
    remove("config.cfg");

    //write empty file
    FILE *f = fopen("config.cfg", "w");

    fprintf(f, "RASTA_T_MAX = 1700\n");
    fprintf(f,"RASTA_T_H = 200\n");
    fprintf(f,"RASTA_MD4_TYPE = FULL\n");
    fprintf(f,"RASTA_SEND_MAX = 25\n");
    fprintf(f,"RASTA_MWA = 15\n");
    fprintf(f,"RASTA_MAX_PACKET = 4\n");
    fprintf(f,"RASTA_DIAG_WINDOW = 6000\n");

    fprintf(f,"RASTA_REDUNDANCY_CONNECTIONS = {\"192.168.2.1:8000\"; \"83.23.1.2:40\"}\n");
    fprintf(f,"RASTA_CRC_TYPE = TYPE_C\n");
    fprintf(f,"RASTA_T_SEQ = 50\n");
    fprintf(f,"RASTA_N_DIAGNOSE = 100\n");
    fprintf(f,"RASTA_N_DEFERQUEUE_SIZE = 2\n");
    fprintf(f,"RASTA_NETWORK = 1234\n");
    fprintf(f,"RASTA_ID = 2345\n");

    fprintf(f,"STRING = \"Test\"\n");
    fprintf(f,"ARRAY = {\"1st entry\"; \"2nd entry\"}\n");
    fprintf(f,"NUMBER = 23\n");
    fprintf(f,"NEGATIV = -23553\n");
    fprintf(f,"HEX = #ff32\n");


    fclose(f);

    struct RastaConfig cfg = config_load("config.cfg");

    //check sending standarts
    CU_ASSERT_EQUAL(cfg.values.sending.t_max,1700);
    CU_ASSERT_EQUAL(cfg.values.sending.t_h,200);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_type, RASTA_CHECKSUM_16B);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_a,0x67452301);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_b,0xefcdab89);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_c,0x98badcfe);
    CU_ASSERT_EQUAL(cfg.values.sending.md4_d,0x10325476);
    CU_ASSERT_EQUAL(cfg.values.sending.send_max, 25);
    CU_ASSERT_EQUAL(cfg.values.sending.mwa, 15);
    CU_ASSERT_EQUAL(cfg.values.sending.max_packet, 4);
    CU_ASSERT_EQUAL(cfg.values.sending.diag_window, 6000);

    //check redundancy
    CU_ASSERT_EQUAL(cfg.values.redundancy.connections.count,2);

    CU_ASSERT_EQUAL(strcmp(cfg.values.redundancy.connections.data[0].ip, "192.168.2.1"),0);
    CU_ASSERT_EQUAL(cfg.values.redundancy.connections.data[0].port, 8000);
    CU_ASSERT_EQUAL(strcmp(cfg.values.redundancy.connections.data[1].ip, "83.23.1.2"),0);
    CU_ASSERT_EQUAL(cfg.values.redundancy.connections.data[1].port, 40);

    CU_ASSERT_EQUAL(cfg.values.redundancy.crc_type.width,32);
    CU_ASSERT_EQUAL(cfg.values.redundancy.crc_type.polynom,0x1EDC6F41);

    CU_ASSERT_EQUAL(cfg.values.redundancy.t_seq, 50);
    CU_ASSERT_EQUAL(cfg.values.redundancy.n_diagnose, 100);
    CU_ASSERT_EQUAL(cfg.values.redundancy.n_deferqueue_size, 2);

    //cechk general
    CU_ASSERT_EQUAL(cfg.values.general.rasta_network,1234);
    CU_ASSERT_EQUAL(cfg.values.general.rasta_id,2345);

    //check custom value
    struct DictionaryEntry entr;
    entr = config_get(&cfg,"STRING");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_STRING);
    CU_ASSERT_EQUAL(strcmp(entr.value.string.c, "Test"),0);

    entr = config_get(&cfg,"ARRAY");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_ARRAY);
    CU_ASSERT_EQUAL(entr.value.array.count,2);
    CU_ASSERT_EQUAL(strcmp(entr.value.array.data[0].c, "1st entry"),0);
    CU_ASSERT_EQUAL(strcmp(entr.value.array.data[1].c, "2nd entry"),0);

    entr = config_get(&cfg,"NUMBER");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_NUMBER);
    CU_ASSERT_EQUAL(entr.value.number, 23);

    entr = config_get(&cfg,"NEGATIV");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_NUMBER);
    CU_ASSERT_EQUAL(entr.value.number, -23553);

    entr = config_get(&cfg,"HEX");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_NUMBER);
    CU_ASSERT_EQUAL(entr.value.number, 0xff32);


}