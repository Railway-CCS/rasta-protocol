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
void longToBytes(uint32_t v, unsigned char* result);

/**
 * Converts a uchar array to a ulong
 * @param v the uchar array
 * @return the ulong
 */
uint32_t bytesToLong(const unsigned char v[4]);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTAUTIL_H
