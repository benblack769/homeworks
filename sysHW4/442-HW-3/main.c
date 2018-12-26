#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cache.h"																																																																																																																																																																																														

cache_t cache, cache2;
uint64_t test, test2;
uint64_t capacity;
uint32_t value_size = 0;
key_type ptra="1", ptrb="2", ptrc="3", ptrd="4", ptre="5", ptrf="6", ptrg="7", ptrh="8", ptri="9", ptrj="10", ptrk="11", ptrl="12", ptrm="13", ptrn="14", ptro="15", ptrp="16", ptrq="17", ptrr="18";
val_type vala="Hi, my name is a.", valb="b!", valc="c, nice to meet you!", vald="d for dog.", vale="e for elephants. pretty cool, right?", valf="fmlfmlfmflmflmflmflmflfmflmflmflml",valg="goodness gracious",valh="happyhappyjoyjoyhappyhappyjoyjoy",vali="ily<3",valj="jokes aside, i hope you like these text messages. i put soooo much thought into them.",valk="killerrrrmillleeerrrr",vall="lol",valm="mangosaremynumber1fruitofalltime",valn="nope.avi",valo="omg",valp="pointers suck",valq="qs are delicious",valr="r's the name, don't ever use me!";

void test_resize() {
	printf("Begin testing of the resizing features.\n");
	cache2 = create_cache(8, NULL);
	cache_set(cache2,ptrb,(val_type)"012345678901234567",strlen("012345678901234567"));
	cache_set(cache2,ptrc,(val_type)"0123456789",strlen("0123456789"));
	print_cache(cache2);
	val_type tester = cache_get(cache2,ptrb,&value_size);
	if (tester) {
		printf("%s\n",(char*)tester);
		printf("We still found the value after applying the new resizing algorithm!\n");
	} else {
		printf("%p\n",tester);
		printf("Test failed... fix this! Exiting...\n");
		exit(-1);
	}
	cache_set(cache2,ptrn,valn,strlen(valn));
	print_cache(cache2);
	printf("Hopefully the number of resizes (currently %d) went from 0 to 1!\n",get_resizes(cache2));
	printf("End testing of the resizing features.\n");
}


void test_evict() {
	printf("Begin testing of the evict features.\n");
	cache2 = create_cache(64, NULL);
	cache_set(cache2,ptrb,valb,strlen(valb));
	print_cache(cache2);
	printf("If we add an item that is 63 bytes large to a 64 byte cache, we should evict the first item, size of 2 bytes (2+63 > 64).\n");
	key_type temp = "20";
	val_type tempv = "012345678901234567890123456789012345678901234567890123456789012";
	cache_set(cache2,temp,tempv,strlen(tempv));
	print_cache(cache2);
	printf("Only 1 item should be printed above.\n");
	printf("End testing of the evict features.\n");
}

uint64_t test_hash(key_type key) {
	uint64_t h = 5381;

	while (*key) {
		h = 33 * h ^ *key++;
	}
	return h;
}


void test_hash_function() {
	printf("Begin testing for cache hash func.\n");
	cache2 = create_cache(capacity, &test_hash);
	test2 = test_hash(ptra);
	test = get_hashfunc(cache2,ptra);
	printf("End testing for cache hash func. Result (these two functions should equal): \n");
	printf("get_hashfunc(cache2,ptra) = %lu, test_hash(ptra) = %lu.\n",test,test2);
	assert(test == test2 && "The hash functions are not equal! Hash functions are NOT getting pasted to caches correctly; fix this! Exiting...\n");
}

uint64_t test_hash_collision(key_type key)
{
	uint64_t h = 1;
	return h;
}

void test_collision_case() {
	printf("Begin testing for collisions.\n");
	cache2 = create_cache(capacity, &test_hash_collision);
	cache_set(cache2,ptra,vala,strlen(vala));
	cache_set(cache2,ptrb,valb,strlen(valb));
	cache_set(cache2,ptrc,valc,strlen(valc));
	cache_set(cache2,ptrd,vald,strlen(vald));
	cache_set(cache2,ptre,vale,strlen(vale));
	cache_set(cache2,ptrf,valf,strlen(valf));
	cache_set(cache2,ptrg,valg,strlen(valg));
	cache_set(cache2,ptrh,valh,strlen(valh));
	cache_set(cache2,ptri,vali,strlen(vali));
	cache_set(cache2,ptrj,valj,strlen(valj));
	cache_set(cache2,ptrk,valk,strlen(valk));
	cache_set(cache2,ptrl,vall,strlen(vall));
	cache_set(cache2,ptrm,valm,strlen(valm));
	cache_set(cache2,ptrn,valn,strlen(valn));
	cache_set(cache2,ptro,valo,strlen(valo));
	cache_set(cache2,ptrp,valp,strlen(valp));
	print_cache(cache2);
	printf("The hash is a constant = 1, so each key/value pairing should be in Node 1.\n");
	printf("End testing for collisions.\n");
}

void test_create_cache() {
	printf("Begin testing for \"create_cache\".\n");
	cache = create_cache(capacity,NULL);
	assert(cache && "Allocation of new cache failed! That's a problem, fix it! Exiting...");
	printf("Finished making the cache successfully.\n");
	print_cache(cache);
	printf("End testing for \"create_cache\".\n");
}

void test_cache_set() {
	printf("Begin testing for \"cache_set\".\n");
	cache_set(cache,ptra,vala,strlen(vala));
	cache_set(cache,ptrb,valb,strlen(valb));
	cache_set(cache,ptrc,valc,strlen(valc));
	cache_set(cache,ptrd,vald,strlen(vald));
	cache_set(cache,ptre,vale,strlen(vale));
	cache_set(cache,ptrf,valf,strlen(valf));
	cache_set(cache,ptrg,valg,strlen(valg));
	cache_set(cache,ptrh,valh,strlen(valh));
	cache_set(cache,ptri,vali,strlen(vali));
	cache_set(cache,ptrj,valj,strlen(valj));
	cache_set(cache,ptrk,valk,strlen(valk));
	cache_set(cache,ptrl,vall,strlen(vall));
	cache_set(cache,ptrm,valm,strlen(valm));
	cache_set(cache,ptrn,valn,strlen(valn));
	cache_set(cache,ptro,valo,strlen(valo));
	cache_set(cache,ptrp,valp,strlen(valp));
	print_cache(cache);
	printf("Updating values for keys %s, %s, and %s.\n",ptre,ptrl,ptrg);
	cache_set(cache,ptre,(val_type)"ELEPHANTS SUCK!",strlen("ELEPHANT SUCK!"));
	cache_set(cache,ptrl,(val_type)"Life is not funny any longer.",strlen("Life is not funny any longer."));
	cache_set(cache,ptrg,(val_type)"Go away, I'm sad.",strlen("Go away, I'm sad."));
	print_cache(cache);
	printf("End testing for \"cache_set\".\n");
}

void test_cache_get() {
	printf("Begin testing for \"cache_get\".\n");
	val_type val1 = cache_get(cache,ptra,&value_size);
	if (val1) {
		printf("%s\n",(char*)val1);
	} else {
		printf("%p\n",val1);
	}
	val_type val2 = cache_get(cache,ptrb,&value_size);
	if (val2) {
		printf("%s\n",(char*)val2);
	} else {
		printf("%p\n",val2);
	}
	val_type val3 = cache_get(cache,ptrc,&value_size);
	if (val3) {
		printf("%s\n",(char*)val3);
	} else {
		printf("%p\n",val3);
	}
	val_type val4 = cache_get(cache,ptrd,&value_size);
	if (val4) {
		printf("%s\n",(char*)val4);
	} else {
		printf("%p\n",val4);
	}
	val_type val5 = cache_get(cache,ptri,&value_size);
	if (val5) {
		printf("%s\n",(char*)val5);
	} else {
		printf("%p\n",val5);
	}
	val_type val6 = cache_get(cache,ptrh,&value_size);
	if (val6) {
		printf("%s\n",(char*)val6);
	} else {
		printf("%p\n",val6);
	}
	val_type val7 = cache_get(cache,ptrl,&value_size);
	if (val7) {
		printf("%s\n",(char*)val7);
	} else {
		printf("%p\n",val7);
	}
	printf("End testing for \"cache_get\".\n");
}

void test_cache_not_inserted_query () {
	printf("Begin testing for cache_not_inserted_query.\n");
	val_type a_val = cache_get(cache,ptra,&value_size);
	val_type no_val = cache_get(cache,ptrq,&value_size);
	if(a_val) {
		printf("I found the value for key \"%s\"! \"%s\"\n",ptra,(char*)a_val);
	}
	else {
		printf("I didn't find a value there... that's a problem, fix it! Exiting...\n");
		exit(-1);
	}
	if(no_val) {
		printf("Key \"%s\" has value \"%s\"... wait, this shouldn't have printed! That's a problem, fix it! Exiting...\n",ptrq,(char*)no_val);
		exit(-1);
	}
	else {
		printf("I didn't find a value there for key \"%s\"... that's a good thing!\n",ptrq);
	}
	printf("End testing for cache_not_inserted_query.\n");
}


void test_cache_inserted_then_deleted() {
	printf("Begin testing for insertion, then deletion.\n");
	printf("Current Cache space used: %lu\n",cache_space_used(cache));
	cache_set(cache,ptrq,(val_type)"shortlived",strlen("shortlived"));
	printf("%s\n",(char*)cache_get(cache,ptrq,&value_size));
	printf("Current Cache space used: %lu\n",cache_space_used(cache));
	print_cache(cache);
	cache_delete(cache,ptrq);
	printf("And after deleting the newly added key...\n");
	print_cache(cache);
	if(cache_get(cache,ptrq,&value_size)) {
		printf("Key \"%s\" has value \"%s\"... wait, this shouldn't have printed! That's a problem, fix it! Exiting...\n",ptrq,(char*)valq);
		exit(-1);
	}
	else {
		printf("I didn't find a value there for key \"%s\"... that's a good thing!\n",ptrq);
	}
	printf("Current Cache space used: %lu\n",cache_space_used(cache));
	printf("End testing for insertion, then deletion.\n");
}


void test_cache_delete() {
	printf("Begin testing for \"cache_delete\".\n");
	printf("Current Cache space used: %lu\n",cache_space_used(cache));
	printf("%s\n",(char*)cache_get(cache,ptri,&value_size));
	print_cache(cache);
	cache_delete(cache,ptri);
	print_cache(cache);
	if(cache_get(cache,ptri,&value_size)) {
		printf("Key \"%s\" has value \"%s\"... wait, this shouldn't have printed! That's a problem, fix it! Exiting...\n",ptri,(char*)vali);
		exit(-1);
	}
	else {
		printf("I didn't find a value there for key \"%s\"... that's a good thing!\n",ptri);

	}
	printf("Current Cache space used: %lu\n",cache_space_used(cache));
	cache_delete(cache,ptra);
	print_cache(cache);
	printf("LRU should now be 2 rather than 1.\n");
	printf("End testing for \"cache_delete\".\n");
}

void test_cache_delete_not_inserted() {
	printf("Begin testing for deleting a item not in the cache.\n");
	printf("Current Cache space used: %lu\n",cache_space_used(cache));
	if(cache_get(cache,ptrr,&value_size)) {
		printf("Key \"%s\" has value \"%s\"... wait, this shouldn't have printed! That's a problem, fix it! Exiting...\n",ptrr,(char*)valr);
		exit(-1);
	}
	else {
		printf("I didn't find a value there for key \"%s\"... sounds good so far!\n",ptrr);
	}
	cache_delete(cache,ptrr);
	if(cache_get(cache,ptrr,&value_size)) {
		printf("Key \"%s\" has value \"%s\"... wait, this shouldn't have printed! That's a problem, fix it! Exiting...\n",ptrr,(char*)valr);
		exit(-1);
	}
	else {
		printf("I STILL didn't find a value there for key \"%s\"... that's a good thing!\n",ptrr);
	}
	printf("Current Cache space used: %lu\n",cache_space_used(cache));
	printf("End testing for deleting a item not in the cache.\n");

}

void test_cache_space_used() {
	printf("Begin testing for \"cache_space_used\".\n");
	printf("%lu\n",cache_space_used(cache));
	printf("End testing for \"cache_space_used\".\n");
}

void test_destroy_cache() {
	printf("Begin testing for \"destroy_cache\".\n");
	destroy_cache(cache);
	cache = NULL;
	assert(!cache && "The cache still exists! That's a problem, fix it! Exiting...");
	printf("The cache was successfully deleted!\n");
	printf("End testing for \"destroy_cache\".\n");
}


int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Exactly one argument expected; the memory size for cache. Exiting...\n");
		exit(-1);
	}

	capacity = atoi(argv[1]);

	srand(time(NULL));

	test_hash_function();
	test_resize();
	test_evict();
	test_collision_case();
	destroy_cache(cache2);
	test_create_cache();
	test_cache_set();
	test_cache_not_inserted_query();
	test_cache_inserted_then_deleted();
	test_cache_get();
	test_cache_delete();
	test_cache_delete_not_inserted();
	test_cache_space_used();
	test_destroy_cache();
	printf("I am done yay happiness!\n");

	return 0;
}

