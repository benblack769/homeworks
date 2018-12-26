#pragma once
#include <inttypes.h>

uint8_t strcomp(const uint8_t *key, const uint8_t *s2);

uint8_t strcopy(const uint8_t *key, uint8_t *dest);

typedef struct meta_obj
{
  uint8_t key[32];
  uint8_t* address;
  uint64_t size;
  uint8_t allocated;
  uint8_t timer;
} meta_obj;

typedef meta_obj* meta_t;

void memcopy(const void *source, void *dest, uint64_t size);
