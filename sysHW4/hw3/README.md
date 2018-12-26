# hw3

Our source and unit testing (unit.c) can be compiled with the included makefile. The test will run several cases (insertion, insertion->deletion->insertion, insertion->replacement, double deletion), comparing the return of cache_get to the original data each time and printing an error if they do not match.
The test will then pause (with scanf-- have to enter a string to continue) and run a series of Sets on 512mb data blocks to test large value sizes and eviction. Each set is proceeded by a description of the state of the hash table and the allocation status of the cache.


(last minute edit) there appears to still be a problem with eviction-- the evicter does not think any values are the right size range to be evicted. I've identified the line that breaks (cache.c, line 158), but not why.
--------------------------------------------------------------

Our initial implementation of cache.c used linear search, no hash, and delete marked values unreadable rather than freeing memory. The cache was cleaned whenever it became full by copying all readable values to a new cache, thus defragmenting and freeing deleted values.

The second implementation used a bucket (linked list) hashtable with the hash function of multiplying the bytes of the key mod number of buckets. The keys were stored in meta structs containing key, value size, LRU time, and pointer to value.

We solved hash collisions by using separate chaining with linked lists. We like this solution because it is fast and easy to implement, though as wikipedia notes, a linked list implementation has poor cache performance, so that is a tradeoff we make. 

We found it difficult to implement eviction and defragmentation with this setup. Values were disorganized, so the only way of finding a chunk of available memory was to scan all of the meta structs.
We decided to implement memory slabs with a dynamic allocator, since we felt that this was an elegant solution to the problem of fragmentation. Similarly to the Scaling Memcache at Facebook paper, our cache stores values in 1MB slabs of various classes, starting at 64 bytes and advancing by powers of 2 to 1MB. Values are stored in the smallest slab class that they can be stored in. When a class is out of memory and needs to store a new value, the slab manager allocates a new slab of that class if there are unallocated slabs. If there are no unallocated slabs, we evict a member of the desired class.

Eviction proceeds by LRU, which is implemented by a timer integer kept on each metadata structure and updated with each cache set and get. Because slabs are all 1MB, the value's address and size are sufficient to find the parent slab and de-allocate the evicted value. This allowed us to minimize the meta data associated with each value.
