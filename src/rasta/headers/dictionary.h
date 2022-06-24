//
// Created by tobia on 18.12.2017.
//

#ifndef LST_SIMULATOR_DICTIONARY_H
#define LST_SIMULATOR_DICTIONARY_H

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

/**
 * Defines the type of an entry
 */
typedef enum {
    DICTIONARY_STRING,
    DICTIONARY_ARRAY,
    DICTIONARY_NUMBER,
    DICTIONARY_ERROR
}dic_entry_type;

/**
 * represents a string
 */
struct DictionaryString {
    char c[256];
};

/**
 * represents an array
 */
struct DictionaryArray {
    struct DictionaryString* data;
    unsigned int count;
};

/**
 * represents the value
 */
union DictionaryValue {
    unsigned int unumber;
    int number;
    struct DictionaryString string;
    struct DictionaryArray array;
};

/**
 * represents an entry
 */
struct DictionaryEntry {
    dic_entry_type type;
    char key[256];
    union DictionaryValue value;
};

/**
 * structure for the dictionary
 */
struct Dictionary {
    unsigned int size;
    unsigned int actual_size;

    struct DictionaryEntry* data;

};
/**
 * returns an allocated DictionaryArray
 * @param size
 * @return
 */
struct DictionaryArray allocate_DictionaryArray(unsigned int size);

/**
 * reallocates an DictionaryArray
 * @param array
 * @param new_size
 */
void reallocate_DictionaryArray(struct DictionaryArray* array, unsigned int new_size);
/**
 * frees an DictionaryArray
 * @param array
 */
void free_DictionaryArray(struct DictionaryArray* array);

/**
 * creates a dictionary with the initial size
 * @param initial_size size should normally be set to 2
 * @return to initialized dictionary
 */
struct Dictionary dictionary_create(unsigned int initial_size);

/**
 * frees the dictionary and all data
 * @param dict
 */
void dictionary_free(struct Dictionary* dict);
/**
 * checks if the dictionary contains an entry with key
 * @param dict
 * @param key
 * @return 1 if its in the dicitonary else 0
 */
int dictionary_isin(struct Dictionary* dict, const char* key);

/**
 * adds a number to the dictionary
 * @param dict
 * @param key the key
 * @param number
 * @return 1 if successfully added, 0 else
 */
int dictionary_addNumber(struct Dictionary* dict, const char * key, int number);

/**
 * adds a string to the dictionary
 * @param dict
 * @param key the key
 * @param string
 * @return 1 if successfully added, 0 else
 */
int dictionary_addString(struct Dictionary* dict, const char * key, struct DictionaryString string);

/**
 * adds an array to the dictionary
 * @param dict
 * @param key the key
 * @param array
 * @return 1 if successfully added, 0 else
 */
int dictionary_addArray(struct Dictionary* dict, const char * key, struct DictionaryArray array);

/**
 * returns an dictionary entry associated with the key, if the key is not in the dictionary, the type is set to DICTIONARY_ERROR
 * NOTE: check the type before accessing the values
 * @param dict
 * @param key
 * @return
 */
struct DictionaryEntry dictionary_get(struct Dictionary* dict, const char* key);


#ifdef __cplusplus
}
#endif

#endif //LST_SIMULATOR_DICTIONARY_H
