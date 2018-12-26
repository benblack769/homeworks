#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "cache.h"
#include "list.h"
#include "tools.h"
#include "slabs.h"

//Values are stored in the buffer
//Keys are stored in the form <key,start,end>
//Keys are strings of length less than KEYMAXLEN
typedef struct cache_real_obj 
{
  hash_func hasher;
  uint64_t size;
  manager_t slab_manager;
  linked_list_t* buckets; //Array of linked list pointers
  uint32_t num_buckets;
} cache_real_obj;

typedef struct cache_obj
{
  cache_real_obj *cache;
} cache_obj;

void print_cache(cache_t cache)
{
  print_slabs(cache->cache->slab_manager);
}


//Destroy all resources connected to a cache object
void destroy_cache_obj(cache_real_obj *cache)
{
  printf("Cache was destroyed.\n");
  //Free buffer
  slab_destroy(cache->slab_manager);
  int i=0;
  for (i; i < cache->num_buckets; i++)  LL_destroy(cache->buckets[i]);
  free(cache);
}

//This might be a useful hash function later
uint64_t hash(_key_t key)
{
  uint64_t ret = 1;
  uint8_t counter = 0;
  while(counter < 32) {
    if (key[counter] == '\0') return ret;
    ret *= key[counter];
    counter++;
  }
  printf("Invalid key: Key is too long or is not a string.\n");
  exit(0);
}

//Does a linear search of the keyspace and returns a metadata struct or NULL
meta_t get_key_loc(cache_t cache, _key_t key)
{
  cache_real_obj *c = cache->cache;
  uint32_t bucket = hash(key) % c->num_buckets;
  node *current = LL_find(c->buckets[bucket], key);
  if (current == NULL) return NULL;
  return current->meta;
}

//Create a new cache object with the given capacity
cache_t create_cache(uint64_t maxmem)
{
  cache_t cache = malloc(sizeof(cache_obj));
  cache->cache = malloc(sizeof(cache_real_obj));
  cache_real_obj *ret = cache->cache;
  ret->hasher = &hash;
  ret->size = maxmem;
  ret->num_buckets = 512;
  ret->buckets = calloc(512,sizeof(linked_list_t));
  int i=0;
  for (i; i < 512; i++) ret->buckets[i] = LL_create();
  cache->cache->slab_manager = initialize(maxmem);
  printf("Created a new cache with size %lu\n",ret->size);
  return cache;
}

//Copies all "in-use" key/values to a new cache and reassigns the cache pointer
//If expand == 1, this also doubles the size of the cache
void defrag(cache_t cache, uint8_t expand)
{
  cache_real_obj *c = cache->cache;
  uint64_t maxmem = (expand == 1) ? c->size * 2 : c->size;
  printf("   ");
  cache_t new_cache = create_cache(maxmem);
  int i;
  for (i=0; i < c->num_buckets; i++)
    {
      node* current = c->buckets[i]->head;
      while (current != NULL)
	{
	  cache_set(new_cache, current->meta->key, current->meta->address, current->meta->size);
	  current = current->next;
	}
    };
  printf("   ");
  destroy_cache_obj(c);
  cache->cache = new_cache->cache;
  free(new_cache);
}

meta_t create_meta(cache_t cache, _key_t key, uint32_t val_size)
{
  meta_t next_meta = malloc(sizeof(meta_obj));
  strcopy(key, next_meta->key);
  next_meta->size = val_size;
  next_meta->allocated = 1;
  next_meta->timer = 0;
}

//increments the LRU timer for every meta object
void bucket_timer_up(cache_t cache){
  cache_real_obj *c = cache->cache;
  int j = 0;
  for (j; j < c->num_buckets; j++)
  {
    node* current_node = c->buckets[j]->head;
    while (current_node != NULL)
    {
      current_node->meta->timer++;
      current_node = current_node->next;
    }
  }
}

//prints the non-null hash buckets
void print_buckets(cache_t cache){
  cache_real_obj *c = cache->cache;
  int j = 0;
  printf("PRINTING BUCKETS\n");
  for (j; j < c->num_buckets; j++){
    linked_list_t curr_LL = c->buckets[j];
    node* current_node = curr_LL->head;
    if (current_node != NULL){
      printf("Bucket %i contains ",j);
      LL_print(curr_LL);
    }
  }
}
    


//slab_class is the slab class that we need to evict in order to make room for the incoming value
void cache_evict(cache_t cache,uint32_t slab_class){
  cache_real_obj *c = cache->cache;
  uint8_t curr_max = 0;
  node* max_node = NULL;
  int i = 0;
  for (i; i < c->num_buckets; i++){
    node* current_node = c->buckets[i]->head;
    while(current_node != NULL){
      if (  (current_node->meta->timer >= curr_max) && 
	    (slab_class <= get_slab_class(c->slab_manager, current_node->meta->size)) 
         ){
	    curr_max = current_node->meta->timer;
	    max_node = current_node;
	  }
    }
    if (max_node == NULL)
      {
	printf("Couldn't find a value to evict.\n");
	return;
      }
  }
  printf("Evicting %s\n",max_node->meta->key);
  cache_delete(cache,max_node->meta->key);
  return;
}

//Add a <key,value> pair to the cache
//If key already exists, overwrite the old value
//If maxmem capacity is exceeded, values will be removed
void cache_set(cache_t cache, _key_t key, val_t val, uint32_t val_size)
{
  cache_real_obj *c = cache->cache;

  //Delete the value if it's already in the cache.
  meta_t old = get_key_loc(cache,key);
  if (old != NULL) cache_delete(cache, key);

  uint64_t available_memory = cache->cache->size - cache_space_used(cache);
  printf("Trying to add a value of size %"PRIu32", with available memory %"PRIu64"\n",val_size,available_memory);
  if (available_memory < val_size)
    {
      printf("   Increasing size.\n");
      defrag(cache, 1); //This doubles the cache size (defragmenting at the same time).
    }
  bucket_timer_up(cache);
  //Create a new meta object and pair it to a slab address, and copy the value over
  meta_t next_meta = create_meta(cache,key,val_size);
  next_meta->address = get_address(c->slab_manager,val_size);
  //enact eviction policy if we need space  
  if (next_meta->address == NULL){
    uint32_t val_slab_class = get_slab_class(c->slab_manager, val_size);
    cache_evict(cache, val_slab_class);
    next_meta->address = get_address(c->slab_manager,val_size);
    if (next_meta->address == NULL){
      uint32_t slab_class = get_slab_class(c->slab_manager, val_size);
      printf("Couldn't add a %u-%u byte value because there are no slabs of that range, and no free slabs to be allocated\n",slab_class>>1, slab_class);
      free(next_meta);
      return;
    }
  }
 
  memcopy(val, next_meta->address, val_size);

  //Add the meta to the appropriate bucket for key hashing
  linked_list_t bucket = c->buckets[c->hasher(key) % c->num_buckets];
  LL_append(bucket, next_meta);
}

//Retrieve the value associated with key, or NULL if not found
val_t cache_get(cache_t cache, _key_t key, uint32_t *val_size)
{
  bucket_timer_up(cache);
  meta_t key_loc = get_key_loc(cache,key);
  if (key_loc == NULL) 
    {
      return NULL;
    }
  else 
    {
      *val_size = key_loc->size;
      key_loc->timer = 0;
      return key_loc->address;
    }
}



//Delete an object from the cache if it's still there
void cache_delete(cache_t cache, _key_t key)
{
  cache_real_obj* c = cache->cache;
  meta_t key_loc = get_key_loc(cache,key);
  if (key_loc == NULL) 
    {
      printf("Key \"%s\" not found for deletion\n.",key);
      return;
    }
  man_delete(c->slab_manager, key_loc->address);
  uint32_t bucket = c->hasher(key) % c->num_buckets;
  LL_delete(c->buckets[bucket], key);
  free(key_loc);
  return;
}

//Compute the total amount of memory used up by all cache values (not keys)
uint64_t cache_space_used(cache_t cache)
{
  int i=0;
  uint64_t size = 0;
  for (i; i < cache->cache->num_buckets; i++)
    {
      node *current = cache->cache->buckets[i]->head;
      while (current != NULL)
      {
        size += current->meta->size;
	current = current->next;
      }
    }
  return size;
}

void destroy_cache(cache_t cache)
{
  destroy_cache_obj(cache->cache);
  free(cache);
}

void check_cache(cache_t cache)
{
  printf("Cache size: %"PRIu64"\n",cache->cache->size);
}
