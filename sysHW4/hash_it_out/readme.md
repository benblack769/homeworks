# Hash it out
@aled1027, @ifjorissen

Hash-it-out is a generic look-aside cache.
A look-aside cache is a key-value storage for items that are difficult or slow to compute.

## TODO
- None currently

-------

###Hash it out goal: Implement a bare-bones look-aside cache. Features: dynamic resizing, collision handling, customizable hash function, LRU eviction policy.

###File overview
```
  readme.md          : what you're reading right now
  c_code/node.h      : header file for the node type, which has been customized for this
                       assignment to store a key and a value
  c_code/node.c      : implementation of the node type used in the linked list
  c_code/dbLL.h      : header file for the doubly linked list of nodes, depending on
                       node.h
  c_code/dbLL.c      : implementation of the doubly linked list
  c_code/cache.h     : header file for cache; cache API 
  c_code/cache.c     : implementation of cache
  c_code/dbLL_tests.c: tests for doubly linked list
  c_code/evict.h     : header file for eviction policy; eviction api
  c_code/evict.c     : implementation of eviction policy
  c_code/main.c      : tests for the cache
  c_code/makefile    : a simple makefile
```

### To run:
  * `make`: creates object files
  * `make run_all`: runs both the cache and linked list tests
  * `make run`: runs the cache tests
  * `make clean`: removes object files

------

### General Notes:
  Our `cache_obj` stores the number of buckets in the table (`num_buckets`), the memory used (`memused`), the maximum amount of memory availiable to the table (`maxmem`), the number of elements in the table (num_elements), a pointer to a hash function (`hash`), a pointer to the linked lists at each bucket (`hash_buckets`), and an evict object (`evict`). 

  Our `node_t` is designed specifically for use in a doubly linked list as well as for use in a hash table. Instead of having just one data item, it has two, a key and a value. 

  Our doubly linked list, `dbLL_t`, stores a pointer to the head and tail nodes in its list, and the size of the list.

### On Collision Resolution
  We decided to use a doubly-linked list to handle collision detection. The idea of resolving collisions using some form of chaining is not new-- it is a common way to handle collisions in hash tables. Another reasonable choice (given scope of this assignment) might have been open-addressing. 

  So in our code, the hash bucket is a pointer to a (unordered) doubly linked list.

  We chose to use the doubly-linked list for the following reasons: 

  - It handles deletion more gracefully than a singly linked list.
  - Collisions become something of a "non-issue," since we only have to insert the key into the list that each bucket in the cache has a pointer to.
  - Deletion in a hash table with chaining has fewer things to keep track of than deletion might in a hash table which leverages open addressing. This is also true of search.
  - Unlike open addressing, where the number of elements in the table must always be less than the number of buckets, it's _possible_ to store more elements in the table than there are number of buckets. Furthermore, as the load factor approaches 1, a table designed with open addressing will see much faster performance degredation than a table designed with chaining as its collision resolution mechanism. Of course, this comes at the expense of (near) constant time accesses, which is the whole point of a hash table.

  Of course, in choosing to use a linked list for our collision resolution mechanism, we incur the overhead of the structure itself, that is to say, the pointers to next and prev nodes, as well as the cost of traversal.


### On eviction

We implemented LRU as our eviction policy, and attempted to do so in a manner that was modular and abstracted, such that the eviction policy is easily changed. 
In pursuit of this goal, we created a simple, generic eviction api with methods like `get, set, delete, destroy` (see `evict.h` for the full API).
In the actual implementation of LRU, we use a queue data-structure implemented via an array with stored indices of the front and end of the queue.
Most eviction operations (except `create`, `destroy` and `get`) run in constant time, but they are implemented naiively, so the performance may not be optimal. 
`get` does not run in constant time (it is rougly linear in the number of keys stored) because the queue data-structure must be looped over to find the key.
If it turns out that that `get` is called often and `delete` and `select_for_removal` are not, then the linear computation currently in get could be moved to `delete` and `select_for_removal`.

### On Testing
We have three sets of tests. 
`cache_tests.c` contains tests for the cache, `evict_tests` contains tests for the eviction, and `dbLL_tests.c` contains tests for the doubly linked list.
Our tests are intended to cover a variety of use-cases, but due to their finite nature, do not cover all test cases. 
In particular, most of our tests, with the major exception of the cache set/get test (a test that Alex heavily relied on), utilize fewer than 10 key-value pairs. 
Where in practice, we imagine that much more than 10 key-value pairs are being set, accessed, and deleted.

-------

### Known Issues
None currently


#####References
  * [Wiki on Open Addressing](https://en.wikipedia.org/wiki/Open_addressing)
  * [Wiki on Hash Tables & Collision Resolution](https://en.wikipedia.org/wiki/Hash_table#Collision_resolution)
