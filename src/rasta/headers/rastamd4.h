//
// source of implementation : http://openwall.info/wiki/people/solar/software/public-domain-source-code/md4
//

#ifndef LST_SIMULATOR_RASTAMD4_H
#define LST_SIMULATOR_RASTAMD4_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

/* Any 32-bit or wider unsigned integer data type will do */
typedef unsigned int MD4_u32plus;

typedef struct {
    MD4_u32plus lo, hi;
    MD4_u32plus a, b, c, d;
    unsigned char buffer[64];
    MD4_u32plus block[16];
} MD4_CTX_RASTA;

#ifdef USE_OPENSSL
#include <openssl/md4.h>
#define MD4_CONTEXT MD4_CTX
#else
#define MD4_CONTEXT MD4_CTX_RASTA
#endif

/**
 * initializes a MD4 context with given initialization values
 * @param a the a word of the initial value
 * @param b the b word of the initial value
 * @param c the c word of the initial value
 * @param d the d word of the initial value
 * @return
 */
MD4_CONTEXT md4InitContext (MD4_u32plus a, MD4_u32plus b, MD4_u32plus c, MD4_u32plus d);

/**
 * generates MD4-Hash for data and saves it in result
 * @param data array of the data
 * @param length of data
 * @param type of security code (0 means no code, 1 means half the code, 2 all of the code)
 * @param result array for the result
 */
void generateMD4(unsigned char* data, int length, int type, unsigned char* result);

/**
 * generates MD4-Hash for data and saves it in result
 * @param data array of the data
 * @param length of data
 * @param type of security code (0 means no code, 1 means half the code, 2 all of the code)
 * @param result array for the result
 */
void generateMD4WithVector(unsigned char* data, int length, int type, MD4_CONTEXT* context, unsigned char* result);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_RASTAMD4_H