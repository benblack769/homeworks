#include "cachewrapper.h"
#ifdef SYSHW3
#include "sysHW3/cache.h"
#elif defined  HIO
#include "Hash-it-Out/cache.h"
#elif defined  HIO2
#include "hash_it_out/c_code/cache.h"
#elif defined  HW3_442
#include "442-HW-3/cache.h"
#elif defined  HW3_S
#include "hw3/cache.h"
#elif defined  SOFT_C
#include "Software-Cache/cache.h"
#elif defined  MATH442
#include "MATH442_HW3/cache.h"
#endif
#include <stdio.h>
cache_t create_cache_wrapper(uint64_t maxmem, hash_func hash){
#ifdef SYSHW3
    printf("syshw3\n");
    return create_cache(maxmem,hash);
#elif defined  HIO
    printf("HIO\n");
    return create_cache(maxmem,hash);
#elif defined  HIO2
    printf("HIO2\n");
    return create_cache(maxmem);
#elif defined HW3_442
    printf("HW3_442\n");
    return create_cache(maxmem,hash);
#elif defined HW3_S
    printf("HW3_S\n");
    return create_cache(maxmem);
#elif defined SOFT_C
    printf("SOFT_C\n");
    return create_cache(maxmem,hash,NULL,NULL);
#elif defined MATH442
    printf("MATH442\n");
    return create_cache(maxmem,hash,NULL);
#endif
}
