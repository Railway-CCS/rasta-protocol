//
// Created by tobia on 18.12.2017.
//

#ifndef LST_SIMULATOR_MEMORY_H
#define LST_SIMULATOR_MEMORY_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

/**
 * Allocates memory of size
 * @param size the size of the memory
 * @return pointer to memory
 */
void * rmalloc(unsigned int size);


/**
 * reallocates memory for element with size
 * @param element
 * @param size
 * @return
 */
void * rrealloc(void* element, unsigned int size);
/**
 * frees an allocated memory
 * @param element pointer to the memory
 */
void rfree(void * element);

/**
 * Copies the first n characters from src to dest
 * @param dest
 * @param src
 * @param n
 * @return
 */
void* rmemcpy(void *dest, const void *src, unsigned int n);


/**
 * sets tje first n characters in dest to ch
 * @param dest
 * @param ch
 * @param n
 * @return
 */
void* rmemset(void *dest, int ch, unsigned int n);

/**
 * Copies the C string pointed by source into the array pointed by destination, including the terminating null character (and stopping at that point).
 * @param dest
 * @param src
 */
void rstrcpy(char * dest, const char * src);

/**
 * Appends a copy of the source string to the destination string.
 * The terminating null character in destination is overwritten by the first character of source, and a null-character is included at the end of the new string formed by the concatenation of both in destination.
 * @param dest
 * @param src
 */
void rstrcat(char * dest, const char * src);

/**
 * Compares the first len bytes of values a and b
 * @param a value a
 * @param b value b
 * @param len number of bytes to compare
 * @return < 0 if if a < b, > 0 if a > b or 0 if a == b
 */
int rmemcmp(const  void * a, const void * b, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_MEMORY_H
