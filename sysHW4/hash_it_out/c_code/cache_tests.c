#include <assert.h>
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>

#include "dbLL_tests.h"
#include "cache.h"

#include "cache_tests.h"

#define my_assert(value, string) \
{if (!(value)) { printf("!!!!FAILURE!!!!! %s\n", string);}}

static void print_key(key_type key)
{
    uint32_t i = 0;
    while (key[i]) {
        printf("%" PRIu32 ", ", key[i]);
        ++i;
    }
    printf("\n");
}

static void test_delete()
{
    printf("Running cache delete test \n");
    cache_t cache = create_cache(10);

    uint8_t a[2] = {'a', '\0'};
    uint8_t b[2] = {'b', '\0'};
    uint8_t c[2] = {'c', '\0'};
    uint8_t val[6] = {10,11,12,13,14,15};

    cache_set(cache, a, val, 6);
    cache_set(cache, b, val, 6);
    cache_set(cache, c, val, 6);

    cache_delete(cache, b);
    cache_delete(cache, a);
    cache_delete(cache, c);

    my_assert(0 == cache_space_used(cache), "not everything was deleted");
    destroy_cache(cache);
}

static void test_mem_overflow()
{
    // test if the cache handles memory overflow correct
    printf("Running cache memory overflow test\n");
    cache_t c = create_cache(10);
    my_assert(cache_space_used(c) == 0, "cache space used initialized incorrectly");
    uint8_t key[2] = {'a', '\0'};
    uint8_t val[6] = {10,11,12,13,14,15};
    cache_set(c, key, val, 6);

    key[0] = 'b';
    uint8_t val2[6] = {20,21,22,23,24,25};
    cache_set(c, key, val2, 6);

    my_assert(6 == cache_space_used(c), "cache space used after a mem overflow is incorrect");
    destroy_cache(c);
}

static void test_set_get()
{
    // A generic test of the cache that randomly generates keys
    // and values, and then sets and gets them from the cache.
    // this test doesn't take evictions into account, so make sure
    // that the cache is created with enough memory

    printf("Running cache set/get test\n");
    uint32_t nsets = rand() % 100;
    uint8_t *saved_keys[nsets];
    uint8_t saved_vals[nsets];

    cache_t c = create_cache(nsets * 10);
    my_assert(cache_space_used(c) == 0, "cache space used initialized incorrectly");

    for (uint32_t i = 0; i < nsets; i++) {
        uint32_t key_size = (rand() % 10) + 2;
        saved_keys[i] = calloc(key_size, sizeof(uint8_t));
        for (uint32_t j = 0; j < key_size - 1; j++) {
            saved_keys[i][j] = rand() % 255;
        }
        saved_keys[i][key_size - 1] = '\0';
        saved_vals[i] = rand() % 100;
        cache_set(c, saved_keys[i], &saved_vals[i], 1);
    }

    for (uint32_t i = 0; i < nsets; i++) {
        uint32_t size;
        val_type v = cache_get(c, (key_type) saved_keys[i], &size);
        if (v != NULL && * (uint8_t *) v != saved_vals[i]) {
            my_assert(false, "test failed!");
        }
        free((void *) v);
    }

    destroy_cache(c);
    for (uint32_t i = 0; i < nsets; i++) {
        free(saved_keys[i]);
        saved_keys[i] = NULL;
    }
}

static void test_duplicate_key()
{
    printf("Running cache duplicate key test\n");
    cache_t c = create_cache(100);
    my_assert(cache_space_used(c) == 0, "cache space is nonzero at initialization");

    uint8_t key[2] = {'a', '\0'};
    uint8_t val[6] = {10,11,12,13,14,15};
    uint8_t val2[4] = {20,21,22,23};
    uint32_t val_size;

    cache_set(c, key, val, 6);
    cache_set(c, key, val2, 4);
    uint8_t *v = (uint8_t*) cache_get(c, key, &val_size);

    my_assert(v[0] == val2[0], "0 incorrect val retrieved after duplicate key insertion");
    my_assert(v[1] == val2[1], "1 incorrect val retrieved after duplicate key insertion");
    my_assert(v[2] == val2[2], "2 incorrect val retrieved after duplicate key insertion");
    my_assert(v[3] == val2[3], "3 incorrect val retrieved after duplicate key insertion");
    
    destroy_cache(c);
}

static void test_space()
{
    printf("Running cache space test\n");
    cache_t c = create_cache(100);
    my_assert(cache_space_used(c) == 0, "cache space is nonzero at initialization");
    uint8_t key[2] = {'a', '\0'};
    uint8_t val[6] = {10,11,12,13,14,15};
    cache_set(c, key, val, 6);

    key[0] = 'b';
    uint8_t val2[4] = {20,21,22,23};
    cache_set(c, key, val2, 4);

    my_assert(10 == cache_space_used(c), "cache_space_used failed");

    destroy_cache(c);
}

void cache_tests()
{
    printf("***Running cache tests***\n");
    test_set_get();
    test_mem_overflow();
    test_duplicate_key();
    test_space();
    test_delete();
}


