#include <stdlib.h>
#include <time.h>
#include "rastautil.h"


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

void longToBytes(uint32_t v, unsigned char* result) {
    if (!isBigEndian()) {
        result[3] = (unsigned char) (v >> 24 & 0xFF);
        result[2] = (unsigned char) (v >> 16 & 0xFF);
        result[1] = (unsigned char) (v >> 8 & 0xFF);
        result[0] = (unsigned char) (v & 0xFF);
    }
    else {
        result[0] = (unsigned char) (v >> 24 & 0xFF);
        result[1] = (unsigned char) (v >> 16 & 0xFF);
        result[2] = (unsigned char) (v >> 8 & 0xFF);
        result[3] = (unsigned char) (v & 0xFF);
    }
}

uint32_t bytesToLong(const unsigned char v[4]) {
    uint32_t result = 0;
    if (!isBigEndian()) {
        result = (v[3] << 24) + (v[2] << 16) + (v[1] << 8) + v[0];
    }
    else {
        result = (v[0] << 24) + (v[1] << 16) + (v[2] << 8) + v[3];
    }
    return result;
}