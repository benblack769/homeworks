# Software-Cache
## Alec Kosik

###Cache
The cache is implemented as a struct with a list of key-value pairs as elements.  The cache also stores various metadata such as total capacity, total memory,
length currently being used, and the memory size.  All operations run in expected O(1) time, but, just like all hash tables that 
do not know the keys ahead of time, can run up to O(n) in the worst case.  Delete runs in O(n) because it has to free everything. But this shouldn't matter because most users probably won't need to quickly delete a cache.

###Collisions
Collisions are resolved by linear probing since probing usually runs better than linked lists of collided key-value pairs when
the load factor of the hashtable is low (below 50% is sufficient).

###LRU Queue
The LRU queue is implemented as a doubly-linked list with 2 pointers in separate but corresponding structs (corresponding to each hashtable bin).

###LRU API
The LRU implementation is abstracted out from the cache and allows the user to define another implementation as long as the user defines
relevant functions and structs including: an add and remove function, an eviciton struct to house these functions and any other data 
needed. Thus the user is required to do more than simply define and plug in a function, as they are asked to do with the hash function,
but the work to sub in another eviction policy should be minimal.

###Hash Function API
The default hash function is the well known djb2 hash.  Users can specify their own hash function and supply it as a parameter to the 
`create_cache` function just like the eviction policy functions: add and remove.

###Tests
All tests are included in the tests.c file which is linked with the rest of the files.  All test cases were passed.

###Dynamic Resizing
The cache does dynamically resize.  Unfortunately this causes a naive implementation of LRU to fail if the memory locations of the cache
are jumbled.  That is, if a new spot of memory must be allocated, the pointers in the linked list will be trashed, so one must abstract
them out into a separate struct, as mentioned above. So this issue has been resolved.

###Valgrind
Valgrind reported 0 leaks and 0 errors.

###Further Improvements
Currently, I am working on a threaded solution to parallelize cache queries.  

