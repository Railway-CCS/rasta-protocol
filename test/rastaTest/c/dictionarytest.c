//
// Created by tobia on 13.01.2018.
//

#include "../headers/dictionarytest.h"
#include "dictionary.h"
#include "string.h"
#include <CUnit/Basic.h>
#include "stdio.h"

void testDictionary() {
    struct Dictionary dict = dictionary_create(0);
    CU_ASSERT_EQUAL(dict.actual_size,2);
    //add a number
    dictionary_addNumber(&dict,"NUMBER",5);

    //add a string
    struct DictionaryString str;
    strcpy(str.c,"Test");
    dictionary_addString(&dict,"STRING",str);

    //add an arry
    struct DictionaryArray arr = allocate_DictionaryArray(2);
    CU_ASSERT_EQUAL(arr.count, 2);
    arr.data[0] = str;
    str.c[0] = 'F';
    arr.data[1] = str;
    dictionary_addArray(&dict,"ARRAY",arr);

    CU_ASSERT_EQUAL(dictionary_isin(&dict, "NUMBER"),1);

    char test[100];
    strcpy(test,"STRING");

    CU_ASSERT_EQUAL(dictionary_isin(&dict, test),1);
    CU_ASSERT_EQUAL(dictionary_isin(&dict, "Array"),1);
    CU_ASSERT_EQUAL(dictionary_isin(&dict, "nunber"),0);

    struct DictionaryEntry entr;

    //check number
    entr = dictionary_get(&dict,"NUMBER");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_NUMBER);
    CU_ASSERT_EQUAL(entr.value.number, 5);

    //check string
    entr = dictionary_get(&dict, "STRING");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_STRING);
    CU_ASSERT_EQUAL(strcmp(entr.value.string.c,"Test"),0);

    //check array
    entr = dictionary_get(&dict, "ARRAY");
    CU_ASSERT_EQUAL(entr.type, DICTIONARY_ARRAY);
    CU_ASSERT_EQUAL(entr.value.array.count, 2);
    CU_ASSERT_EQUAL(strcmp(entr.value.array.data[0].c,"Test"),0);
    CU_ASSERT_EQUAL(strcmp(entr.value.array.data[1].c,"Fest"),0);



    dictionary_free(&dict);

}

