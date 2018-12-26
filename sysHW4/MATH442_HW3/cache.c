#include <stdio.h>
#include <assert.h>
#include "cache.h"

//This file contains basic operations implemented for a cache
//many of which uses functions in hash_table.c and LRU.c as helper functions


// Create a new cache object with a given maximum memory capacity.
cache_t create_cache(uint32_t maxmem, hash_func hash_customized, evict_func evict_customized){
    cache_t cache = (cache_t) malloc( sizeof(struct cache_obj));
    assert(cache!=NULL&&"cache fails to allocate");

    // Initialize the cache
    cache->maxmem = maxmem;

    // Initialize the linked_list pointed by the cache
    cache->linked_list = (linked_list_t) malloc( sizeof(struct linked_list_obj));
    assert(cache!=NULL&&"cache fails to allocate");

    linked_list_initialize(cache->linked_list, evict_customized);

    // Initialize the hash_table pointed by the cache
    cache->hash_table = (hash_table_t) malloc( sizeof(struct hash_table_obj));
    assert(cache!=NULL&&"cache fails to allocate");

    hash_table_initialize(cache->hash_table, hash_customized);

	return cache;
}

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size){

    item_t item_to_set = hash_table_find_item(cache->hash_table, key);
    // If key already exists, it will overwrite the old value.
    if (item_to_set!=NULL){
        item_to_set->val = val;
        return;
    }

    //If maxmem capacity is exceeded, sufficient values will be removed
    if (cache_space_used(cache) >= cache->maxmem){
        cache_evict(cache);
    }

    //Check if we need to resize hash table
    if (cache->hash_table->current_size+1 > (uint32_t) ((double) cache->hash_table->buckets_num*LOAD_FACTOR)){
        resize_hash_table(cache->hash_table);
    }

    item_t item = hash_table_set(cache->hash_table, key, val, val_size);
    node_t node = linked_list_set(cache->linked_list);

    // //connect the item in hash_table with its corresponding node in linked list
    cache_connect_node_and_item(item,node);

}

// Retrieve the value associated with key in the cache, or NULL if not found. It will move the item with key to the front of the linked_list
val_type cache_get(cache_t cache, key_type key, uint32_t * val_size){
    item_t item_ptr = hash_table_find_item(cache->hash_table, key);
    if (item_ptr!=NULL){
        //move the newly used node to the front of the linked_list
        linked_list_top(cache->linked_list,item_ptr->node_ptr);
        *val_size = item_ptr->val_size;
        return item_ptr->val;
    }
    //If key is not found, assign 0 to val_size
    *val_size = 0;
	return NULL;

}


// Delete an object from the cache, if it's still there
void cache_delete(cache_t cache, key_type key){
        item_t item_ptr = hash_table_find_item(cache->hash_table, key);
        linked_list_delete(cache->linked_list, item_ptr->node_ptr);
        hash_table_delete(cache->hash_table, item_ptr);

}

// Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(cache_t cache){

    return hash_table_space_used(cache->hash_table);

}

// Destroy all resource connected to a cache object
void destroy_cache(cache_t cache){
    //Destroy double linked list and hash table
    destroy_linked_list(cache->linked_list);
    destroy_hash_table(cache->hash_table);
    free(cache->hash_table);
    free(cache->linked_list);
}


//Evict the last one to make space
void cache_evict(cache_t cache){
    //find the key to evict
    key_type key_to_evict = cache->linked_list->evict_f(cache);
    cache_delete(cache, key_to_evict);

}

//connect the item in hash_table with its corresponding node in linked list
void cache_connect_node_and_item(item_t item_ptr,node_t node_ptr){
    item_ptr->node_ptr = node_ptr;
    node_ptr->item_ptr = item_ptr;
}

//print the basic information of the cache
void draw_cache(cache_t cache){
    printf("Cache Basic Info:\n");
    printf("cache_maxmem: %u, cache_current_size: %u\n",cache->maxmem,cache_space_used(cache));
    printf("hash_table_buckets_num: %u, hash_table_current_size: %u\n",cache->hash_table->buckets_num,cache->hash_table->current_size);
    draw_linked_list(cache->linked_list);
    draw_hash_table(cache->hash_table);

}
