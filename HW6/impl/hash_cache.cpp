#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "cache.h"
#include "helper.h"
#include "replacement.h"

struct key_val_obj{
    key_type key;
    val_type val;
    uint32_t val_size;

    pinfo_t policy_info;
};
typedef struct key_val_obj key_val_s;
struct link_obj;
typedef struct link_obj* link_t;
struct link_obj{
    key_val_s data;
    link_t next;
    link_t * prev_next_ptr;//the pointer to the thing pointing to it
};
void del_link(cache_t cache, link_t obj);
void release_link_data(cache_t cache,link_t obj);
void extract_link(link_t obj);

uint64_t def_hash_fn(key_type key);
size_t map_to_location(uint64_t hash_val,size_t table_size){
    return hash_val % table_size;
}

struct cache_obj{
    uint64_t maxmem;
    uint64_t mem_used;
    link_t * table;
    size_t table_size;
    size_t num_elements;
    hash_func h_fn;
    policy_t evic_policy;
};
const size_t default_table_size = 2503;//medium size prime number

struct user_identifier{//declared in replacement.h
    link_t * linkpp;
};

cache_t create_cache(uint64_t maxmem,hash_func h_fn){
    cache_t n_cache = new(cache_obj);

    n_cache->maxmem = maxmem;
    n_cache->mem_used = 0;
    n_cache->table_size = default_table_size;
    n_cache->num_elements = 0;
    n_cache->table = (link_t *)(calloc(default_table_size,sizeof(key_val_s)));
    n_cache->h_fn = (h_fn == NULL) ? def_hash_fn : h_fn;
    n_cache->evic_policy = create_policy(maxmem);

    return n_cache;
}

link_t * querry_hash(cache_t cache, key_type key){
    //returns the pointer to the pointer of the key, if the key exists,
    // and the pointer to the location the key would be, if it were added, if it is not there
    size_t hash_loc = map_to_location(cache->h_fn(key),cache->table_size);
    link_t * cur_item = &cache->table[hash_loc];
    while(*cur_item != NULL && strcmp((char*)(key),(char*)((*cur_item)->data.key))){
        cur_item = &(*cur_item)->next;
    }
    return cur_item;
}
void resize_table(cache_t cache,uint64_t new_size){
    const size_t old_t_size = cache->table_size;
    link_t * old_table = cache->table;

    cache->table = (link_t  *)calloc(new_size,sizeof(link_t));
    cache->table_size = new_size;

    //add the pointers into the new cache table without changing the data at all
    for(size_t i = 0; i < old_t_size;i++){
        link_t cur_l = old_table[i];
        while(cur_l != NULL){
            link_t next_l = cur_l->next;

            link_t * hash_loc = querry_hash(cache,cur_l->data.key);
            *hash_loc = cur_l;
            cur_l->next = NULL;
            cur_l->prev_next_ptr = hash_loc;

            cur_l = next_l;
        }
    }
    free(old_table);
}
void assign_to_link(link_t * linkp,key_val_s data){
    if(*linkp == NULL){
        *linkp = (link_t)calloc(1,sizeof(struct link_obj));
    }
    (*linkp)->data = data;
    (*linkp)->prev_next_ptr = linkp;
}
void add_to_cache(cache_t cache,link_t link, key_type key, val_type val, uint32_t val_size){
    //adds in a new item in the location of the cache
    key_type key_copy = (key_type)make_copy(key,strlen((char*)key)+1);

    key_val_s new_item;
    new_item.key = key_copy;
    new_item.val = make_copy(val,val_size);
    new_item.val_size = val_size;
    //give the policy the pointer to the key so that it can read the value of the key when
    //the policy teels it about evictions in ids_to_delete_if_added
    new_item.policy_info = create_info(cache->evic_policy,(void*)(link),val_size);

    link->data = new_item;

    cache->mem_used += val_size;
    cache->num_elements++;

    //resizes table if load factor is over 0.5
    if(cache->num_elements*2 > cache->table_size){
        resize_table(cache,cache->table_size*2);
    }
}
void make_room_for(cache_t cache,uint32_t val_size){
    user_id_t id;
    while(should_pop_this(cache->evic_policy,val_size,&id)){
        link_t link = (link_t)(id);
        del_link(cache,link);
    }
}

/*
 * cache set is very careful that maxmem is never exceeded. In order to do this while
 * maintaining a single querry_hash call, it makes a placeholder object that is invalid
 * while it deletes things as requested by LRU. This way, the position of the new link
 * is maintained by extract_link, and does not need to be recomputed.
 */
void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size){
    link_t * init_link = querry_hash(cache,key);
    if(*init_link != NULL){
        //if the item is already in the list, then free the associated memory (but not the object itself)
        release_link_data(cache,*init_link);
    }
    else{
        //else add the new element, but with empty data
        assign_to_link(init_link,key_val_s());
    }
    link_t mylink = *init_link;//now a valid link in the linked list but with invaid data

    //if the policy tells the cache not to add the item, do not add it, instead remove it from list
    if(should_add(cache->evic_policy,val_size)){
        make_room_for(cache,val_size);
        add_to_cache(cache,mylink,key,val,val_size);
    }
    else{
        //else delete the object
        extract_link(mylink);
        free(mylink);
    }
}
val_type cache_get(cache_t cache, key_type key, uint32_t *val_size){
    link_t hash_l = *querry_hash(cache,key);
    if(hash_l != NULL){
        //if the item is in the cache, tell the policy that fact, and return the value
        info_gotten(cache->evic_policy,hash_l->data.policy_info);
        *val_size = hash_l->data.val_size;
        return hash_l->data.val;
    }
    else{
        //if the item is not in the cache, return NULL
        *val_size = 0;
        return NULL;
    }
}
void release_link_data(cache_t cache,link_t obj){
    if(obj != NULL){
        cache->mem_used -= obj->data.val_size;
        cache->num_elements--;

        free((uint8_t*)obj->data.key);
        free((void *)obj->data.val);

        delete_info(cache->evic_policy,obj->data.policy_info);
    }
}
void extract_link(link_t obj){
    if(obj != NULL){
        //extracts obj from doubly linked list
        *(obj->prev_next_ptr) = obj->next;
        if(obj->next != NULL){
            obj->next->prev_next_ptr = obj->prev_next_ptr;
        }
    }
}

void del_link(cache_t cache,link_t obj){
    //deletes the thing pointed to by the obj and replaces it with the next thing in the linked list  (a NULL if it is at the end)
    extract_link(obj);
    release_link_data(cache,obj);
    if(obj != NULL){
        free(obj);
    }
}
void cache_delete(cache_t cache, key_type key){
    del_link(cache,*querry_hash(cache,key));
}
uint64_t cache_space_used(cache_t cache){
    return cache->mem_used;//should be thread safe (also is not called in networked cache except for in POST destroy, so it should be fine)
}
void destroy_cache(cache_t cache){
    //deletes the links
    for(size_t i = 0; i < cache->table_size; i++){
        while(cache->table[i] != NULL){
            del_link(cache,cache->table[i]);
        }
    }
    delete_policy(cache->evic_policy);
    free(cache->table);
    delete cache;
}
//taken from stack overflow post
uint64_t def_hash_fn(key_type str){
    uint64_t hash = 5381;
    char c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
