#include <stdio.h>
#include <assert.h>
#include "cache.h"

//This file contains my tests on my cache.

//For the convenience of test, we use the following datatype and size in our test
#define DATATYPE uint32_t
#define DATASIZE 4

//data_obj keeps a pair of key and value
struct data_obj{
    uint8_t key_pointee;
    DATATYPE value_pointee;
    uint32_t value_size;
    key_type key;
    val_type value;
    uint32_t * value_size_ptr;

};
typedef struct data_obj * data_t;

//It has the following test functions:
//create a new data_obj 
data_t create_data(uint8_t key_pointee, DATATYPE value_pointee, uint32_t value_size);
//free the memory used by data
void destroy_data(data_t data);
//Test the success of creating a new cache and return it back
cache_t test_create_cache(uint32_t maxmem);
//Test when query for a key that was already inserted; 
void test_key_inserted();
//Test when query for a key that wasn't inserted; 
void test_key_not_inserted();
//Test when query for a key that was inserted and modified; 
void test_key_modified();
//Test when query for a key that was inserted and deleted; 
void test_key_deleted();
//Test when query for a key that was evicted;
void test_key_evicted();
//Test when there is a hash_collision 
void test_hash_collision();
//Test when hash_table needs to be resized
void test_resize_hash_table();
//Test LRU;
void test_LRU();
//check if we destroy the cache successfully
void test_destroy_cache(cache_t cache);




//create a new data_obj
data_t create_data(uint8_t key_pointee, DATATYPE value_pointee, uint32_t value_size){
    data_t data = (data_t) malloc( sizeof(struct data_obj));
    data->value_pointee = value_pointee;
    data->key_pointee = key_pointee;
    data->value_size = value_size;
    data->key = &(data->key_pointee);
    data->value = &(data->value_pointee);
    uint32_t zero = 0;
    data->value_size_ptr = &zero;
    return data;
}
//free the memory used by data
void destroy_data(data_t data){
    free(data);
}

//Test the success of creating a new cache and return it back
cache_t test_create_cache(uint32_t maxmem){
    cache_t cache = create_cache(maxmem,NULL,NULL);

    assert(cache!=NULL&&"new cache fails to be allocated");
    assert(cache->maxmem==maxmem&&"maxmem is incorrect");
    return cache;
}

//Test when query for a key that was already inserted; 
void test_key_inserted(){
    cache_t cache = create_cache(3*DATASIZE,NULL,NULL);
    data_t data1 = create_data(1,1,DATASIZE); 


    cache_set(cache,data1->key,data1->value,data1->value_size);
    val_type retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value)==data1->value_pointee&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==data1->value_size&&"retrived value size is incorrect");

    test_destroy_cache(cache);
    destroy_data(data1);
}

//Test when query for a key that wasn't inserted; 
void test_key_not_inserted(){
    cache_t cache = create_cache(3*DATASIZE,NULL,NULL);

    data_t data1 = create_data(1,1,DATASIZE); 
    data_t data2 = create_data(2,2,DATASIZE); 

    cache_set(cache,data1->key,data1->value,data1->value_size);
    val_type retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value)==data1->value_pointee&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==data1->value_size&&"retrived value size is incorrect");

    //Retrive a key that is not inserted
    val_type retrived_value2 = cache_get(cache,data2->key,data2->value_size_ptr); 
    assert(retrived_value2==NULL&&"retrived value is incorrect");
    assert(*data2->value_size_ptr==0&&"retrived value size is incorrect");

    test_destroy_cache(cache);
    destroy_data(data1);
    destroy_data(data2);

}

//Test when query for a key that was inserted and modified; 
void test_key_modified(){
    cache_t cache = create_cache(3*DATASIZE,NULL,NULL);

    data_t data1 = create_data(1,1,DATASIZE); 
    data_t data2 = create_data(1,2,DATASIZE); 
    cache_set(cache,data1->key,data1->value,data1->value_size);

    val_type retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value)==data1->value_pointee&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==data1->value_size&&"retrived value size is incorrect");


    //modify the value with the original key 
    cache_set(cache,data2->key,data2->value,data2->value_size);
    //We are supposed to retrive the new value
    retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value)==data2->value_pointee&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==data2->value_size&&"retrived value size is incorrect");

    test_destroy_cache(cache);
    destroy_data(data1);
    destroy_data(data2);
}

//Test when query for a key that was inserted and deleted; 
void test_key_deleted(){
    cache_t cache = create_cache(3*DATASIZE,NULL,NULL);

    data_t data1 = create_data(1,1,DATASIZE); 
    cache_set(cache,data1->key,data1->value,data1->value_size);


    val_type retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 

    assert(*(DATATYPE *)(retrived_value)==data1->value_pointee&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==data1->value_size&&"retrived value size is incorrect");

    //delete the item 
    cache_delete(cache,data1->key);
    assert(cache_get(cache,data1->key,data1->value_size_ptr)==NULL&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==0&&"retrived value size is incorrect");

    test_destroy_cache(cache);
    destroy_data(data1);
}

//Test when query for a key that was evicted;
void test_key_evicted(){
    cache_t cache = create_cache(1*DATASIZE,NULL,NULL);

    data_t data1 = create_data(1,1,DATASIZE); 
    data_t data2 = create_data(2,2,DATASIZE); 
    cache_set(cache,data1->key,data1->value,data1->value_size);
    
    val_type retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value)==data1->value_pointee&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==data1->value_size&&"retrived value size is incorrect");

    //Add a new data to evict the original one
    cache_set(cache,data2->key,data2->value,data2->value_size);
    val_type retrived_value2 = cache_get(cache,data2->key,data2->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value2)==data2->value_pointee&&"retrived value is incorrect");
    assert(*(data2->value_size_ptr)==data2->value_size&&"retrived value size is incorrect");


    //The original data should have been evicted
    retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 
    assert(retrived_value==NULL&&"retrived value is incorrect");

    test_destroy_cache(cache);
    destroy_data(data1);
    destroy_data(data2);

}

//Test when there is a hash_collision 
void test_hash_collision(){

    cache_t cache = create_cache(5*DATASIZE,NULL,NULL);

    data_t data1 = create_data(1,1,DATASIZE); 
    data_t data2 = create_data(2,2,DATASIZE); 
    
    cache_set(cache,data1->key,data1->value,data1->value_size);
    cache_set(cache,data2->key,data2->value,data2->value_size);

    assert(*(DATATYPE *)(cache_get(cache,data1->key,data1->value_size_ptr))==data1->value_pointee&&"value with key is incorrect");
    assert(*(DATATYPE *)(cache_get(cache,data2->key,data2->value_size_ptr))==data2->value_pointee&&"value2 with key2 is incorrect");
  
    test_destroy_cache(cache);
    destroy_data(data1);
    destroy_data(data2);
}

//Test when hash_table needs to be resized
void test_resize_hash_table(){

    cache_t cache = create_cache(5*DATASIZE,NULL,NULL);

    data_t data1 = create_data(1,1,DATASIZE); 
    data_t data2 = create_data(2,2,DATASIZE); 
    data_t data3 = create_data(3,3,DATASIZE); 
    
    cache_set(cache,data1->key,data1->value,data1->value_size);
    cache_set(cache,data2->key,data2->value,data2->value_size);
    assert(cache->hash_table->buckets_num==5&&"bucket number is incorrect before resizing");
    
    cache_set(cache,data3->key,data3->value,data3->value_size);
    assert(cache->hash_table->buckets_num==10&&"bucket number is incorrect after resizing");  
    
    test_destroy_cache(cache);
    destroy_data(data1);
    destroy_data(data2);
    destroy_data(data3);

}

void test_LRU(){
    cache_t cache = create_cache(2*DATASIZE,NULL,NULL);

    data_t data1 = create_data(1,1,DATASIZE); 
    data_t data2 = create_data(2,2,DATASIZE); 
    data_t data3 = create_data(3,3,DATASIZE); 
    
    cache_set(cache,data1->key,data1->value,data1->value_size);
    cache_set(cache,data2->key,data2->value,data2->value_size);

    //Get key1 to put it to the front of the linked_list
    cache_get(cache,data1->key,data1->value_size_ptr);

    //Set key3 to evict key2
    cache_get(cache,data3->key,data3->value_size_ptr);

    //key1 is supposed to be kept
    cache_set(cache,data1->key,data1->value,data1->value_size);
    val_type retrived_value = cache_get(cache,data1->key,data1->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value)==data1->value_pointee&&"retrived value is incorrect");
    assert(*(data1->value_size_ptr)==data1->value_size&&"retrived value size is incorrect");

    //key2 is supposed to be evicted already
    cache_set(cache,data2->key,data2->value,data2->value_size);
    val_type retrived_value2 = cache_get(cache,data2->key,data2->value_size_ptr); 
    assert(*(DATATYPE *)(retrived_value2)==data2->value_pointee&&"retrived value is incorrect");
    assert(*(data2->value_size_ptr)==data2->value_size&&"retrived value size is incorrect");

    test_destroy_cache(cache);
    destroy_data(data1);
    destroy_data(data2);
    destroy_data(data3);

}


//check if we destroy the cache successfully
void test_destroy_cache(cache_t cache){

    destroy_cache(cache);
    assert(cache->linked_list->current_size==0&&"cache fails to be destoryed");
}

int main(){
    printf("Hello, World! Test begins!\n");
    test_destroy_cache(test_create_cache(DATASIZE));
    test_key_inserted();
    test_key_not_inserted();
    test_key_modified();
    test_key_deleted();
    test_key_evicted();
    test_hash_collision();
    test_resize_hash_table();
    test_LRU();
    printf("All tests pass! Byebye, World!\n");

    return 0;
}