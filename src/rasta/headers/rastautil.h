#ifndef LST_SIMULATOR_RASTAUTIL_H
#define LST_SIMULATOR_RASTAUTIL_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <stdint.h>

struct RastaByteArray {
    unsigned char* bytes;
    unsigned int length;
};

/**
 * Frees the bytes array and sets length to 0
 * @param data the byte array
 */
void freeRastaByteArray(struct RastaByteArray* data);

/**
 * Allocates space for the bytearray
 * @param data the data
 * @param length the length
 */
void allocateRastaByteArray(struct RastaByteArray* data, unsigned int length);

/**
 * this will generate a 4 byte timestamp of the current system time
 * @return current system time in s since 1970
 */
uint32_t current_ts();

int isBigEndian();

/**
 * Converts a unsigned long into a uchar array
 * @param v the uchar array
 * @param result the assigned uchar array; length should be 4
 */
void hostLongToLe(uint32_t v, unsigned char* result);

/**
 * Converts 4 little-endian bytes to a host ulong
 * @param v pointer to uint32_t in LE byte order
 * @return the ulong
 */
uint32_t leLongToHost(const unsigned char *v);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTAUTIL_H
