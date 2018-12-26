#pragma once

struct manager_obj;
typedef struct manager_obj* manager_t;

void *get_address(manager_t man, uint32_t val_size);

void man_delete(manager_t man, uint8_t *val_ptr);

uint32_t get_slab_class(manager_t man, uint32_t size);

manager_t initialize(uint32_t size);

void print_slabs(manager_t man);
