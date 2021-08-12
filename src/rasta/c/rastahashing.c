#include "rastahashing.h"
#include <rmemory.h>

void rasta_md4_set_key(rasta_hashing_context_t * context, MD4_u32plus a, MD4_u32plus b, MD4_u32plus c, MD4_u32plus d){
    // MD4 IV has length 4*4 bytes
    allocateRastaByteArray(&context->key, 16);
    unsigned char buffer[4];

    // write a, b, c, d to key
    longToBytes(a, buffer);
    rmemcpy(context->key.bytes, buffer, 4 * sizeof(unsigned char));

    longToBytes(b, buffer);
    rmemcpy(&context->key.bytes[4], buffer, 4 * sizeof(unsigned char));

    longToBytes(c, buffer);
    rmemcpy(&context->key.bytes[8], buffer, 4 * sizeof(unsigned char));

    longToBytes(d, buffer);
    rmemcpy(&context->key.bytes[12], buffer, 4 * sizeof(unsigned char));
}

MD4_CONTEXT rasta_get_md4_ctx_from_key(rasta_hashing_context_t * context){
    unsigned char buffer[4];
    MD4_u32plus a, b, c, d;

    // read a, b, c, d from key
    rmemcpy(&buffer, context->key.bytes, 4);
    a = bytesToLong(buffer);

    rmemcpy(&buffer, &context->key.bytes[4], 4);
    b = bytesToLong(buffer);

    rmemcpy(&buffer, &context->key.bytes[8], 4);
    c = bytesToLong(buffer);

    rmemcpy(&buffer, &context->key.bytes[12], 4);
    d = bytesToLong(buffer);

    return md4InitContext(a, b, c, d);
}

void rasta_calculate_hash(struct RastaByteArray data, rasta_hashing_context_t * context,  unsigned char * hash){
    MD4_CONTEXT md4_ctx;
    switch (context->algorithm){
        case RASTA_ALGO_MD4:
            md4_ctx = rasta_get_md4_ctx_from_key(context);
            generateMD4WithVector(data.bytes, data.length, context->hash_length,  &md4_ctx, hash);
            break;
        case RASTA_ALGO_BLAKE2B:
            generateBlake2(data.bytes, data.length, context->key.bytes, context->key.length, context->hash_length, hash);
            break;
        case RASTA_ALGO_SIPHASH_2_4:
            generateSiphash24(data.bytes, data.length, context->key.bytes, context->hash_length, hash);
            break;
        default:
            // just use MD4
            md4_ctx = rasta_get_md4_ctx_from_key(context);
            generateMD4WithVector(data.bytes, data.length, context->hash_length,  &md4_ctx, hash);
            break;
    }
}
