#include <stdio.h>
#include <assert.h>
#include "cache.h"

//This file contains basic operations implemented for a double linked list which is the
//data structure I use to implement LRC.


//LRU is my default eviction policy
key_type evict_default(cache_t cache){
    return cache->linked_list->rear->item_ptr->key;
}

//Initialize a linked_list
void linked_list_initialize(linked_list_t linked_list, evict_func evict_customized){
    linked_list->current_size = 0;
    linked_list->front = linked_list->rear = NULL;

    if (evict_customized==NULL){
        linked_list->evict_f = evict_default;
    }
    else{
        linked_list->evict_f = evict_customized;
    }
}

//Add a new node to current linked list
node_t linked_list_set(linked_list_t linked_list){
    //Initialize the new node
    node_t new_node_ptr = (node_t) malloc( sizeof(struct node_obj) );

    new_node_ptr->item_ptr = NULL;
    new_node_ptr->prev = NULL;
    new_node_ptr->next = linked_list->front;

    // If cache is empty, change both front and rear pointers
    if (is_linked_list_empty(linked_list)){
        linked_list->rear = linked_list->front = new_node_ptr;
    }
    else {
        linked_list->front->prev = new_node_ptr;
        linked_list->front = new_node_ptr;
    }
    linked_list->current_size++;

    return new_node_ptr;
}

//move a newly used key to the front of the linked_list
void linked_list_top(linked_list_t linked_list,node_t node_ptr){
    //if it's already the first item, do nothing
    if (node_ptr->prev==NULL){
        return;
    }

    //if not, move it to the front in the linked list
    //if the node is the last, tell the cache to change its pointer(to rear) to the node before it.
    if (node_ptr->next==NULL){
        linked_list->rear = node_ptr->prev;   
    }
    //if the node has neighbors, tell them to connect with each other
    if (node_ptr->prev!=NULL){
        node_ptr->prev->next = node_ptr->next;
    }
    if (node_ptr->next!=NULL){
        node_ptr->next->prev = node_ptr->prev;
    }
    if (!is_linked_list_empty(linked_list)){
        linked_list->front->prev = node_ptr;
    }
    node_ptr->prev = NULL;
    node_ptr->next = linked_list->front;
    
    linked_list->front = node_ptr;
}


//delete a key from the linked list
void linked_list_delete(linked_list_t linked_list, node_t node_ptr){ 
    if (node_ptr->prev==NULL){
        linked_list->front = node_ptr->next;
    }
    if (node_ptr->next==NULL){
        linked_list->rear = node_ptr->prev;
    }
    if (node_ptr->prev!=NULL){
        node_ptr->prev->next = node_ptr->next;
    }
    if (node_ptr->next!=NULL){
        node_ptr->next->prev = node_ptr->prev;
    }
    free(node_ptr);
    linked_list->current_size--;
}


//Destroy a linked list
void destroy_linked_list(linked_list_t linked_list){
    node_t next = NULL;
    for(node_t p = linked_list->front; p!=NULL; p=next){
        next = p->next;
        free(p);
    }   
    linked_list->front = linked_list->rear = NULL;
    linked_list->current_size = 0;
}

// Check whether the cache is empty
uint32_t is_linked_list_empty(linked_list_t linked_list){
    return (linked_list->current_size == 0);
}

//draw the current linked list
void draw_linked_list(linked_list_t linked_list){
    uint32_t node_per_line = 8;
    uint32_t counter = 0;
    printf("Current linked list:");
    for (node_t p = linked_list->front;p!=NULL;p=p->next, counter++){
        if (counter%node_per_line==0){
            printf("\n");
        }
        printf("(key: %hhu, val: %u,val_size:%u)",*(p->item_ptr->key), *(uint32_t *)p->item_ptr->val, p->item_ptr->val_size);

        if (p!=linked_list->front){
            printf(", ");
        }

    }    
    printf("\n");
}