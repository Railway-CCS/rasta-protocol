/**
 * This module provides functions to calculate the CRC checksum of a bytearray.
 * The CRC options as specified in 6.3.6 can be generated using the functions
 * crc_init_opt_b, crc_init_opt_c, crc_init_opt_d, crc_init_opt_e
 *
 * Example:
 *
 *      struct RastaByteArray data;
 *      allocateRastaByteArray(&data, 9);
 *      data.bytes = (unsigned char*)"123456789";
 *
 *      // initialize the options as in 6.3.6 b)
 *      struct crc_options options_b = crc_init_opt_b();
 *
 *      // pre generate the crc table, this is optional but recommended if not called the table will be generated when
 *      // the checksum is calculated, thus making the first computation slower
 *      crc_generate_table(&options_b);
 *
 *      // calculate the checksum
 *      unsigned long res = crc_calculate(&options_b, data);
 *
 *      // res has value 0x0E7C650A now
 */

#ifndef LST_SIMULATOR_RASTACRC_H
#define LST_SIMULATOR_RASTACRC_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include "rastautil.h"

/**
 * representation of the options the the crc algorithm will use
 */
struct crc_options{
    /**
     * length of crc in bit
     */
    unsigned short width;
    /*
     * the crc polynom without msb
     */
    unsigned long polynom;
    /**
     * the initial value (currently unused)
     */
    unsigned long initial;
    /**
     * the initial value for the table lookup algorithm
     */
    unsigned long initial_optimized;
    /**
     * 0 if reflected input is disabled, 1 otherwise
     */
    int refin;
    /**
     * 0 if reflected output is disabled, 1 otherwise
     */
    int refout;
    /**
     * value for the final xor operation, hast to be the same length as width
     */
    unsigned long final_xor;

    /**
     * mask for internal crc computation, do not set
     */
    unsigned long crc_mask;
    /**
     * mask for internal crc computation, do not set
     */
    unsigned long crc_high_bit;

    /*
     * 1 if the crc lookup table has been generated, 0 otherwise
     */
    int is_table_generated;
    /**
     * the precomputed crc lookup table, generate by calling 'crc_generate_table'
     */
    unsigned long table[256];
};

/**
 * initializes crc options as in 6.3.6 a)
 * no checksum is used (width = 0)
 * @return configured crc_options struct
 */
struct crc_options crc_init_opt_a();

/**
 * initializes crc options as in 6.3.6 b)
 * width=32, polynom=0xEE5B42FD, initial=initial_optimized=0, refin=refout=0, final_xor=0
 * @return configured crc_options struct
 */
struct crc_options crc_init_opt_b();

/**
 * initializes crc options as in 6.3.6 c)
 * width=32, polynom=0x1EDC6F41, initial=0x2A26F826, initial_optimized=0xFFFFFFFF, refin=refout=1, final_xor=0xFFFFFFFF
 * @return configured crc_options struct
 */
struct crc_options crc_init_opt_c();

/**
 * initializes crc options as in 6.3.6 d)
 * width=16, polynom=0x1021, initial=initial_optimized=0, refin=refout=1, final_xor=0
 * @return configured crc_options struct
 */
struct crc_options crc_init_opt_d();

/**
 * initializes crc options as in 6.3.6 e)
 * width=16, polynom=0x8005, initial=initial_optimized=0, refin=refout=1, final_xor=0
 * @return configured crc_options struct
 */
struct crc_options crc_init_opt_e();

/**
 * generate the crc lookup table for the given @p options
 * @param options the options which the table is generated for
 */
void crc_generate_table(struct crc_options * options);

/**
 * calculates the crc of the given @p data with the given @p options
 * if the crc lookup table has not been generated yet, it will be generated first.
 * @param options the options which are used
 * @param data the data which's checksum is calculated
 * @return the calculated checksum
 */
unsigned long crc_calculate (struct crc_options * options, struct RastaByteArray data);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTACRC_H