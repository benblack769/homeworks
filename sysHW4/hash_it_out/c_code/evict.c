#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "evict.h"

struct evict_obj
{
    // we use a naiive queue to implement LRU
    key_type *queue; // our queue
    uint32_t max_queue_size; // sizeof(queue)
    uint32_t front; // index of top of queue
    uint32_t rear; // index of back of queue
};

evict_t evict_create(uint32_t max_size)
{
    evict_t e = calloc(1, sizeof(struct evict_obj));
    assert(e);
    e->max_queue_size = max_size;
    e->queue = calloc(max_size, sizeof(key_type));
    for (uint32_t i = 0; i < max_size; ++i) {
        e->queue[i] = NULL;
    }
    e->front = 0; 
    e->rear = 0;
    return e;
}

void evict_set(evict_t evict, key_type key) 
{
    // check for resizing queue
    if (evict->rear + 1 == evict->max_queue_size) {
        evict->max_queue_size *= 2;
        evict->queue = realloc(evict->queue, evict->max_queue_size * sizeof(key_type));
        assert(evict->queue && "memory");
    }

    // This is commented out because because it requires linear time and our current 
    // cache API does not rely on it. This section of the code checks if a key is already
    // in the queue datastructure, and if it is, sets the old version to NULL. 
    // If this code is commented out, a key may be be prematurely evicted.
    //
    // check if key already in queue:
    //for (uint32_t i = evict->front; i < evict->rear; ++i) {
    //    if (evict->queue[i] && strcmp((char*) evict->queue[i], (char*) key) == 0) {
    //        free((void*) evict->queue[i]);
    //        evict->queue[i] = NULL;
    //        break;
    //    }
    //}

    // put key on back of queue
    key_type key_copy = calloc(strlen((const char*) key) + 1, sizeof(uint8_t));
    strcpy((char*) key_copy, (char*) key);
    evict->queue[evict->rear] = key_copy;
    ++evict->rear;
}

void evict_get(evict_t evict, key_type key) 
{
    // key has been used, so we need to remove it from the queue, and put it on the back
    bool failed = true;
    for (uint32_t i = evict->front; i < evict->rear; ++i) {
        if (evict->queue[i] &&
                strcmp((char*) evict->queue[i], (char*) key) == 0) {
            // check for resizing queue
            if (evict->rear + 1 == evict->max_queue_size) {
                // double the size of the queue!
                evict->queue = realloc(evict->queue, 2 * evict->max_queue_size * sizeof(key_type));
                for (uint32_t j = evict->max_queue_size; j < 2 * evict->max_queue_size; ++j) {
                    evict->queue[j] = NULL;
                }
                evict->max_queue_size *= 2;
                assert(evict->queue && "memory");
            }

            // place key on rear of queue
            evict->queue[evict->rear] = evict->queue[i];
            ++evict->rear;
            evict->queue[i] = NULL;
            failed = false;
            break;
        }
    }
    if (failed) {
        fprintf(stderr, "key not found\n");
        assert(false);
    }
}

void evict_delete(evict_t evict, key_type key) 
{
    // key has been used, so we need to remove it from the queue, and put it on the back
    bool failed = true;
    for (uint32_t i = evict->front; i < evict->rear; ++i) {
        if (evict->queue[i] &&
                strcmp((char*) evict->queue[i], (char*) key) == 0) {
            // place key on rear of queue
            free((uint8_t*) evict->queue[i]);
            evict->queue[i] = NULL;
            failed = false;
        }
    }
    if (failed) {
        assert(false);
        fprintf(stderr, "key not found in eviction key\n");
    }
}

void evict_destroy(evict_t evict)
{
    for (uint32_t i = 0; i < evict->max_queue_size; ++i) {
        if (evict->queue[i]) {
            free((uint8_t*) evict->queue[i]);
            evict->queue[i] = NULL;
        }
    }
    free(evict->queue);
}

key_type evict_select_for_removal(evict_t evict)
{
    while (evict->front < evict->rear) {
        if (evict->queue[evict->front]) {
            key_type ret_key = calloc(strlen((const char*) evict->queue[evict->front]) + 1, sizeof(uint8_t));
            strcpy((char*) ret_key, (char*) evict->queue[evict->front]);
            return ret_key;
        }
        ++evict->front;
    }

    fprintf(stderr, "no keys to evict\n");
    return NULL;
}













