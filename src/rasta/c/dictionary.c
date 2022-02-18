//
// Created by tobia on 18.12.2017.
//
#include <ctype.h>
#include "rmemory.h"
#include "dictionary.h"
#include "string.h"

/**
 * makes the string uppercase
 * @param string
 */
void uppercase(char * string) {
    for (unsigned int i = 0; i < strlen(string); i++) {
        if (isalpha(string[i])) {
            string[i] = (char)toupper(string[i]);
        }
    }
}
/**
 * changes the directories size
 * @param dict
 * @param size
 */
void dictionary_change_size(struct Dictionary* dict, unsigned int size) {
    dict->data = rrealloc(dict->data,size * sizeof(struct DictionaryEntry));
    dict->actual_size = size;
}

/**
 * adds a entry to a dictionary. Automatically reallocates
 * NOTE: key is set in entry
 * @param dict
 * @param entry
 * @return 1 if the entry was added successfully else 0
 */
int dictionary_add(struct Dictionary* dict, struct DictionaryEntry entry) {
    if (dictionary_isin(dict,entry.key)) return 0;

    uppercase(entry.key);

    if (dict->size >= dict->actual_size) {
        dictionary_change_size(dict,dict->actual_size*2);
    }
    dict->data[dict->size] = entry;
    dict->size++;

    return 1;
}

/*
 * Public definitions
 */

struct DictionaryArray allocate_DictionaryArray(unsigned int size) {
    struct DictionaryArray result;
    result.data = rmalloc(sizeof(struct DictionaryString)* size);
    result.count = size;
    return result;
}

void reallocate_DictionaryArray(struct DictionaryArray* array, unsigned int new_size) {
    array->data = rrealloc(array->data,sizeof(struct DictionaryString)*new_size);
    array->count = new_size;
}

void free_DictionaryArray(struct DictionaryArray* array) {
    if (array->count == 0) return;
    array->count = 0;
    rfree(array->data);
}

struct Dictionary dictionary_create(unsigned int initial_size) {
    if (initial_size < 2) initial_size = 2;
    struct Dictionary result;
    result.size = 0;
    result.actual_size = initial_size;
    result.data = rmalloc(sizeof(struct DictionaryEntry) * initial_size);

    return result;
}

void dictionary_free(struct Dictionary* dict) {
    for (unsigned int i = 0; i < dict->size; i++) {
        if (dict->data[i].type == DICTIONARY_ARRAY) {
            free_DictionaryArray(&dict->data[i].value.array);
        }
    }
    if (dict->actual_size != 0) {
        dict->actual_size = 0;
        dict->size = 0;
        rfree(dict->data);
    }
}

int dictionary_isin(struct Dictionary* dict, const char key[265]) {
    int result = 0;
    char nkey[265];
    strcpy(nkey,key);
    uppercase(nkey);

    for (unsigned int i = 0; i < dict->size; i++) {
        if (strcmp(nkey, dict->data[i].key) == 0) {
            //key matched
            result = 1;
            return result;
        }
    }

    return result;
}

int dictionary_addNumber(struct Dictionary* dict, const char key[265], int number) {
    struct DictionaryEntry entr;
    entr.type = DICTIONARY_NUMBER;
    strcpy(entr.key,key);
    entr.value.number = number;

    return dictionary_add(dict,entr);
}

int dictionary_addString(struct Dictionary* dict, const char key[265], struct DictionaryString string) {
    struct DictionaryEntry entr;
    entr.type = DICTIONARY_STRING;
    strcpy(entr.key,key);
    entr.value.string = string;

    return dictionary_add(dict,entr);
}

int dictionary_addArray(struct Dictionary* dict, const char key[265], struct DictionaryArray array) {
    struct DictionaryEntry entr;
    entr.type = DICTIONARY_ARRAY;
    strcpy(entr.key,key);
    entr.value.array = array;

    return dictionary_add(dict,entr);
}

struct DictionaryEntry dictionary_get(struct Dictionary* dict, const char key[265]) {
    struct DictionaryEntry result;
    result.type = DICTIONARY_ERROR;
    char nkey[265];
    strcpy(nkey,key);
    uppercase(nkey);
    for (unsigned int i = 0; i < dict->size; i++) {
        if (strcmp(nkey, dict->data[i].key) == 0) {
            //found
            return dict->data[i];
        }
    }
    return result;
}


