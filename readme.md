File|contents
--|--
cache.h | Assigned header file
replacement.h | Eviction policy API
hash_cache.c | Final cache implementation
lru_replacement.c | LRU code using the replacement.h interface
test.c | The test code with the main function
basic_cache.c | A kind of shitty and mostly non-functional cache implementation (currently commeted out)
link.h | Helper header for basic_cache.c (not included by main project)


## Design


### Hash table

Hash table uses chaining to resolve collisions. I seriously considered using open addressing with linear probing for its simplicity and speed, but I noticed that with a load factor of 0.5, I would be wasting huge amounts of memory, as each item in the table would be 32 bytes, and at worst, I would have 4 of these per actual item, which is 128 bytes of overhead, which is ridiculous. Deletes would also be complicated to do quickly, so I threw this idea out. Chaining does not have the memory overhead problem because  my table only holds pointers (so 32 bytes of overhead, which is acceptable).

Maintains a load factor of 0.5 as values are added, which seems a good balance between memory usage and hash speed as the memory overhead is still relatively small, and assuming a good hash function, there shouldn't be many collisions (only a few percent) at 0.5 load.

### Eviction policy interface

At first, I considered an interface which was completely abstracted from the cache, and held no information about the cache. While this would be very generalizable, I did not like the fact that I would have to create a duplicate hash table, so I instead made the interface in "replacement.h".

This interface has an abstract data that the user inputs (user_id_t), that is associated with the policy information. The idea is that when the policy decides to delete a value, it returns markers that the cache understands, so that it can then delete the values. In my hash table cache, I used pointers to the keys I was storing in my cache.

On the cache side, the cache keeps track of a bit of abstract data that the policy returns, and interacts with the policy using this bit of data. In the LRU, this is a pointer to a link in the bidirectional linked list that forms the LRU.

If I were to make FILO, I would have a simple queue, and p_info_t would be a pointer to the location of the queue of the item.

I were to make Evict Largest, I would have p_info_t be a pointer to the location in the heap I use to keep track of the largest.

It was abstract enough that I could directly test LRU without dealing with any cache at all (using markers of integers, rather than string pointers).

## Success and status

#### Serious memory leaks

After running the tests and all functions return, and all values are deleted, there is still over 100MB of memory taken up on my system. I have not been able to track this down.

#### Constant time performance

Time for 1,000,000 iters on sets of around 50,000 keys is around 0.27 seconds
Time for 10,000,000 iters on sets of around 500,000 keys is around 3.4 seconds

This appears to be scaling liniarly with number of iterations, which means that each key access is probably constant time.

Cycles per op = (3s * 3ghertz) / 10,000,000iters = 900 cycles/iter which reasonable to assume for constant time, even if it is kind of slow.

#### Testing

Disclaimer: I have never really learned how to test, and I sort of ran out of time while writing tests, so these are kind of awful tests.

The final test, cache_speed_test is by far the most thorough and useful, despite not checking if the gotten values are the correct ones, because it sets, deletes, and gets keys in a random way, which means that every combination of set, delete and get is tested, including multiple sets and deletes of the same key. This exposed all sorts of errors. Including a bug in my hash function which killed performance.

It also was a great way to check for memory leaks. After all, even minor memory leaks are fairly serious when running something millions of times.

However, it is not such a great way to test for correctness of LRU and and the hash table, and I feel now that the tests I wrote to test for correctness are quite inadequate.
