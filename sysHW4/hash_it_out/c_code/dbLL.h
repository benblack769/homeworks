/*
 * dbLL.h: headerfile for a doubly linked list
 * @ifjorissen, @aled1027
 * 3.4.16
 *
 */
#pragma once

#include "node.h"

typedef struct _dbLL_t dbLL_t;
struct _dbLL_t{
    node_t *head;
    node_t *tail;
    uint32_t size;
};

dbLL_t *new_list();

// insert a new node into the list with (key, val, val_size)
void ll_insert(dbLL_t *list, key_type key, val_type val, uint32_t val_size);

// removes the node with key specified in function call
uint32_t ll_remove_key(dbLL_t *list, key_type key);

// search list for key. If the key is found, return the value. 
// If the key is not found, NULL is returned
val_type ll_search(dbLL_t *list, key_type key, uint32_t *val_size);

// prints the entire list to stdout
void rep_list(dbLL_t *list);

void destroy_list(dbLL_t *list);

// returns an array of all keys
// can access size of array via list->size
key_type *ll_get_keys(dbLL_t *list);

uint32_t ll_size(dbLL_t *list);

