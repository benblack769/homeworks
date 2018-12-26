// Alex Pan
//
// cache.h - header file for the cache implementation.
//
// This is the header file for my cache API. It implements all the basic
// functions of the cache and requires that the user include their own
// implementation for their eviction policy. I've implemented a default
// policy in the included file header "lru.h" which in acts as the
// modifiable API of the LRU eviction policy. I've also highlighted
// the specific areas in which this API depends on user defined structs.
// (Specifically the user needs to define things on lines: 14, 32, 58.)

#include <inttypes.h>
#include "lru.h" // User defined eviction policy header file.

typedef const uint8_t *key_type;
typedef const void *val_type;

// For a given key string, return a pseudo-random integer:
typedef uint64_t (*hash_func)(key_type key);


// Struct cache_obj. This contains a pointer to a hash function,
// values for the maximum memory capacity, the current occupied
// memory, the number of total buckets in the cache, and the number of
// occupied buckets. It also contains an array of cache_keyval elements,
// and a user defined datastructure for the eviction policy. The keyvals
// struct is linked to the LRU eviction policy so I've moved that over
// to lru.h to keep things cleaner. To implement the LRU policy as default,
// the evict_t struct and related methods are all defined in the lru.h
// header file which can be  modified by the user if desired.
struct cache_obj{
	// Pointer to a hash function that should be user defined,
	// but a default one is given in the implementation.
	hash_func hashf;

	// Information about the cache
	uint64_t maxmemory;
	uint64_t occupiedmemory;
	uint64_t buckets;
	uint64_t occupiedbuckets;

	// An array of cache_keyvals that contain the elements we insert
	// into the cache. Defined in lru.h
	struct cache_keyval * keyvals; // Must be defined by user
	evict_t lru; // Must be defined by user
};
typedef struct cache_obj *cache_t;


// Create a new cache object with a given maximum memory capacity
// and a hash function.
cache_t create_cache(uint64_t maxmem, hash_func hash);

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size);

// Retrieve the value associated with key in the cache, or NULL if not found.
// The size of the returned buffer will be assigned to *val_size.
val_type cache_get(cache_t cache, key_type key,uint32_t *_val_size);

// Delete an object from the cache, if it's still there.
void cache_delete(cache_t cache, key_type key);

// Compute the total amount of memory used up by all cache values (not keys).
uint64_t cache_space_used(cache_t cache);

// Destroy all resource connected to a cache object.
void destroy_cache(cache_t cache);
