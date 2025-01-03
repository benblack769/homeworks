#include <inttypes.h>

struct cache_obj;
typedef struct cache_obj *cache_t;

typedef const uint8_t *key_type;
typedef const void *val_type;

// For a given key string, return a pseudo-random integer:
typedef uint64_t (*hash_func)(key_type key);

// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size);

// Retrieve the value associated with key in the cache, or NULL if not found.
// The size of the returned buffer will be assigned to *val_size.
val_type cache_get(cache_t cache, key_type key, uint32_t *val_size);

// Delete an object from the cache, if it's still there
void cache_delete(cache_t cache, key_type key);

// Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(cache_t cache);

// Destroy all resource connected to a cache object
void destroy_cache(cache_t cache);
