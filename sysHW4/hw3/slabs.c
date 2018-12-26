#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tools.h"
#include "slabs.h"


typedef struct slab{
  uint8_t memory[1<<20]; //1 MB slabs
  uint32_t class;
  uint32_t status[512]; //16384 bits of allocation status
  uint8_t isFull;
} slab;


struct manager_obj{
  uint8_t full;
  uint32_t* classes;
  struct slab* slabs;
  uint32_t num_slabs;
  uint32_t* directory;
};


void bit_string(uint32_t bits)
{
  int i, j;
  for (i=31; i >= 0; i--)
    {
      j = (bits & (1<<i)) != 0;
      printf("%d",j);
    }
}

void toggle_bit(uint32_t *status, uint32_t bit)
{
  uint32_t *chunk = status + (bit / 32);
  uint32_t index = bit % 32;
  chunk[0] ^= (0x80000000 >> index);
  printf("done\n");
}


//Sets an entry's allocation status to 0 by xoring it
void slab_delete(struct slab *s, uint32_t item)
{
  toggle_bit(s->status, item);
  s->isFull = 0;
  bit_string(s->status[0]);
}

void slab_destroy(manager_t man)
{
  free(man->slabs);
  free(man->directory);
  free(man->classes);
  free(man);
}

//Returns which slab class a value of size <size> would fit in most tightly
uint32_t get_slab_class(manager_t man, uint32_t size)
{
  uint32_t *classes = man->classes;
  int i=0;
  while (classes[i] != 0) 
  {
    if (classes[i] >= size) return classes[i];
    i++;
  }
  printf("Error: Value %"PRIu32" did not fit in any slab.\n",size);
  exit(0);
}

//Returns 0 if there are no free indices; or (index+1) of the first free index.
uint32_t get_free_index(struct slab *s)
{
  uint32_t i,j;
  for (i=0; i < 512; i++)
  {
    if (s->status[i] == ~0) 
      {
	continue; //status is full of 1's
      }
    else if (s->status[i] == 0) 
    {
      return 32*i + 1; //status is full of 0's
    }
    if (s->status[i] == ~0) return 0;
    j =  __builtin_clz(~s->status[i]); //get #leading 1's
    return 32*i+j + 1;
  } 
  return 0;
}

struct slab* allocate_slab(manager_t man, uint32_t index, uint32_t slab_class)
{
  struct slab* s = &man->slabs[index];
  man->directory[index] = slab_class;
  s->class = slab_class;
  int i;
  //Set the relevant allocation bits to 0	  
  for (i=0; i < (1<<20) / slab_class / 32; i++)
  {
     s->status[i] = 0;
  }
  int remainder = (1<<20) - i*32*slab_class; //the remaining space
  remainder /= slab_class;  //The number of entries left (individual allocation bits)
  if (remainder > 31) {
    printf("Something happened in the slab allocation calculation\n");
    exit(-1);
  }
  int j;
  for (j=remainder; j < 32; j++) s->status[i] |= 1 << (31-j);
  //Set the irrelevant allocations bits 1; they can never be changed
  for (i++; i < 512; i++)
  {
    s->status[i] = ~0;
  }
  s->isFull = 0;
  return s;
}


//Returns a slab with free entries or NULL if there is none
struct slab *get_slab(manager_t man, uint32_t slab_class)
{
 int i=0;
  for (i; i < man->num_slabs; i++)
    {
      if (man->directory[i] == slab_class && !man->slabs[i].isFull)
      {
	return man->slabs + i;
      }
      if (man->directory[i] == 0)
      {
        return allocate_slab(man,i,slab_class);
      }    
  }
  man->full = 1;
  return NULL;
}

//Deletes a value from its slab
void man_delete(manager_t man, uint8_t *val_ptr)
{
  uint8_t *start = (uint8_t*)man->slabs;
  int i = 0;
  //Loop over slabs until we pass our value address. This gives the slab index
  while (start < val_ptr) 
    {
      start += sizeof(struct slab);
      i++;
    }
  struct slab *to_adjust = man->slabs + i;

  //Find the exact index of the value
  i = (val_ptr - to_adjust->memory); //This is how many bytes are between 0th entry and val
  i /= to_adjust->class;                      //This is the index of value
  slab_delete(to_adjust, i);
}

//Returns an available address for val_size, and sets that address to Allocated.
void *get_address(manager_t man, uint32_t val_size)
{
  uint32_t slab_class = get_slab_class(man, val_size);
  struct slab* s = get_slab(man, slab_class); //Returns a slab with free entries or NULL
  if (s == NULL) 
    {
      return NULL;
    }
  uint32_t i = get_free_index(s);
  if (i == 0) {
    s->isFull = 1;
    return get_address(man, val_size);
  }
  toggle_bit(s->status, i-1);
  return s->memory + s->class * (i-1);
}



//initializes a slab manager and its slabs. The number of classes and how much each class holds is currently hardcoded, but given our implementation it is easy to change.
manager_t initialize(uint32_t size){
    manager_t ret = malloc(sizeof(struct manager_obj));
    ret->full = 0;
    ret->classes = malloc(16 * 32);

    //makes slab classes from 64 bytes to 1MB up by powers of 2 each time    
    int i = 6;
    for(i; i<=20; i++){
      ret->classes[i-6] = 1 << i;
    }
    ret->classes[15] = 0;

    ret->num_slabs = size/(sizeof(struct slab));

    //allocates the array is the slabs, holds the values
    ret->slabs = malloc(sizeof(struct slab) * ret->num_slabs);

    //allocates directory, which is an array of ints that keeps track of which slabs are assigned to which class.
    ret->directory = calloc(ret->num_slabs, 32);
    return ret;
}


void print_slabs(manager_t man)
{
  printf("Man has %u slabs:\n",man->num_slabs);
  int i=0; int j=0;
  for (i; i < man->num_slabs; i++)
    {
      printf("Class %d, status ",man->directory[i]);
      bit_string(man->slabs[i].status[0]);
      printf(" ... ");
      bit_string(man->slabs[i].status[511]);
      printf("\n");
    }
}
