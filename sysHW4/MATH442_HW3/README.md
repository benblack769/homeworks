# MATH442_HW3
##NAME: Ziyuan Zhong
##File Hierarchy
```
cache.h - API of the look-aside cache
cache.c - look-aside cache 
LRU.c - LRU implemented using double-linked list
hash_table.c - implementation of cache using hash_table
test.c - tests made on the cache
Structure_picture.JPG - a picture drawn by hands to show the basic structure of my cache

Environment:
System: OS X EI Capitan 10.11.3
Compiler: clang-700.1.81

Command:
gcc -O3 cache.c test.c LRU.c hash_table.c -o test
./test
```
##Part0: Structure of my cache in the end
![alt tag](https://cloud.githubusercontent.com/assets/12670254/13593047/b681798a-e4ac-11e5-8f82-b3fa1402ab78.JPG)


##Part1: Basic cache operations
Note: I didn't keep the original part1 version. I will describe the way I once did in part1.

Implementation: I use a double linked list to manage the items.
Each time when user adds a new item, the new created item is put to the front of the list. 
Each time when user deletes an item, it's removed from the list.
Eviction Policy: I evict the last element in the linked list when the current_size == max_size


##Part2: Testing
I made the a series of tests on my cache in test.c. While there are lots of repetitions/overlap in these tests, I got flexibility to change and play with each of them. I trade repetition for flexibility.


##Part3: Performance
I implement a hash-table(separate-chain).
The average time of each iteration is O(1) but in the worst case, some operations(get,set,delete) take O(N). The hash function I created is just a simple(and stupid) one which return the double of the value of input key(This leads to the fact that half of the buckets are forever empty). I did this intentionaly for the convenience of testing hash collision.


##Part4: Collision resolution
I used separate chaining with unordered linked lists to deal with collision.
Reasons: I choose this method mainly because of the fact that the other common method-open addressing relies heavily on hash function, which is not easy to create a good one without large overhead.
Besides, Separate chaining with other data-structures(e.g.self-balancing tree) are not easy to implement and they usually have large memory costs.
At the same time, separate chaining is easy to implement without tricky bugs.
Pros:
(1)easy to implement.
(2)can use a simple hash function.
(3)relatively small memory costs.
(4)Performs well(when the load factor is relatively low). This is aided in the next part when we resize the hash table as the load factor goes beyond a certain threshold.
Cons:
(1)if the key distribution is far from being uniform(i.e. when most keys are in the same few number of chains), the time the basic operations take will be O(N), which is not quite different from that of a linked list.
(2)large overhead to traverse a chain when there are too many items on one chain.


##Part5: Dynamic resizing
I used resizing by copying all entries.
Reasons: I suppose that this cache will be used offline(not used for server) and handle a relatively moderate number of data.
Pros: 
(1)Easy to implement.
(2)The cost of resizing is still O(1).
Cons: 
(1)Each resizing has relatively high overhead. 


##Part6: Eviction policy
I abstract away my eviction policy in LRU.c using a double linked-list consists of pointers to my items which have keys and values. This design gives me lots of space to change my policy in the future.

By default, each time when the cache needs to evict one item to make space for the new item, it calls the its eviction function and evicts the item which has the key returned by the function. 
The API for the eviction policy is:
```
// For a full cache, return the key of an element to evict 
typedef key_type (*evict_func)(struct cache_obj * cache);
```
The function receives a full cache as its input and output a key_type which is the pointer to a key in cache to be evicted.
Currently, it just returns the key of the last element in the linked list which turns out to be the oldest because each time when a new item is created or an old item is retrived, the corresponding node is put to the front of the linked list(if it is not currently the front one).

It's very easy modify eviction policy:
e.g.If we want to change LRU to FIFO, we can let linked_list_top(LRU.c) do nothing by commenting out its function body. Or we can comment out the usage of it in cache_get(cache.c).


