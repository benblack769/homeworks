#include <stdlib.h>
#include <inttypes.h>


//Returns 0 for "false" and 1 for "true"
uint8_t strcomp(const uint8_t *str1, const uint8_t *keyspace)
{
  int i=0;
  while (i < 32)
  {
    if (str1[i] == keyspace[i]) i++;
    else return 0;
    if (str1[i] == '\0' && keyspace[i] == '\0') return 1;
  }
  return 1;
}

void strcopy(const uint8_t *src, uint8_t *dest)
{
  int i=0;
  while (src[i] != '\0')
    {
      dest[i] = src[i];
      i++;
    }
  dest[i] = src[i];
}


//Copies <size> bytes from source into dest
void memcopy(const void *source, void *dest, uint64_t size)
{
  int i;
  uint8_t *copy = dest;
  const uint8_t *values = source;
  for (i=0; i < size; i++) 
  {
    copy[i] = values[i];
  }
}
