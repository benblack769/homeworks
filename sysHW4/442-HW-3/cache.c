#include "cache.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct _item {
	key_type key;
	val_type value;
	uint32_t size;
	struct _item *next, *prev, *newer, *older;

} *item_t;

struct cache_obj {
	uint64_t capacity;
	hash_func hashfunc;
	uint64_t table_size;
	item_t* items;
	item_t lru, mru; 
	uint8_t resizes;
	uint64_t current_storage_used;
};

#define MAX_SIZE 18446744073709551615U // = 2^64
#define SIZE_STANDARD 64
#define BYTE_SIZE 8
uint64_t id; // An identifier variable for each cache node
struct _item *iterate; // Used for iterating over the linked list of items in the cache

//This is the dbj2 hash algorithm.
uint64_t default_hash(key_type key) {
	uint64_t h = 5381;

	while (*key) {
		h = 33 * h ^ *key++;
	}
	return h;
}

// Getter functions for testing purposes:
uint8_t get_resizes(cache_t cache) {
	return cache->resizes;
}

uint64_t get_hashfunc(cache_t cache, key_type key) {
	return cache->hashfunc(key);
}

uint64_t get_current_storage_used(cache_t cache) {
	return cache->current_storage_used;
}

cache_t create_cache(uint64_t maxmem, hash_func hash) {

	if (maxmem < SIZE_STANDARD) {
		printf("The given maxmem (%lu) is too small; set to 64 bytes.\n",maxmem);
		maxmem = SIZE_STANDARD;
	}

	cache_t new_cache = calloc(1, sizeof(struct cache_obj));

	new_cache->capacity = maxmem;

	new_cache->table_size = maxmem / BYTE_SIZE;

	if (!hash) {
		printf("No custom hash given. Using default.\n");
		new_cache->hashfunc = &default_hash;
	}

	else {
		new_cache->hashfunc = hash;
	}

	new_cache->items = calloc(new_cache->table_size, sizeof(item_t));
	new_cache->resizes = 0;
	new_cache->current_storage_used = 0;
	new_cache->mru = NULL;
	new_cache->lru = NULL;

	for(id=0; id < new_cache->table_size; id++) {
		new_cache->items[id] = NULL;
	}

	return new_cache;
}

void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size) {

	if (!cache || !key || !val) {
		printf("A problem occurred at cache_set: Either the cache, key, or value is NULL. Exiting...\n");
		exit(-1);
	}

	if (val_size <= cache->capacity) {

		while (cache->current_storage_used + val_size > cache->capacity) {
			printf("The key/value pairing about to be added will/still exceed capacity! Removing the least recently used pairing, which is \"%s\".\n",cache->lru->key);
			cache_delete(cache,cache->lru->key);
		}
	}
	// Dynamically resizes by doubling if there's at least half as many nodes as the table_size.
	while (cache->current_storage_used + val_size > (cache->capacity / 2) ) {
		printf("We've reached (or are still at) over half capacity; doubling cache size.\n");
		if (cache->table_size * 2 > MAX_SIZE) {
			// Do nothing; doubling would be too big of a resize.
		}
		else {
			cache->table_size = cache->table_size * 2;
			cache->capacity = cache->capacity * 2;
			cache->resizes += 1;
			cache->items = realloc(cache->items, cache->table_size * sizeof(item_t));
			for (uint8_t i = cache->table_size / 2; i < cache->table_size; i++) {
				cache->items[i] = NULL;
			}
		}
	}

	id = (cache->hashfunc(key)) % cache->table_size;

	uint8_t breakIt = 0;
	for (uint8_t i = 0; i < cache->resizes+1; id = ((cache->hashfunc(key)) % ((uint64_t) pow(2, i) * cache->table_size))) {
		for (iterate = cache->items[id]; iterate; iterate = iterate->next) {
			if ((iterate->key) && (!strcmp(key,iterate->key))) {
				cache_delete(cache,key);
				breakIt = 1;
				break;
			}
		}
		if (breakIt) {
			break;
		}
		i++;
	}

	id = (cache->hashfunc(key)) % cache->table_size;

	item_t item = calloc(1, sizeof(struct _item));
	item->size = val_size;
	item->key = strdup(key);
	item->value = calloc(1, val_size);
	memcpy((void*)item->value, val, val_size);

	if (!cache->items[id]) {
		cache->items[id] = item;
		item->next = NULL;
		item->prev = NULL;
	}
	else {
		for (iterate = cache->items[id]; iterate; iterate = iterate->next) {
			if (!iterate->next) {
				iterate->next = item;
				item->next = NULL;
				item->prev = iterate;
				break;
			}
		}
	}

	if (!cache->mru && !cache->lru) {
		cache->mru = item;
		cache->lru = item;
	}
	else {
		cache->mru->older = item;
		item->newer = cache->mru;
		item->older = NULL;
		cache->mru = item;
	}

	cache->current_storage_used += item->size;

}

val_type cache_get(cache_t cache, key_type key, uint32_t *val_size)	{

	if (!cache || !key) {
		printf("A problem occurred at cache_get: Either the cache or key is NULL. Exiting...\n");
		exit(-1);
	}
	id = cache->hashfunc(key) % cache->table_size;
	for (uint8_t i = 0; i < (cache->resizes + 1); id = ((cache->hashfunc(key)) % (((uint64_t) pow(2, i)) * cache->table_size))) {
		for (item_t item = cache->items[id]; item; item = item->next) {
			if ((item->key) && (!strcmp(key,item->key))) {
				*val_size = item->size;
				if (item != cache->mru) { 
					cache->mru->older = item;
					item->older = NULL;
					item->newer = cache->mru;
					cache->mru = item; 
				}
				return item->value;
			}
		}
		i++;
	}
	*val_size = 0;
	return NULL;
}

void cache_delete(cache_t cache, key_type key) {

	if (!cache || !key) {
		printf("A problem occurred at cache_delete: Either the cache or key is NULL. Exiting...\n");
		exit(-1);
	}

	id = cache->hashfunc(key) % cache->table_size;

	for (uint8_t i = 0; i < cache->resizes+1; id = ((cache->hashfunc(key)) % (((uint64_t) pow(2, i)) * cache->table_size))) {
		for (item_t item = cache->items[id]; item; item = item->next) {
			if ((item->key) && (!strcmp(key,item->key))) {
				cache->current_storage_used -= item->size;
				if (item == cache->mru) {
					cache->mru = item->newer;
				}
				if (item == cache->lru) {
					cache->lru = item->older;
				}

				if (item->older && item->newer) {
					item->older->newer = item->newer;
					item->newer->older = item->older;
				}

				else if (item->older) {
					item->older->newer = NULL;
				}
				else if (item->newer) {
					item->newer->older = NULL;
				}
				if (item->next && item->prev) {
					item->next->prev = item->prev;
					item->prev->next = item->next;
				}
				else if (item->next) {
					cache->items[id] = item->next;
				}
				else if (item->prev) {
					item->prev->next = NULL;
				}
				item->key = NULL;
				free((key_type*)item->key);
				item->value = NULL;
				free((val_type*)item->value);
				item = NULL;
				free(item);
				if (!cache->items[id]->key) {
					cache->items[id] = NULL;
				}
				return;
			}
		}
		i++;
	}
	printf("Didn't find the key/value pair for deletion.\n");
}

uint64_t cache_space_used(cache_t cache) {

	if (!cache) {
		printf("A problem occurred at cache_space_used: The cache is NULL. Exiting...\n");
		exit(-1);
	}
	return (uint64_t) cache->current_storage_used;
}

void destroy_cache(cache_t cache) {

	if (!cache) {
		printf("A problem occurred at destroy_cache: The cache is NULL. Exiting...\n");
		exit(-1);
	}

	item_t item, next_item; // Need a separate next_item rather than ->next since we're freeing everything;
	// to keep track of the next item.

	for (id = 0; id < cache->table_size; id++) {
		next_item = NULL;
		for (item = cache->items[id]; item; item = next_item) {
			if (item->key) {
				next_item = item->next;
				free((key_type *)item->key);
				item->key = NULL;
				free((val_type *)item->value);
				item->value = NULL;
				free(item);
				item = NULL;
			}
			else {
				free(item);
				item = NULL;
			}
		}
	}

	free(cache->items);
	cache->items = NULL;
	free(cache);
	cache = NULL;
}

void print_cache(cache_t cache) {

	if (!cache) {
		printf("A problem occurred at print_cache: The cache is NULL.\n");
		exit(-1);
	}

	printf("print_cache: Basic information about the given cache:\n");
	printf("Capacity: %lu bytes; Current Memory Used: %lu bytes; Table Size: %lu elements.\n",cache->capacity,cache->current_storage_used,cache->table_size);
	if (cache->mru && cache->lru) {
		printf("Most Recently Used item is key \"%s\"; Least Recently Used item is key \"%s\".\n",cache->mru->key,cache->lru->key);
	}
	printf("The list of nodes/keys/values in the cache:\n");

	for (id = 0; id < cache->table_size; id++) {
		iterate = cache->items[id];
		while(iterate) {
			printf("Node %lu: key = \"%s\", value = \"%s\", value size = %u.\n", id, iterate->key, (char*)iterate->value,iterate->size);
			//usleep(200000); // uncomment if you want to slow down the speed of this printing out.
			iterate = iterate->next;
		}
	}
}
