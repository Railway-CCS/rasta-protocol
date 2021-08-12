//
// Created by tobia on 05.02.2018.
//

#include "../headers/rastalisttest.h"
#include <CUnit/Basic.h>
#include "rastalist.h"

void check_rastalist() {
    //check sr part
    struct RastaList list = rastalist_create(2);
    struct rasta_connection scon;
    struct rasta_connection* con;

    //add three items
    scon.remote_id = 1;
    rastalist_addConnection(&list,scon);
    scon.remote_id = 2;
    rastalist_addConnection(&list,scon);
    scon.remote_id = 3;
    rastalist_addConnection(&list,scon);

    CU_ASSERT_EQUAL(list.size,3);

    //get second element
    con = rastalist_getConnection(&list,1);
    CU_ASSERT_EQUAL(con->remote_id, 2);

    //get non existig element
    con = rastalist_getConnection(&list,25);
    CU_ASSERT_EQUAL(con, 0);

    //remove element and check new place
    rastalist_remove(&list,1);
    CU_ASSERT_EQUAL(list.size,2);
    con = rastalist_getConnection(&list,1);
    CU_ASSERT_EQUAL(con->remote_id, 3);






}
