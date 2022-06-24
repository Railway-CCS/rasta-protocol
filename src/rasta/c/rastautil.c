#include <stdlib.h>
#include <time.h>
#include "rastautil.h"
#include <endian.h>


uint32_t current_ts(){
    long ms;
    time_t s;
    struct timespec spec;

    clock_gettime(CLOCK_MONOTONIC, &spec);

    s = spec.tv_sec;

    // seconds to milliseconds
    ms = s * 1000;

    // nanoseconds to milliseconds
    ms += (long)(spec.tv_nsec / 1.0e6);

    return (uint32_t)ms;
}

void freeRastaByteArray(struct RastaByteArray* data) {
    data->length = 0;
    free(data->bytes);
}


void allocateRastaByteArray(struct RastaByteArray* data, unsigned int length) {
    data->bytes = malloc(length);
    data->length = length;
}

int isBigEndian() {
    /*unsigned short t = 0x0102;
    return (t & 0xFF) == 0x02 ? 1 : 0;*/
    int i = 1;
    return ! *((char *) &i);
}

void hostLongToLe(uint32_t v, unsigned char* result) {
    uint32_t *target = (uint32_t *) result;
    *target = htole32(v);
}

uint32_t leLongToHost(const unsigned char v[4]) {
    uint32_t *result = (uint32_t*) v;
    return le32toh(*result);
}