#ifndef RASTA_RASTABLAKE2_H
#define RASTA_RASTABLAKE2_H

#include <stdint.h>
#include <stddef.h>


/**
 * generates a BLAKE2 (blake2b) hash for the given data data and saves it in result
 * @param data array of the data
 * @param data_length length of data
 * @param key the key for the BLAKE2 function
 * @param key_length the length of the key
 * @param hash_type type of security code (0 means no code, 1 means first 8 bytes, 2 means first 16 bytes)
 * @param result array for the result
 */
void generateBlake2(unsigned char* data, int data_length, const unsigned char * key, int key_length,  int hash_type, unsigned char* result);


/*
 * Start of BLAKE2 implementation from RFC 7693
 * https://tools.ietf.org/html/rfc7693#page-16
 */

// state context
typedef struct {
    uint8_t b[128];                     // input buffer
    uint64_t h[8];                      // chained state
    uint64_t t[2];                      // total number of bytes
    size_t c;                           // pointer for b[]
    size_t outlen;                      // digest size
} blake2b_ctx;


// Initialize the hashing context "ctx" with optional key "key".
//      1 <= outlen <= 64 gives the digest size in bytes.
//      Secret key (also <= 64 bytes) is optional (keylen = 0).
int blake2b_init(blake2b_ctx *ctx, size_t outlen,
                 const void *key, size_t keylen);    // secret key

// Add "inlen" bytes from "in" into the hash.
void blake2b_update(blake2b_ctx *ctx,   // context
                    const void *in, size_t inlen);      // data to be hashed

// Generate the message digest (size given in init).
//      Result placed in "out".
void blake2b_final(blake2b_ctx *ctx, void *out);

// All-in-one convenience function.
int blake2b(void *out, size_t outlen,   // return buffer for digest
            const void *key, size_t keylen,     // optional secret key
            const void *in, size_t inlen);      // data to be hashed

#endif //RASTA_RASTABLAKE2_H
