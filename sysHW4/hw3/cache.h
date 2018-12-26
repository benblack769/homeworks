#include <inttypes.h>

struct cache_obj;
typedef struct cache_obj *cache_t;

typedef const uint8_t *_key_t;
typedef const void *val_t;

// For a given key string, return a pseudo-random integer:
typedef uint64_t (*hash_func)(_key_t key);

typedef void (*rpmt_func)(cache_t cache);

// Create a new cache object with a given maximum memory capacity.
cache_t create_cache(uint64_t maxmem);

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(cache_t cache, _key_t key, val_t val, uint32_t val_size);

// Retrieve the value associated with key in the cache, or NULL if not found
val_t cache_get(cache_t cache, _key_t key, uint32_t *val_size);

// Delete an object from the cache, if it's still there
void cache_delete(cache_t cache, _key_t key);

// Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(cache_t cache);

// Destroy all resource connected to a cache object
void destroy_cache(cache_t cache);

void check_cache(cache_t cache);

//Prints the allocation status of the cache
void print_cache(cache_t cache);
