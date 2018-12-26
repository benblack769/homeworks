#pragma once
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "lru.h"

/*
The user must define an evicition struct that holds an add and delete function
pointer in it. They can also define an add or delete function and pass them to
the cache create function.  If the user does not wish to implment an eviction
policy then they should include the lru header and link with lru.c.
*/

//Forward Declaration of cache object
struct cache_obj;
typedef struct cache_obj *cache_t;

// For a given key string, return a pseudo-random integer:
typedef uint64_t (*hash_func)(key_type key);

typedef struct pair_t
{
  key_type key;
  val_t val;
  size_t size;
  node evict;
} pair;

struct cache_obj
{
  //The list of key-value pairs
  pair *dict;
  //The total number of bins in the dict, following c++ naming conventions
  size_t capacity;
  //Number of keys in the cache
  size_t length;
  //Size taken up by values
  size_t memsize;
  //Maximum Memory
  size_t maxmem;
  //hash function
  hash_func hash;
  //eviction struct
  evict_class *evict;
};

// A wrapper around memcpy with some basic error checking
void *changeval(void *cacheval, const void *newval, size_t val_size);

// Create a new cache object with a given maximum memory capacity.
cache_t create_cache(uint64_t maxmem, hash_func hash, add_func add, remove_func remove);

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(cache_t cache, key_type key, val_t val, uint32_t val_size);

// Retrieve the value associated with key in the cache, or NULL if not found
val_t cache_get(cache_t cache, key_type key, uint32_t *val_size);

// Delete an object from the cache, if it's still there
void cache_delete(cache_t cache, key_type key);

// Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(cache_t cache);

// Destroy all resource connected to a cache object
void destroy_cache(cache_t cache);
