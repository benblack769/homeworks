#include <inttypes.h>
#include "tools.h"

typedef struct node{
  uint8_t* key;
  meta_t meta;
  struct node *next;
} node;

typedef struct linked_list{
  node* head;
  uint8_t len;
} linked_list;

typedef linked_list* linked_list_t;

linked_list_t LL_create();

void LL_append(linked_list_t LL, meta_t meta);

void LL_delete(linked_list_t LL, const uint8_t *key);

node* LL_find(linked_list_t LL, const uint8_t *key);

void LL_destroy(linked_list_t LL);
