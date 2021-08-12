#ifndef RASTA_RASTASIPHASH24_H
#define RASTA_RASTASIPHASH24_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * generates a SipHash 2-4 hash for the given data data and saves it in result
 * @param data array of the data
 * @param data_length length of data
 * @param key the key for the SipHash function
 * @param key_length the length of the key
 * @param hash_type type of security code (0 means no code, 1 means first 8 bytes, 2 means first 16 bytes)
 * @param result array for the result
 */
void generateSiphash24(const unsigned char* data, int data_length, const unsigned char * key,  int hash_type, unsigned char* result);


int siphash(const uint8_t *in, size_t inlen, const uint8_t *k,
            uint8_t *out, size_t outlen);

int halfsiphash(const uint8_t *in, const size_t inlen, const uint8_t *k,
                uint8_t *out, const size_t outlen);

#endif //RASTA_RASTASIPHASH24_H
