#include <stdlib.h>
#include <stdio.h>
#include "cache.h"
#include "cachewrapper.h"

void basic_test();

int main(){
    basic_test();
    return 0;
}
void basic_test(){
    cache_t cache = create_cache_wrapper(1000,NULL);
    uint64_t val = 1231233;
    key_type key = "hi there";
    cache_set(cache,key,&val,8);
    uint32_t num = 0;
    if(*(uint64_t*)(cache_get(cache,key,&num)) != val || num != 8){
        printf("cache not working!");
    }
}
