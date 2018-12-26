### Testing Plan

It looks like all the api breaks are in the create_cache function. It shouldn't be too hard to just create a wrapper for each person's create_cache with ifdefs. You can set definitions when compiling, so the compile script will just make 7 different executables with different defines, and then run them all and compare output.


### Test ideas

* Test tiny strings with a single null terminated charachter followed by nonsense and see if it is consistent.
* Test large strings with identical first bytes and see if they map to the same bucket


### Changes to cache source code to get compiles to work

* MATH442_HW3: changed 32 bit cache_space_used to 64 bit
* Put in NULL for additional function arguments for Software-Cache and MATH442_HW3. I am pretty sure that this means use the default (LRU) eviction functions in both bits of code.
* Changed key_t to key_type in one of the projects.
* Included stdint.h in Hash-it-Out/lru.h, Software-Cache/lru.h
* Changed function name from undefined function to "make_item_array" in sysHW3/lru_replacement.h (line 137)
* Changed type of hash_default and hash_func to 64 bit integer functions instead of 32 bit in MATH442_HW3/cache.c, cache.h
