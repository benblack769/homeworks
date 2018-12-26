#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbLL.h"
#include "dbLL_tests.h"


static void insert_elts_into_list(dbLL_t *test_ll)
{
    uint8_t key1[2] = {1, '\0'};
    uint8_t key2[2] = {89, '\0'};
    uint8_t key3[2] = {12, '\0'};
    uint8_t key4[2] = {30, '\0'};
    uint8_t val;

    val = 99;
    ll_insert(test_ll, key1, &val, 1);
    rep_list(test_ll);

    val = 23;
    ll_insert(test_ll, key2, &val, 1);
    rep_list(test_ll);

    val = 25;
    ll_insert(test_ll, key3, &val, 1);
    rep_list(test_ll);

    val = 255;
    ll_insert(test_ll, key4, &val, 1);

    printf("************************************************\n");
    printf("*** List after insertion ***********************\n");
    printf("************************************************\n");
    rep_list(test_ll);
    printf("************************************************\n");
}

static void test_dbLL_creation()
{
    printf("\n************************************************\n");
    printf("*** testing CREATION of doubly linked list ***\n");
    printf("************************************************\n");

    dbLL_t *test_ll = new_list();
    destroy_list(test_ll);

    printf("\n*** ... done testing creation ***\n");
    printf("************************************************\n");

}

static void test_dbLL_insert()
{
    dbLL_t *test_ll = new_list();
    insert_elts_into_list(test_ll);
    destroy_list(test_ll);
}

static void test_dbLL_search()
{
    printf("\n************************************************\n");
    printf("*** testing SEARCH on the doubly linked list ***\n");
    printf("************************************************\n");

    uint8_t val = 0;
    uint8_t *res;
    uint32_t val_size;

    uint8_t key1[2] = {1, '\0'};
    uint8_t key2[2] = {89, '\0'};
    uint8_t key3[2] = {12, '\0'};
    uint8_t key4[2] = {30, '\0'};
    uint8_t key5[2] = {37, '\0'};
    uint8_t key6[2] = {2, '\0'};

    dbLL_t *test_ll = new_list();
    insert_elts_into_list(test_ll);

    //search for some values in the list
    val = 99;
    res = (uint8_t *)ll_search(test_ll, key1, &val_size);
    assert(*res == val);

    val = 23;
    res = (uint8_t *)ll_search(test_ll, key2, &val_size);
    assert(*res == val);

    val = 25;
    res = (uint8_t *)ll_search(test_ll, key3, &val_size);
    assert(*res == val);

    val = 255;
    res = (uint8_t *)ll_search(test_ll, key4, &val_size);
    assert(*res == val);

    //(try to) search for some value not in the list 
    res = (uint8_t *)ll_search(test_ll, key5, &val_size);
    assert(res == NULL);

    //(try to) search for some value not in the list 
    res = (uint8_t *)ll_search(test_ll, key6, &val_size);
    assert(res == NULL);

    //print & destory
    rep_list(test_ll);
    destroy_list(test_ll);

    printf("*** ... done testing search\n");
    printf("************************************************\n");
}

static void test_dbLL_remove()
{
    printf("\n************************************************\n");
    printf("*** testing REMOVAL on the doubly linked list ***\n");
    printf("************************************************\n");

    dbLL_t *test_ll = new_list();
    insert_elts_into_list(test_ll);

    uint8_t key1[2] = {1, '\0'};
    uint8_t key2[2] = {89, '\0'};
    uint8_t key3[2] = {12, '\0'};
    uint8_t key4[2] = {30, '\0'};
    uint8_t key5[2] = {37, '\0'};

    uint32_t res; // res will 0 if no <k,v> is removed

    // remove middle value
    res = ll_remove_key(test_ll, key2);
    assert(res > 0);
    rep_list(test_ll);

    res = ll_remove_key(test_ll, key4);
    assert(res > 0);
    rep_list(test_ll);

    //(try to) remove a non-existent value
    res = ll_remove_key(test_ll, key5);
    assert(res == 0);
    rep_list(test_ll);

    //remove the head
    res = ll_remove_key(test_ll, key1);
    assert(res > 0);
    rep_list(test_ll);

    //(try to) remove something we've already removed
    res = ll_remove_key(test_ll, key4);
    assert(res == 0);
    rep_list(test_ll);

    //remove the remaining value in the list
    res = ll_remove_key(test_ll, key3);
    assert(res > 0);

    rep_list(test_ll);
    destroy_list(test_ll);

    printf("*** ... done testing removal\n");
    printf("************************************************\n");
}

void dbll_tests()
{
    test_dbLL_creation();
    test_dbLL_insert();
    test_dbLL_search();
    test_dbLL_remove();
}
