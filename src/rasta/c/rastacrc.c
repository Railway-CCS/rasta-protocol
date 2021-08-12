#include "rastacrc.h"


/**
 * reflects the lower @p n bits
 * @param crc_in the crc input value
 * @param n the number of bits that will be reflected
 * @return the calculated value
 */
unsigned long reflect (unsigned long crc_in, int n) {
    unsigned long j=1;
    unsigned long crc_out=0;

    for (unsigned long i=(unsigned long)1<<(n-1); i; i>>=1) {
        if (crc_in & i){
            crc_out|=j;
        }
        j<<= 1;
    }
    return (crc_out);
}

struct crc_options crc_init_opt_a(){
    struct crc_options options;
    options.width = 0;

    options.is_table_generated = 0;

    return options;
}

struct crc_options crc_init_opt_b(){
    struct crc_options options;

    options.width = 32;
    options.polynom = 0xEE5B42FD;
    options.initial = 0;
    options.initial_optimized = 0;
    options.refin = 0;
    options.refout = 0;
    options.final_xor = 0;

    //set mask and high bit
    options.crc_mask = ((((unsigned long)1<<(options.width-1))-1)<<1)|1;
    options.crc_high_bit = (unsigned long)1<<(options.width-1);


    options.is_table_generated=0;

    return options;
}

struct crc_options crc_init_opt_c(){
    struct crc_options options;

    options.width = 32;
    options.polynom = 0x1EDC6F41;
    options.initial = 0x2A26F826;
    options.initial_optimized = 0xFFFFFFFF;
    options.refin = 1;
    options.refout = 1;
    options.final_xor = 0xFFFFFFFF;

    //set mask and high bit
    options.crc_mask = ((((unsigned long)1<<(options.width-1))-1)<<1)|1;
    options.crc_high_bit = (unsigned long)1<<(options.width-1);


    options.is_table_generated=0;

    return options;
}

struct crc_options crc_init_opt_d(){
    struct crc_options options;

    options.width = 16;
    options.polynom = 0x1021;
    options.initial = 0;
    options.initial_optimized = 0;
    options.refin = 1;
    options.refout = 1;
    options.final_xor = 0;

    //set mask and high bit
    options.crc_mask = ((((unsigned long)1<<(options.width-1))-1)<<1)|1;
    options.crc_high_bit = (unsigned long)1<<(options.width-1);


    options.is_table_generated=0;

    return options;
}

struct crc_options crc_init_opt_e(){
    struct crc_options options;

    options.width = 16;
    options.polynom = 0x8005;
    options.initial = 0;
    options.initial_optimized = 0;
    options.refin = 1;
    options.refout = 1;
    options.final_xor = 0;

    //set mask and high bit
    options.crc_mask = ((((unsigned long)1<<(options.width-1))-1)<<1)|1;
    options.crc_high_bit = (unsigned long)1<<(options.width-1);


    options.is_table_generated=0;

    return options;
}


void crc_generate_table(struct crc_options * options) {
    // generate table
    unsigned long bit;

    for (int i=0; i<256; i++) {

        unsigned long crc=(unsigned long)i;
        if (options->refin){
            crc=reflect(crc, 8);
        }
        crc<<= options->width-8;

        for (int j=0; j<8; j++) {
            bit = crc & options->crc_high_bit;
            crc<<= 1;
            if (bit) crc^= options->polynom;
        }

        if (options->refin){
            crc = reflect(crc, options->width);
        }
        crc&= options->crc_mask;
        options->table[i]= crc;
    }

    options->is_table_generated = 1;
}
unsigned long crc_calculate (struct crc_options * options, struct RastaByteArray data) {
    if (!options->is_table_generated){
        crc_generate_table(options);
    }

    unsigned long crc = options->initial_optimized;

    if (options->refin){
        crc = reflect(crc, options->width);
    }

    if (!options->refin){
        while (data.length--){
            crc = (crc << 8) ^ options->table[ ((crc >> (options->width-8)) & 0xff) ^ *data.bytes++];
        }
    }
    else{
        while (data.length--){
            crc = (crc >> 8) ^ options->table[ (crc & 0xff) ^ *data.bytes++];
        }
    }

    if (options->refout ^ options->refin){
        crc = reflect(crc, options->width);
    }
    crc^= options->final_xor;
    crc&= options->crc_mask;

    return crc;
}