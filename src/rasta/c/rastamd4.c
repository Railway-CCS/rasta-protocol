//
// Created by Johannes on 27.11.2017.
//

#include "rastamd4.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*
 * The basic MD4 functions.
 */
#define F(x, y, z)			((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)			(((x) & ((y) | (z))) | ((y) & (z)))
#define H(x, y, z)			((x) ^ (y) ^ (z))

/*
 * The MD4 transformation for all three rounds.
 */
#define STEP(f, a, b, c, d, x, s) \
	(a) += f((b), (c), (d)) + (x); \
	(a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));

#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define SET(n) \
	(*(MD4_u32plus *)&ptr[(n) * 4])
#define GET(n) \
	SET(n)
#else
#define SET(n) \
	(ctx->block[(n)] = \
	(MD4_u32plus)ptr[(n) * 4] | \
	((MD4_u32plus)ptr[(n) * 4 + 1] << 8) | \
	((MD4_u32plus)ptr[(n) * 4 + 2] << 16) | \
	((MD4_u32plus)ptr[(n) * 4 + 3] << 24))
#define GET(n) \
	(ctx->block[(n)])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update the bit
 * counters.
 */
static const void *body(MD4_CTX_RASTA *ctx, const void *data, unsigned long size)
{
    const unsigned char *ptr;
    MD4_u32plus a, b, c, d;
    MD4_u32plus saved_a, saved_b, saved_c, saved_d;
    const MD4_u32plus ac1 = 0x5a827999, ac2 = 0x6ed9eba1;

    ptr = (const unsigned char *)data;

    a = ctx->a;
    b = ctx->b;
    c = ctx->c;
    d = ctx->d;

    do {
        saved_a = a;
        saved_b = b;
        saved_c = c;
        saved_d = d;

/* Round 1 */
        STEP(F, a, b, c, d, SET(0), 3)
        STEP(F, d, a, b, c, SET(1), 7)
        STEP(F, c, d, a, b, SET(2), 11)
        STEP(F, b, c, d, a, SET(3), 19)
        STEP(F, a, b, c, d, SET(4), 3)
        STEP(F, d, a, b, c, SET(5), 7)
        STEP(F, c, d, a, b, SET(6), 11)
        STEP(F, b, c, d, a, SET(7), 19)
        STEP(F, a, b, c, d, SET(8), 3)
        STEP(F, d, a, b, c, SET(9), 7)
        STEP(F, c, d, a, b, SET(10), 11)
        STEP(F, b, c, d, a, SET(11), 19)
        STEP(F, a, b, c, d, SET(12), 3)
        STEP(F, d, a, b, c, SET(13), 7)
        STEP(F, c, d, a, b, SET(14), 11)
        STEP(F, b, c, d, a, SET(15), 19)

/* Round 2 */
        STEP(G, a, b, c, d, GET(0) + ac1, 3)
        STEP(G, d, a, b, c, GET(4) + ac1, 5)
        STEP(G, c, d, a, b, GET(8) + ac1, 9)
        STEP(G, b, c, d, a, GET(12) + ac1, 13)
        STEP(G, a, b, c, d, GET(1) + ac1, 3)
        STEP(G, d, a, b, c, GET(5) + ac1, 5)
        STEP(G, c, d, a, b, GET(9) + ac1, 9)
        STEP(G, b, c, d, a, GET(13) + ac1, 13)
        STEP(G, a, b, c, d, GET(2) + ac1, 3)
        STEP(G, d, a, b, c, GET(6) + ac1, 5)
        STEP(G, c, d, a, b, GET(10) + ac1, 9)
        STEP(G, b, c, d, a, GET(14) + ac1, 13)
        STEP(G, a, b, c, d, GET(3) + ac1, 3)
        STEP(G, d, a, b, c, GET(7) + ac1, 5)
        STEP(G, c, d, a, b, GET(11) + ac1, 9)
        STEP(G, b, c, d, a, GET(15) + ac1, 13)

/* Round 3 */
        STEP(H, a, b, c, d, GET(0) + ac2, 3)
        STEP(H, d, a, b, c, GET(8) + ac2, 9)
        STEP(H, c, d, a, b, GET(4) + ac2, 11)
        STEP(H, b, c, d, a, GET(12) + ac2, 15)
        STEP(H, a, b, c, d, GET(2) + ac2, 3)
        STEP(H, d, a, b, c, GET(10) + ac2, 9)
        STEP(H, c, d, a, b, GET(6) + ac2, 11)
        STEP(H, b, c, d, a, GET(14) + ac2, 15)
        STEP(H, a, b, c, d, GET(1) + ac2, 3)
        STEP(H, d, a, b, c, GET(9) + ac2, 9)
        STEP(H, c, d, a, b, GET(5) + ac2, 11)
        STEP(H, b, c, d, a, GET(13) + ac2, 15)
        STEP(H, a, b, c, d, GET(3) + ac2, 3)
        STEP(H, d, a, b, c, GET(11) + ac2, 9)
        STEP(H, c, d, a, b, GET(7) + ac2, 11)
        STEP(H, b, c, d, a, GET(15) + ac2, 15)

        a += saved_a;
        b += saved_b;
        c += saved_c;
        d += saved_d;

        ptr += 64;
    } while (size -= 64);

    ctx->a = a;
    ctx->b = b;
    ctx->c = c;
    ctx->d = d;

    return ptr;
}

void MD4_Init_Rasta(MD4_CTX_RASTA *ctx)
{
    ctx->a = 0x67452301;
    ctx->b = 0xefcdab89;
    ctx->c = 0x98badcfe;
    ctx->d = 0x10325476;

    ctx->lo = 0;
    ctx->hi = 0;
}

void MD4_Update_Rasta(MD4_CTX_RASTA *ctx, const void *data, unsigned long size)
{
    MD4_u32plus saved_lo;
    unsigned long used, available;

    saved_lo = ctx->lo;
    if ((ctx->lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
        ctx->hi++;
    ctx->hi += size >> 29;

    used = saved_lo & 0x3f;

    if (used) {
        available = 64 - used;

        if (size < available) {
            memcpy(&ctx->buffer[used], data, size);
            return;
        }

        memcpy(&ctx->buffer[used], data, available);
        data = (const unsigned char *)data + available;
        size -= available;
        body(ctx, ctx->buffer, 64);
    }

    if (size >= 64) {
        data = body(ctx, data, size & ~(unsigned long)0x3f);
        size &= 0x3f;
    }

    memcpy(ctx->buffer, data, size);
}

#define OUT(dst, src) \
	(dst)[0] = (unsigned char)(src); \
	(dst)[1] = (unsigned char)((src) >> 8); \
	(dst)[2] = (unsigned char)((src) >> 16); \
	(dst)[3] = (unsigned char)((src) >> 24);

void MD4_Final_Rasta(unsigned char *result, MD4_CTX_RASTA *ctx)
{
    unsigned long used, available;

    used = ctx->lo & 0x3f;

    ctx->buffer[used++] = 0x80;

    available = 64 - used;

    if (available < 8) {
        memset(&ctx->buffer[used], 0, available);
        body(ctx, ctx->buffer, 64);
        used = 0;
        available = 64;
    }

    memset(&ctx->buffer[used], 0, available - 8);

    ctx->lo <<= 3;
    OUT(&ctx->buffer[56], ctx->lo)
    OUT(&ctx->buffer[60], ctx->hi)

    body(ctx, ctx->buffer, 64);

    OUT(&result[0], ctx->a)
    OUT(&result[4], ctx->b)
    OUT(&result[8], ctx->c)
    OUT(&result[12], ctx->d)

    memset(ctx, 0, sizeof(*ctx));
}

MD4_CONTEXT md4InitContext (MD4_u32plus a, MD4_u32plus b, MD4_u32plus c, MD4_u32plus d){
#ifdef USE_OPENSSL
    MD4_CTX context;
    MD4_Init(&context);
    // setting IV does not work with openssl, ignore it
#else
    MD4_CTX_RASTA context;
    context.a = a;
    context.b = b;
    context.c = c;
    context.d = d;

    context.hi = 0;
    context.lo = 0;
#endif

    return context;
}

void generateMD4(unsigned char* data, int length, int type, unsigned char* result) {
    MD4_CONTEXT context;
#ifdef USE_OPENSSL
    MD4_Init (&context);
#else
    MD4_Init_Rasta(&context);
#endif

    generateMD4WithVector(data, length, type, &context, result);
}

void generateMD4WithVector(unsigned char* data, int length, int type, MD4_CONTEXT* context, unsigned char* result) {
    unsigned char MD4code[type == 2 ? 16 : 8];
#ifdef USE_OPENSSL
    MD4_Update (context, data, length);
    MD4_Final (MD4code, context);
#else
    MD4_Update_Rasta(context, data, length);
    MD4_Final_Rasta(MD4code, context);
#endif

    switch(type) {
        // 5.3.11 Sicherheitscode
        // kein Sicherheitscode
        case 0:
            for (int i = 0; i < 8; i++) {
                result[i] = 0;
            }
            break;
            // untere Hälfte des MD4
        case 1:
            for (int i = 0; i < 8; i++) {
                result[i] = (unsigned char) MD4code[i];
            }
            break;
            // volle MD4
        case 2:
            for (int i = 0; i < 16; i++) {
                result[i] = (unsigned char) MD4code[i];
            }
            break;
        default:
            return;
    }
}