// Alex Pan
// 
// test.c
//
// This file defines all my test functions. It contains both the tests
// for my LRU API and my cache API. I built them as I was writing up 
// my implementations, so they are structured as such. The LRU tests
// are independent from the cache tests, but the cache tests depends on 
// the LRU eviction policy to work. Similarly, within my tests for both of 
// these sections, the first test functions are pretty much independent of
// the rest of the API, but the second test functions won't pass unless
// the first ones pass, etc. When running these tests, if anything fails
// we should see a print statement telling us which test case failed
// along with some relevant information showing us what the "unexpected"
// behavior was.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cache.h"

////////////////////////// testing LRU functions ////////////////////////
void testing_lru_add(){
	int failed = 0;
	evict_t e = calloc(1,sizeof(struct evict));
	lru_add(e,0);
	if(e->head->htable_index != 0 ||
		e->tail != NULL ||
		e->head->prev != NULL ||
		e->head->next != NULL){
			printf("Test failed on initalizing first element");
			++failed;
	}
	for(int i = 1;i<10;++i){
		lru_add(e,i);
		if(e->head->htable_index != i ||
			e->tail->htable_index != 0 ||
			e->head->prev != NULL ||
			e->tail->next != NULL){
				printf("%p\n",e->head->prev);
				printf("%p\n",e->tail->next);
				printf("%d\t",e->head->htable_index);
				printf("%d\t",e->tail->htable_index);
				printf("Test failed on initalizing element %d\n",i);
				++failed;
		}
	}
	if (failed==0) printf("Testing lru_add() successful\n");
	else printf("Testing lru_add() failed\n");
	destroy_evict(e);
	e = NULL;
}

void testing_lru_update(){
	int failed = 0;
	evict_t e = calloc(1,sizeof(struct evict));
	for(int i = 0;i<10;++i){
		lru_add(e,i);
	}
	lru_update(e,e->head);
	if(e->head->htable_index != 9 ||
		e->head->next->htable_index!=8||
		e->tail->htable_index != 0 ||
		e->head->prev != NULL ||
		e->tail->next != NULL){
		printf("Test failed on updating first element\n");
	}

	for(int i = 8;i>0;--i){
		node_t n = e->head->next;
		for(int j = 8-i;j>0;--j){
			n = n->next;
		}
		lru_update(e,n);
		if(e->head->htable_index != i ||
			e->head->next->htable_index != i+1||
			e->tail->htable_index != 0 ||
			e->head->prev != NULL ||
			e->tail->next != NULL){
				printf("head %d",e->head->htable_index);
				printf("tail %d",e->tail->htable_index);
				printf("Test failed on updating element %d\n",i);
				++failed;
		}
	}

	lru_update(e,e->tail);
	if(e->head->htable_index != 0 ||
		e->head->next->htable_index != 1 ||
		e->tail->htable_index != 9 ||
		e->head->prev != NULL ||
		e->tail->next != NULL){
			printf("head %d",e->head->htable_index);
			printf("tail %d",e->tail->prev->htable_index);
			printf("tailprev %d",e->tail->htable_index);
			printf("Test failed on updating last element\n");
			++failed;
	}
	if (failed==0) printf("Testing lru_update() successful\n");
	else printf("Testing lru_update() failed\n");

	destroy_evict(e);
	e = NULL;
}

void testing_lru_delete(){
	int failed = 0;
	evict_t e = calloc(1,sizeof(struct evict));
	for(int i = 0;i<10;++i){
		lru_add(e,i);
	}
	//[9,8,7,6,5,4,3,2,1,0]
	for(int i = 0;i<=5;++i){
		node_t n = e->head;
		for(int j=0;j<i;++j){
			n = n->next;
		}
		lru_update(e,n);
	}
	//[3,4,5,6,7,8,9,2,1,0]
	for(int i = 0;i<=3;++i){
		node_t n = e->tail->prev->prev->prev;
		lru_update(e,n);
	}
	//[7,8,9,3,4,5,6,2,1,0]
	for(int i = 0;i<2;++i){
		lru_delete(e,e->tail);
		if (e->head->htable_index!= 7 ||
			e->tail->htable_index!= i+1){
				printf("%d\n",i);
				printf("head %d",e->head->htable_index);
				printf("tail %d",e->tail->htable_index);
				printf("Test failed on deleting element from end\n");
				++failed;
		}
	}
	lru_delete(e,e->tail);
	for(int i = 3;i<6;++i){
		lru_delete(e,e->head->next->next->next);
		if (e->head->htable_index!= 7 ||
			e->tail->htable_index!= 6){
				printf("head %d",e->head->htable_index);
				printf("tail %d",e->tail->htable_index);
				printf("Test failed on deleting element from middle\n");
				++failed;
		}
	}
	for(int i = 7;i<9;++i){
		lru_delete(e,e->head);
		if (e->head->htable_index!= i+1 ||
			e->tail->htable_index!= 6){
				printf("head %d",e->head->htable_index);
				printf("tail %d",e->tail->htable_index);
				printf("Test failed on deleting element from head\n");
				++failed;
		}
	}
	lru_delete(e,e->head);
	lru_delete(e,e->tail);
	if (e->head==NULL||
		e->tail!=NULL){
		printf("Test failed on trying to delete tail from queue of one");
		++failed;
	}
	lru_delete(e,e->head);
	if (e->head!=NULL||
		e->tail!=NULL){
		printf("Test failed on clearing queue");
		++failed;
	}


	if (failed==0) printf("Testing lru_delete() successful\n");
	else printf("Testing lru_delete() failed\n");

	destroy_evict(e);
	e=NULL;
}

void testing_create_evict(){
	int failed = 0;
	evict_t e = create_evict(NULL,NULL,NULL);
	if (e->head!=NULL){
		printf("NULL on head failed\n");
		++failed;
	} 
	if (e->tail!=NULL){
		printf("NULL on tail failed\n");
		++failed;
	}
	if (failed==0) printf("Testing create_evict() successful\n");
	else printf("Testing create_evict() failed\n");
	destroy_evict(e);
	e = NULL;
}

void testing_create_cache(){
	int failed = 0;
	int size = 100;
	cache_t c = create_cache(size,NULL);

	char * current = "checking maxmemory";
	if(c->maxmemory!=size){
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "checking number of buckets";
	if(c->buckets!=size){
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "checking amount of occupied buckets";
	if (c->occupiedbuckets!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "checking amount of memory";
	if(c->maxmemory!=size){
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "checking amount of occupied memory";
	if (c->occupiedmemory!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	} 
	if (failed==0) printf("Testing create_cache() successful\n");
	else printf("Testing create_cache() failed\n");

	destroy_cache(c);
	c = NULL;
}


void testing_cache_set(){
	int failed = 0;
	cache_t c = create_cache(100,NULL);
	char * current = "setting first element (str)";
	cache_set(c,(key_type)"1",(val_type)"val1",5);
	if(strcmp(c->keyvals[(c->hashf)((key_type)"1")%(c->buckets)].val,"val1")!=0 ||
		c->lru->head->htable_index != 17 ||
		c->lru->tail != NULL ||
		c->lru->head != c->keyvals[17].lru_node ||
		c->occupiedbuckets != 1){
		printf("Test failed on: %s\n",current);
		printf("%s\n",c->keyvals[(c->hashf)((key_type)"1")%(c->buckets)].val);
		printf("%d\n",c->lru->head->htable_index);
		printf("%d\n",c->keyvals[17].lru_node->htable_index);
		printf("%d\n",c->occupiedbuckets);
		++failed;
	}

	current = "setting second element (int)";
	int a = 2;
	int * ap = &a;
	cache_set(c,(key_type)"22",(val_type)ap,sizeof(ap));
	if( *(int*)c->keyvals[(c->hashf)((key_type)"22")%(c->buckets)].val != 2 ||
		c->lru->head->htable_index!=34 ||
		c->lru->tail->htable_index!=17 ||
		c->occupiedbuckets != 2){
		printf("Test failed on: %s\n",current);
		printf("%d\n",*(int*)c->keyvals[(c->hashf)((key_type)"22")%(c->buckets)].val);
		printf("%d\n",c->lru->head->htable_index );
		++failed;
	}

	current = "resetting a value in the cache";
	cache_set(c,(key_type)"1",(val_type)"33",3);
	if( strcmp(c->keyvals[(c->hashf)((key_type)"1")%(c->buckets)].val,"33")!=0 ||
		c->lru->head->htable_index != 17 ||
		c->lru->tail->htable_index != 34 ||
		c->occupiedbuckets != 2){
		printf("Test failed on: %s\n",current);
		printf("%s\n",c->keyvals[(c->hashf)((key_type)"1")%(c->buckets)].val);
		printf("%d\n",c->lru->head->htable_index);
		++failed;
	}

	current = "overloading the cache's load factor";
	cache_set(c,(key_type)"3",(val_type)"val3",5);
	cache_set(c,(key_type)"4",(val_type)"val4",5);
	cache_set(c,(key_type)"5",(val_type)"val5",5);
	cache_set(c,(key_type)"6",(val_type)"val6",5);
	if( c->lru->head->htable_index != 21 ||
		c->lru->head->next->htable_index != 20 ||
		c->lru->head->next->next->htable_index != 19 ||
		c->lru->head->next->next->next->htable_index != 18 ||
		c->lru->tail->prev->htable_index != 17 ||
		c->lru->tail->htable_index != 34 ||
		c->occupiedbuckets != 6 ||
		c->buckets != 100){
		printf("Test failed on: %s\n",current);
		printf("%d\n",c->lru->head->htable_index);
		printf("%d\n",c->lru->head->next->htable_index);
		printf("%d\n",c->lru->head->next->next->htable_index);
		printf("%d\n",c->lru->head->next->next->next->htable_index);
		printf("%d\n",c->lru->tail->prev->htable_index);
		printf("%d\n",c->lru->tail->htable_index);
		++failed;
	}
	current = "overloading the cache's maxmem";
	cache_set(c,(key_type)"7",(val_type)"123456789012345678901234567890\
		123456789012345678901234567890123456789012345678901234567890",91);

	if( c->lru->head->htable_index != 17 ||
		c->lru->head->next->htable_index != 21 ||
		c->lru->tail->htable_index != 21 ||
		c->occupiedbuckets != 2 ||
		c->buckets != 100){
		printf("Test failed on: %s\n",current);
		printf("%d\n",c->lru->head->htable_index);
		printf("%d\n",c->lru->tail->htable_index);
		++failed;
	}
	if (failed==0) printf("Testing cache_set() successful\n");
	else printf("Testing cache_set() failed\n");
	destroy_cache(c);
	c = NULL;
}

void testing_cache_get(){
	int failed = 0;
	cache_t c = create_cache(100,NULL);

	char * current = "getting a val from an empty cache";
	uint8_t a[5];

	if(cache_get(c,(key_type)"1")!=NULL){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "getting a val from cache w/ one element";
	cache_set(c,(key_type)"1",(val_type)"val1",5);
	strcpy(a,cache_get(c,(key_type)"1"));
	if(strcmp(a,"val1")!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	cache_set(c,(key_type)"2",(val_type)"val2",5);
	cache_set(c,(key_type)"3",(val_type)"val3",5);
	cache_set(c,(key_type)"4",(val_type)"val4",5);
	cache_set(c,(key_type)"5",(val_type)"val5",5);

	current = "getting an old val from cache w/ multiple elements";
	strcpy(a,cache_get(c,(key_type)"1"));
	if(strcmp(a,"val1")!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "getting a val from cache w/ multiple elements";
	strcpy(a,cache_get(c,(key_type)"3"));
	if(strcmp(a,"val3")!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	cache_set(c,(key_type)"6",(val_type)"val6",5);
	current = "getting a new val from cache after resize";
	strcpy(a,cache_get(c,(key_type)"6"));
	if(strcmp(a,"val6")!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	cache_set(c,(key_type)"2",(val_type)"val6",5);
	current = "getting an old val from cache after resize";
	strcpy(a,cache_get(c,(key_type)"2"));
	if(strcmp(a,"val6")!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	int x = 2;
	int * xp = &x;
	cache_set(c,(key_type)"7",(val_type)xp,sizeof(x));
	cache_set(c,(key_type)"6",(val_type)"val6",5);
	current = "getting a new val (int) from cache after resize";
	int * p = cache_get(c,(key_type)"7");
	if(*p != x){
		printf("%d",*p);
		printf("Test failed on: %s\n",current);
		++failed;
	}
	
	if (failed==0) printf("Testing cache_get() successful\n");
	else printf("Testing cache_get() failed\n");
	destroy_cache(c);
}

void testing_cache_delete(){
	int failed = 0;
	cache_t c = create_cache(100,NULL);

	char * current = "deleting a val empty cache";
	cache_delete(c,(key_type)"1");
	for(int i=0;i<c->buckets;++i){
		if(c->keyvals[i].key!=NULL||
			c->keyvals[i].val!=NULL){
		printf("Test failed on: %s\n",current);
		++failed;
		}
	}
	cache_set(c,(key_type)"1",(val_type)"val1",5);
	cache_set(c,(key_type)"2",(val_type)"val2",5);
	cache_set(c,(key_type)"3",(val_type)"val3",5);
	cache_set(c,(key_type)"4",(val_type)"val4",5);
	cache_set(c,(key_type)"5",(val_type)"val5",5);

	current = "deleting the first inserted val in a cache of multiple elements";
	cache_delete(c,(key_type)"1");
	if(cache_get(c,(key_type)"1")!=NULL){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "deleting the last inserted val in a cache of multiple elements";
	cache_delete(c,(key_type)"5");
	if(cache_get(c,(key_type)"1")!=NULL){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "deleting a val from a resized (by overload) cache";
	cache_set(c,(key_type)"1",(val_type)"val1",5);
	cache_set(c,(key_type)"5",(val_type)"val5",5);
	cache_set(c,(key_type)"6",(val_type)"val6",5);
	cache_delete(c,(key_type)"6");
	if(cache_get(c,(key_type)"6")!=NULL||
		c->occupiedbuckets != 5 ||
		c->buckets != 100){
		printf("%llu\n",c->occupiedbuckets);
		printf("%llu\n",c->buckets);
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "automatic deletion from maxmemory overload";
	cache_set(c,(key_type)"7",(val_type)"123456789012345678901234567890\
		123456789012345678901234567890123456789012345678901234567890",91);
	//everything except for this and the most recent element in queue
	//should be evicted because of the memory overload. currently
	//the most recent element is under the key "5".
	if(cache_get(c,(key_type)"1")!=NULL||
		c->lru->head->htable_index!=17 ||
		c->buckets != 100 ||
		c->occupiedbuckets != 2){
		printf("%d\n",c->lru->head->htable_index);
		printf("%d\n",c->lru->tail->htable_index);
		printf("%llu\n",c->buckets);
		printf("%llu\n",c->occupiedbuckets);
		printf("Test failed on: %s\n",current);
		++failed;
	}

	if (failed==0) printf("Testing cache_delete() successful\n");
	else printf("Testing cache_delete() failed\n");
	destroy_cache(c);
}

void testing_cache_space_used(){
	int failed = 0;
	cache_t c = create_cache(100,NULL);

	char * current = "memory used by empty cache";
	uint64_t size = cache_space_used(c);
	if(size!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "memory used by empty cache after deleting a (nonexistant)";
	cache_delete(c,(key_type)"1");
	size = cache_space_used(c);
	if(size!=0){
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "memory used after inserting str \"val1\" (size 5)";
	cache_set(c,(key_type)"1",(val_type)"val1",5);
	size = cache_space_used(c);
	if(size!=5){
		printf("%llu\n",size);
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "memory used after deleting str \"val1\" (size 5)";
	cache_delete(c,(key_type)"1");
	size = cache_space_used(c);
	if(size!=0){
		printf("%llu\n",size);
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "memory used after inserting 5 elements of size 5";
	cache_set(c,(key_type)"1",(val_type)"val1",5);
	cache_set(c,(key_type)"2",(val_type)"val2",5);
	cache_set(c,(key_type)"3",(val_type)"val3",5);
	cache_set(c,(key_type)"4",(val_type)"val4",5);
	cache_set(c,(key_type)"5",(val_type)"val5",5);
	size = cache_space_used(c);
	if(size!=25){
		printf("%llu\n",size);
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "memory used after resize (by overload)";
	cache_set(c,(key_type)"6",(val_type)"val6",5);
	size = cache_space_used(c);
	if(size!=30){
		printf("%llu\n",size);
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "memory used by deleting a val from \
	a resized (by overload) cache";	
	cache_delete(c,(key_type)"6");
	size = cache_space_used(c);
	if(size!=25){
		printf("%llu\n",size);
		printf("Test failed on: %s\n",current);
		++failed;
	}
	current = "memory used after maxmemory overload and one automatic \
	deleting from LRU eviction";
	cache_set(c,(key_type)"7",(val_type)"123456789012345678901234567890\
		12345678901234567890123456789012345",76);
	size = cache_space_used(c);
	if(size!=96 ||
		c->occupiedbuckets != 5){
		printf("%llu\n",size);
		printf("%llu\n", c->occupiedbuckets);
		printf("Test failed on: %s\n",current);
		++failed;
	}

	current = "memory used after maxmemory overload and several automatic \
	deletions from LRU eviction";
	cache_set(c,(key_type)"7",(val_type)"123456789012345678901234567890",31);
	size = cache_space_used(c);
	if(size!=31 ||
		c->occupiedbuckets != 1){
		printf("%llu\n",size);
		printf("%llu\n", c->occupiedbuckets);
		printf("Test failed on: %s\n",current);
		++failed;
	}

	if (failed==0) printf("Testing cache_space_used() successful\n");
	else printf("Testing cache_space_used() failed\n");
	destroy_cache(c);
}
