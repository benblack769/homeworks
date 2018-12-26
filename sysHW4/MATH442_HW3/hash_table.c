#include <stdio.h>
#include <assert.h>
#include "cache.h"

//This file contains basic operations implemented for a hash table.


//A simple default hash function
// For a given key string, return a pseudo-random integer
uint64_t hash_default(key_type key){
    uint32_t index = (uint32_t)(*key) * 2;
    return index;
}

//Initialize a hash_table_obj
void hash_table_initialize(hash_table_t hash_table, hash_func hash_customized){
    hash_table->buckets_num = HASH_INIT_BUCKET_NUM;
    hash_table->load_factor = LOAD_FACTOR;
    hash_table->scale_factor = SCALE_FACTOR;
    hash_table->current_size = 0;

    if (hash_customized==NULL){
        hash_table->hash_f = hash_default;
    }
    else{
        hash_table->hash_f = hash_customized;
    }

    // Allocate memory for hash_table
    hash_table->hash = (item_t *) malloc((hash_table->buckets_num) * sizeof(item_t));
    assert(hash_table->hash!=NULL&&"hash_table fails to allocate");

    // Initialize all hash entries as empty
    for(uint32_t i = 0; i < hash_table->buckets_num; ++i){
        hash_table->hash[i] = NULL;
    }
}


//Add a new_node_ptr to current hash table
item_t hash_table_set(hash_table_t hash_table, key_type key, val_type val, uint32_t val_size){
    //Initialize a new bucket
    item_t new_item_ptr = (item_t) malloc( sizeof(struct item_obj) );

    new_item_ptr->key = key;
    new_item_ptr->val = val;
    new_item_ptr->val_size = val_size;
    new_item_ptr->node_ptr = NULL;
    new_item_ptr->prev = NULL;
    new_item_ptr->next = NULL;

    uint32_t bucket_index = hash_table_hash_f(hash_table, key);
    item_t current_item_ptr = hash_table->hash[bucket_index];
    if (current_item_ptr==NULL){
        hash_table->hash[bucket_index] = new_item_ptr;
    }
    else {
        //Put the new item at the end of the chain
        item_t p = current_item_ptr;
        for (;p->next!=NULL;p=p->next);
        p->next = new_item_ptr;
        new_item_ptr->prev = p;
    }

    hash_table->current_size++;

    return new_item_ptr;
}



//delete a key from the hash table
void hash_table_delete(hash_table_t hash_table, item_t item_ptr){
    uint32_t bucket_index = hash_table_hash_f(hash_table, item_ptr->key);
    if (item_ptr->prev==NULL){
        hash_table->hash[bucket_index] = item_ptr->next;
    }
    if (item_ptr->prev!=NULL){
        item_ptr->prev->next = item_ptr->next;
    }
    if (item_ptr->next!=NULL){
        item_ptr->next->prev = item_ptr->prev;
    }
    free(item_ptr);
    hash_table->current_size--;

}

//free the space used by hash from the hash_table_object
void destroy_hash(item_t * hash, uint32_t buckets_num){
    for(uint32_t i = 0; i < buckets_num; ++i){
        item_t next = NULL;
        for (item_t p = hash[i];p!=NULL;p=next){
            next = p->next;
            free(p);
        }
    }
}

//Destroy a hash table
void destroy_hash_table(hash_table_t hash_table){
    destroy_hash(hash_table->hash, hash_table->buckets_num);
    free(hash_table->hash);
    hash_table->buckets_num = 0;
    hash_table->current_size = 0;
}

//Resize hash_table by a scale factor, if there are more than hash_table->load_factor*hash_table->current_size items in the hash_table
void resize_hash_table(hash_table_t hash_table){

    uint32_t old_buckets_num = hash_table->buckets_num;
    item_t * old_hash = hash_table->hash;

    hash_table->buckets_num = old_buckets_num * hash_table->scale_factor;
    hash_table->hash = (item_t *) malloc((hash_table->buckets_num) * sizeof(item_t));
    assert(hash_table->hash!=NULL&&"hash_table fails to allocate");

    // Initialize all hash entries as empty in the new hash_table
    for(uint32_t i = 0; i < hash_table->buckets_num; ++i){
        hash_table->hash[i] = NULL;
    }

    //copy all the buckets from the original hash table to the new hash table
    for(uint32_t i = 0; i < old_buckets_num; ++i){
        for (item_t p = old_hash[i];p!=NULL;p=p->next){
            hash_table_set(hash_table, p->key,p->val,p->val_size);
        }
    }
    //destroy the old hash_table
    destroy_hash(old_hash,old_buckets_num);
    free(old_hash);
}

//Look up key in the hash_table; it returns NULL if no item is found; otherwise, it returns a pointer to the corresponding item.
item_t hash_table_find_item(hash_table_t hash_table, key_type key){
    uint32_t bucket_index = hash_table_hash_f(hash_table, key);
    item_t current_item_ptr = hash_table->hash[bucket_index];
    for (;current_item_ptr!=NULL;current_item_ptr=current_item_ptr->next){
        if (*(current_item_ptr->key)==*key){
            return current_item_ptr;
        }
    }
    return NULL;
}

//Return bucket number of a given key
uint32_t hash_table_hash_f(hash_table_t hash_table, key_type key){
    uint32_t bucket_index = hash_table->hash_f(key) % hash_table->buckets_num;
    return bucket_index;
}
// Compute the total amount of memory used up by all values in hash_table(not keys)
uint32_t hash_table_space_used(hash_table_t hash_table){
    uint32_t sum = 0;
    for(uint32_t i = 0; i < hash_table->buckets_num; ++i){
        for (item_t p = hash_table->hash[i];p!=NULL;p=p->next){
            sum+=p->val_size;
        }
    }
    return sum;
}
//draw the current hash table
void draw_hash_table(hash_table_t hash_table){
    printf("Current hash table:\n");
    for(uint32_t i = 0; i < hash_table->buckets_num; ++i){
        printf("SLOT %u: ",i);
        for (item_t p = hash_table->hash[i];p!=NULL;p=p->next){
            if (p!=hash_table->hash[i]){
                printf(", ");
            }
            printf("%hhu",*(p->key));
        }
        printf("\n");
    }
}
