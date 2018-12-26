# HW 3: Hash it out! (Jeremy Cosel & Nathan Duncan)

##File Hierarchy

```
cache.h: The given Cache API
cache.c: Our implementation of the Cache API
main.c: Our tests for the code
test.out: The output of Main

To compile: gcc -o Test cache.c main.c -lm
```

##Part 1: Basic Cache Operations

This naive implementation (started by Eitan, completed by Jeremy) is pretty simple, but builds a solid foundation for the future code to be written. We create two structures: a cache_obj, as requested by the API, that stores the capacity, the number of items as an integer, and an array of items. Each element of the array is a custom type named item, with its own key and value, also defined in the API. It also has a value size associated with it, since values can be a variable size.

The code is pretty self-explanatory and can be seen in the "naive" branch. As items are added, they're put randomly in the array. If it sees a duplicate, it will just overwrite the previous item with the new item's data. If the array fills up, it just won't add anything at all. I leave the rest to the reader to deciper as this isn't the "main attraction" of the assignment.

(Sidenote: For the sake of time and the implementation chosen, maxmem isn't really used at this point.)


##Part 2: Testing

Our tests include the following:

```
test\_create\_cache(): Runs create\_cache and asserts that the assigned variable to the function points to something, then prints out the cache (which should print no items at this point).
test\_cache\_set(): Runs cache\_set with 16 different items on a 10-element array; this tests generic insertion and insertion on a full cache. A print before and after three insertions of already existed pointers within the cache shows that already existing items are replaced with the newer pairing.
test\_cache\_get(): Tries to retrieve 7 different items, some of which should be in the cache, and some of which aren't added due to it not adding elements after it fills. It will print its value (assumed to be a char*) if found, and the pointer value (nil) if not.
test\_cache\_delete(): Tries to retrieve a particular item, before and after a call to cache_delete. Returns the virtual memory address before, and (nil) after.
test\_cache\_space\_used(): Prints the integer value of the cache space used after its function call.
test\_destroy\_cache(): Runs destroy\_cache.
```

##Part 3: Performance

We selected the "dbj2" hash function as our default function, because it had relatively good performance in terms of speed and amount of collisions. We considered using cyptographic hash functions, but they were far more difficult to work with, and were slower. Everything was easily constant time at this point.

We tested that we were able to take hash functions as inputs. At this point, we couldn't test that the actual imported hash function does its job, but our subsequent tests (collision cases) do this job satisfactorily.

##Part 4: Collision Resolution

We originally wanted to use cuckoo hashing, but working with multiple hash functions was difficult with the way the API was structured. We compromised on open addressing because it was the best solution for one hash function. We implemented open addressing with linear probing because its performance was the best, and if we were more worried about collisions, we would have switched the hash functions to SHA1.

We also changed the implementation from arrays to singly-linked lists at this part, in anticipation of dynamic resizing. We tested the collision resolution with the test\_hash\_collision function, which essentially inserts every item into the same hash "node", 1. Our open addressing was successful!

##Part 5: Dynamic Resizing

To handle dynamic resizing, we recognize the primary issue was extending the number of entries in our hash table. This was dictated by our hash function. We instead carry around a table size and mod our hash function by it. Resizing is done by doubling the table size and reallocating new memory (which we zero). Instead of reinserting all the values right away with a new hash, we instead opted to take the performance overhead over time. This happens because we have to look in the location that all the previous items that could possibly exist before the resizing.

##Part 6: Eviction Policy

Our implementation of LRU relied on extending our definition of item\_t to include a newer and older item pointer. Our cache definition now contains a LRU and MRU pointer, for the least and most recently used items, respectively. This made it quite easy to hide the queue necessary for LRU in our code's structure. We evict whenever there's not enough size when trying to cache\_set.
