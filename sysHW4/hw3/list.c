#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "list.h"
#include "tools.h"

#define KEYMAXLEN (32)
#define HASHTABLELENGTH (255)



//creates a new bucket in the form of a linked_list, the new bucket is created with a root node
linked_list_t LL_create(){
  linked_list_t new_bucket = malloc(sizeof(linked_list));
  new_bucket->head = NULL;
  return new_bucket;
}

void print(linked_list_t LL)
{
  printf("List: ");
  node *current = LL->head;
  while (current != NULL)
    {
      printf("Node %s : ", current->meta->key);
      current = current->next;
    }
  printf("NULL\n\n");
}


//appends a node with the given key to the end of the given LL
void LL_append(linked_list_t LL, meta_t meta)
{
  node* new = malloc(sizeof(struct node));
  new->next = NULL;
  new->meta = meta;

  //traverses linearly to the end of the LL
  node* current = LL->head;
  node* prev = NULL;
  while (current != NULL)
  {
    prev = current;
    current = current->next;
  }
  if (prev == NULL) LL->head = new;
  else prev->next = new;
  return;
}

node* LL_find(linked_list_t LL, const uint8_t* unhashed_key)
{
  node* current = LL->head;
  while (current != NULL)
    {
      if (strcomp(current->meta->key, unhashed_key)) return current;
      current = current->next;
    }
  return NULL;
}



//deletes a node with the given key from the given LL
void LL_delete(linked_list_t LL, const uint8_t* unhashed_key){
  const char error[] = "The specified key does not exist in this list.\n";  

    node* current = LL->head;

    if (current == NULL) 
    {
      printf("%s",error);
      return;
    }

    //covers case where the root is the node to be deleted
    if (strcomp(current->meta->key,unhashed_key))
      {
	LL->head = current->next;
        free(current);
        return;
      }

    node* previous = current;

    //covers general case by linear searching through the LL until we hit the node with the given key and deleting it 
    while (current != NULL)
    {
      previous = current;
      current = current->next;
      if (current == NULL) 
      {
	printf("%s", error);
	return;
      }
      if (strcomp(current->meta->key, unhashed_key))
      {
	previous->next = current->next;
	free(current);
	return;
      }
    }

    printf("Not sure how you got down here.\n");
    return;
}


void LL_nullify(linked_list_t LL, uint8_t* unhashed_key){
  struct node* trash = LL_find(LL, unhashed_key);
  trash->key = NULL;
}

void LL_print(linked_list_t LL){
  struct node* current_node = LL->head;
  printf("LL: ");
  while (current_node != NULL){
    const uint8_t* curr_key = current_node->meta->key;
    printf("%s, ",curr_key);
    current_node = current_node->next;
  }
  printf("NULL\n");
  return;
}


/*
node** initialize_hash_table(){
  node** ls = malloc(sizeof(node*) * HASHTABLELENGTH);
  for(int i=0; i<=HASHTABLELENGTH; i++){
    
*/





void LL_destroy(linked_list_t LL)
{
  node *current = LL->head;
  while(current != NULL)
  {
    node *next = current->next;
    free(current);
    current = next;
  }
  free(LL);
}



/*
int main() {
    uint8_t *test_key = "hello";
    linked_list* test = LL_create();
    meta_t meta = malloc(sizeof(meta_obj));
    strcopy("hello", meta->key);
    LL_append(test, meta);
    printf("appended 'hello'\n");

    meta_t test_key2 = malloc(sizeof(meta_obj));
    strcopy("goodbye",test_key2->key);
    LL_append(test, test_key2);
    printf("appended 'goodbye'\n");

    printf("FIND TESTING HERE:\n");
    node* two = LL_find(test, "hello");
    printf("Node hello  returned as %s\n\n",two->meta->key);

    printf("DELETE TESTING HERE:\n");
    LL_delete(test, "hello");
    printf("deleted 'hello'\n");
    LL_append(test, meta);
    LL_append(test, meta);
    printf("appended two 'hello's\n");
    printf("We'd like a 'key does not exist' message here: ");
    LL_delete(test, "nothing");
    LL_delete(test,"goodbye");
    printf("Deleted 'goodbye'\n");
    printf("We'd like another 'key does not exist' message here: ");
    LL_delete(test,"goodbye");
    LL_append(test, meta);
    printf("appended a third 'hello'\n");
    node *n = test->head;

    LL_print(test);
    
    return 0;
  }
*/
