/*
 * node.h: the header file for a node
 * @ifjorissen, @aled1027
 * 3.4.16
 *
 */

#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef const uint8_t *key_type;
typedef const void *val_type;

typedef struct _node_t node_t;
struct _node_t
{
    key_type key;
    val_type val;
    uint32_t val_size;
    node_t *next;
    node_t *prev;
};

// Node is specifically designed for (1) usage in a double linked list
// and (2) for holding a key-value pair. 

//create a new node with a key, value, and the size of the value
node_t *new_node(key_type key, val_type val, uint32_t val_size);

// set node's next and prev pointers to next and prev respectively
void set_next(node_t *node, node_t *next);
void set_prev(node_t *node, node_t *prev);

//represent the node (print it)
void rep_node(node_t *node); 

key_type get_key(node_t *node);
