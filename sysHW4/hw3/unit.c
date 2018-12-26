#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "cache.h"

//Check if two arrays are equal
int test(void *a1, void *a2, int len)
{
  int i;
  uint8_t *arr1 = a1;
  uint8_t *arr2 = a2;
  if (a1 == NULL && a2 == NULL) return 1;
  if ((a1 == NULL && a2 != NULL) || (a2 == NULL && a1 != NULL)) 
    {
     printf("Null error\n");
     return 0;
    }
  for (i=0; i < len; i++)
    {
      if (arr1[i] != arr2[i]) 
      printf("Unequal arrays at index %d\n",i);
      return 0;
    }
  return 1;
}

void usr_input()
{
  static cache_t cache;
  scanf("%*s");
  char cmd[32],arg1[32],arg2[32],arg3[32];
  scanf("%[^(](%[^,)],%[^,)],%[^,)])",cmd,arg1,arg2,arg3);
  printf("%s   %s   %s   %s\n",cmd,arg1,arg2,arg3);

  if (strcomp(cmd,"create_cache"))
    {
      cache = create_cache(atoi(arg1));
    }
  else if (strcomp(cmd,"cache_set"))
    {
      cache_set(cache,arg1,arg2,atoi(arg3));
    }
  else if (strcomp(cmd,"cache_delete"))
    {
      cache_delete(cache,arg1);
    }
  else if (strcomp(cmd,"cache_get"))
    {
      val_t r = cache_get(cache,arg1,(uint32_t*)arg3);
      printf("%s -- size %u\n",(char*)r,*arg3);
    }
}

int main()
{

  const uint64_t mem = 1 << 21;
  cache_t cache = create_cache(mem);
  uint32_t data[8] = {0,1,2,3,4,5,6,7};
  printf("Adding data to cache.\n");
  cache_set(cache, "test", data, 32);
  printf("Added data to cache.\n");
  uint32_t *size = malloc(4);

  //Test ability to save/return one value
  printf("\nTesting ability to return value\n");
  void *ret = (void *)cache_get(cache,"test",size);
  test(data,ret,32);

  //Test ability to delete values
  printf("\nTesting ability to delete values\n");
  cache_delete(cache,"test");

  printf("Deleted, testing ability to retrieve again\n");
  ret = (void *)cache_get(cache,"test",size);
  test(NULL,ret,16);

  //Test ability to overwrite value
  printf("\nTesting ability to overwrite value\n");
  cache_set(cache,"test", data, 16);
  ret = (void *)cache_get(cache,"test",size);
  test(data,ret,16);
  
  //Test ability to save two values
  printf("\nTesting ability to store two values\n");
  cache_set(cache,"test2",data,32);
  ret = (void *)cache_get(cache,"test2",size);
  test(data,ret,32);
  print_cache(cache);
  //Test the test function
  printf("\nThis should fail:\n");
  uint32_t fake[] = {54,37,21};
  test(ret,fake,32);


  //Test ability to add to a deleted value
  printf("\nTesting ability to add to a deleted value\n");
  cache_set(cache,"test",data,32);
  ret = (void *)cache_get(cache,"test",size);
  test(data,ret,32);

  uint8_t *big_data = malloc(1<<14);
  cache_set(cache,"hallo",big_data,1<<11);
  cache_set(cache,"hallo1",big_data,1<<12);

  destroy_cache(cache);  
  free(big_data);


  //Eviction testing

  printf("\n\nEviction Testing: (enter a key to proceed) \n");
  scanf("%*s");

  uint8_t *bigger_data = malloc(1<<19);
  cache_t test_cache = create_cache(mem);
  print_cache(test_cache);
  cache_set(test_cache,"yes",bigger_data,1<<19);
  print_buckets(test_cache);
  print_cache(test_cache);
  cache_set(test_cache,"yes1",bigger_data,1<<19);
  print_buckets(test_cache);
  print_cache(test_cache);
  cache_set(test_cache,"yes2",bigger_data,1<<19);
  print_buckets(test_cache);
  print_cache(test_cache);
  cache_set(test_cache,"yes3",bigger_data,1<<19);
  print_buckets(test_cache);
  print_cache(test_cache);
  cache_set(test_cache,"no",bigger_data,1<<19);
  print_buckets(test_cache);
  print_cache(test_cache);
  cache_set(test_cache,"no2",bigger_data,1<<19);
  print_buckets(test_cache);
  print_cache(test_cache);
  destroy_cache(test_cache);
  free(bigger_data);
  
}
