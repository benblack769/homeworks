#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#pragma

//This is a header file which contains constants, structs
//and prototypes of all functions in cache.c, hash_table.c and LRU.c


//1. Constants, sturcts and function signatures

// HASH_INIT_BUCKET_NUM - the initial bucket numbers of my hash table
// LOAD_FACTOR - the threshold of resizing(expanding) my hash table
// SCALE_FACTOR - the scale to expand my hash table

#define HASH_INIT_BUCKET_NUM 5
#define LOAD_FACTOR 0.5
#define SCALE_FACTOR 2

// node_obj - a node object which stores a pair of key and value; it is also an unit element in a double linked_list
// item_obj - an item object which stores a pointer to a node; it is also an unit element in a hash_table
// hash_table_obj - a hash table object that has a 2D array which consists of pointers to items.
// linked_list_obj - a double linked list object that has pointers to the front and end of the double linked_list which consists of nodes.
// cache_obj - cache which has pointers to hash_table and linked_list

struct node_obj;
struct item_obj;
struct hash_table_obj;
struct linked_list_obj;
struct cache_obj;

typedef const uint8_t * key_type;
typedef const void * val_type;

// For a given key string, return a pseudo-random integer:
typedef uint64_t (*hash_func)(key_type key);
// For a full cache, return the key of an element to evict
typedef key_type (*evict_func)(struct cache_obj * cache);

typedef struct node_obj * node_t;
typedef struct item_obj * item_t;
typedef struct hash_table_obj * hash_table_t;
typedef struct linked_list_obj * linked_list_t;
typedef struct cache_obj * cache_t;

struct node_obj{
	item_t item_ptr;
	struct node_obj * prev;
	struct node_obj * next;

};
struct item_obj{
	key_type key;
	val_type val;
	uint32_t val_size;
	node_t node_ptr;
    struct item_obj * prev;
    struct item_obj * next;
};
struct hash_table_obj{
    uint32_t current_size;
    uint32_t buckets_num;
    double load_factor;
    uint32_t scale_factor;
    item_t * hash; // an array of pointers to buckets
    hash_func hash_f;
};
struct linked_list_obj{
	uint32_t current_size;
	node_t front;
	node_t rear;
    evict_func evict_f;
};
struct cache_obj{
	hash_table_t hash_table;
	linked_list_t linked_list;
	uint32_t maxmem;
};



//2. Functions in cache.c

// Create a new cache object with a given maximum memory capacity.
cache_t create_cache(uint32_t maxmem,hash_func hash_customized, evict_func evict_customized);
// Add a <key, value> pair to the cache.
// If key already exists, it will overwrite the old value.
// If maxmem capacity is exceeded, sufficient values will be removed
// from the cache to accomodate the new value.
void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size);
// Retrieve the value associated with key in the cache, or NULL if not found.
// The size of the returned buffer will be assigned to *val_size.
val_type cache_get(cache_t cahce, key_type key, uint32_t *val_size);

// Delete an object from the cache, if it's still there
void cache_delete(cache_t cache, key_type key);

// Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(cache_t cache);

// Destroy all resource connected to a cache object
void destroy_cache(cache_t cache);

//evict an item out of cache
void cache_evict(cache_t cache);
//connect the item in hash_table with its corresponding node in linked list
void cache_connect_node_and_item(item_t item_ptr,node_t node_ptr);
//print the basic information of the cache
void draw_cache(cache_t cache);



//3.Functions in hash_table.c

//Initialize a hash_table_obj
void hash_table_initialize(hash_table_t hash_table, hash_func hash_customized);
//Add a new_node_ptr to current hash table
item_t hash_table_set(hash_table_t hash_table, key_type key, val_type val, uint32_t val_size);
//A simple default hash function
uint64_t hash_default(key_type key);

//delete the key from the hash table
void hash_table_delete(hash_table_t hash_table, item_t item_ptr);

//free the space used by hash from the hash_table_object
void destroy_hash(item_t * hash, uint32_t buckets_num);

//Destroy a hash table
void destroy_hash_table(hash_table_t hash_table);

//Resize hash_table by a scale factor, if there are more than load_factor*hash_table_size items in the hash_table
void resize_hash_table(hash_table_t hash_table);

//Look up key in the hash_table; it returns NULL if no item is found; otherwise, it returns a pointer to the corresponding item.
item_t hash_table_find_item(hash_table_t hash_table, key_type key);
//Return bucket number of a given key
uint32_t hash_table_hash_f(hash_table_t hash_table, key_type key);
// Compute the total amount of memory used up by all values in hash_table(not keys)
uint32_t hash_table_space_used(hash_table_t hash_table);
//draw the current hash table
void draw_hash_table(hash_table_t hash_table);




//4.Functions in LRU.c

//Initialize a linked_list
void linked_list_initialize(linked_list_t linked_list, evict_func evict_customized);
//Add a new node to current linked list
node_t linked_list_set(linked_list_t linked_list);
//move the newly used key to the front of the linked_list
void linked_list_top(linked_list_t linked_list,node_t node_ptr);

//A simple default eviction policy(LRU)
key_type evict_default(cache_t cache);

//delete the key from the linked list
void linked_list_delete(linked_list_t linked_list, node_t node_ptr);

//Destroy a linked list
void destroy_linked_list(linked_list_t linked_list);

// Check whether the linked_list is empty
uint32_t is_linked_list_empty(linked_list_t linked_list);

//draw the current linked list
void draw_linked_list(linked_list_t linked_list);
