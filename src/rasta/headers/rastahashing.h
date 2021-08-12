#ifndef RASTA_RASTAHASHING_H
#define RASTA_RASTAHASHING_H

#include <rastautil.h>
#include <rastamd4.h>
#include <rastablake2.h>
#include <rastasiphash24.h>


/**
 * Algorithms that can be used for the RaSTA SR layer checksum
 */
typedef enum {
    /**
     * MD4
     */
    RASTA_ALGO_MD4 = 0,
    /**
     * Blake2b
     */
    RASTA_ALGO_BLAKE2B = 1,
    /**
     * SipHash-2-4
     */
    RASTA_ALGO_SIPHASH_2_4 = 2
} rasta_hash_algorithm;

/**
 * used checksum type
 */
typedef enum {
    /**
     * no checksum
     */
    RASTA_CHECKSUM_NONE = 0,
    /**
     * 8 byte checksum
     */
    RASTA_CHECKSUM_8B = 1,
    /**
     * 16 byte checksum
     */
    RASTA_CHECKSUM_16B = 2
}rasta_checksum_type;

typedef struct rasta_hashing_ctx{
    /**
     * The hashing algorithm
     */
    rasta_hash_algorithm algorithm;
    /**
     * The length of the resulting hash
     */
    rasta_checksum_type hash_length;
    /**
     * The key / iv for the hashing algorithm
     */
    struct RastaByteArray key;
}rasta_hashing_context_t;

/**
 * Calculates a checksum over the given data using the parameters in the hashing context
 * @param data the data to hash
 * @param context the hashing context that contains the neccessary parameters for hashing the data
 * @param hash the resulting hash
 */
void rasta_calculate_hash(struct RastaByteArray data, rasta_hashing_context_t * context,  unsigned char * hash);

/**
 * Sets the key of the hashing context based the the MD4 initial value
 * @param context the context where the key is set
 * @param a A part of the initial MD4 value
 * @param b B part of the initial MD4 value
 * @param c C part of the initial MD4 value
 * @param d D part of the initial MD4 value
 */
void rasta_md4_set_key(rasta_hashing_context_t * context, MD4_u32plus a, MD4_u32plus b, MD4_u32plus c, MD4_u32plus d);

#endif //RASTA_RASTAHASHING_H
