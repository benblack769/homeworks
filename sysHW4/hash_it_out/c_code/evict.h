#pragma once

#include <inttypes.h>

typedef const uint8_t *key_type; //TODO - ask eitan what the best way to do types here is 
struct evict_obj;
typedef struct evict_obj *evict_t;
typedef uint64_t (*hash_func)(key_type key);

// creates evict object and returns a pointer to it
// takes as input the max size of queue, but the size of the
// eviction object is dynamically adjusted as needed
evict_t evict_create(uint32_t arr_size);

// notifies evict obj that key has been set to cache
// assumes that key type is not already in queue, as 
// checking for the key would require non-constant time.
void evict_set(evict_t evict, key_type key);

// notifies evict obj that key has been "gotten" from cache
// i.e., it's been accessed
void evict_get(evict_t evict, key_type key);

// notifies evict obj that key has been delete from cache
void evict_delete(evict_t evict, key_type key);

// delete and free all memory of evict_t
void evict_destroy(evict_t evict);

// Destroy all resource connected to a cache object
// User of evict_select_for_removal is responsible for freeing the memory
// Does not actually remove the key from the eviction object, evict_delete must still be called
key_type evict_select_for_removal(evict_t evict);
