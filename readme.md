File!!!

### Success and status


Serious memory leaks.

### Testing

Disclaimer: I have never really learned how to test, and I sort of ran out of time while writing tests, so these are kind of awful tests.

The final test, cache_speed_test is by far the most thorough and useful, despite not checking if the gotten values are the correct ones, because it sets, deletes, and gets keys in a random way, which means that every combination of set, delete and get is tested, including multiple sets and deletes of the same key. This exposed all sorts of errors.

It also was a great way to check for memory leaks, as even minor memory leaks are fairly serious when running something millions of times.

### Eviction policy interface

At first, I considered an interface which was completely abstracted from the cache, and held no information about the cache. While this would be very generalizable, I did not like the fact that I would have to create a duplicate hash table, so I instead made the interface in "replacement.h".

This interface has an abstract data that the user inputs (user_id_t), that is associated with the policy information. The idea is that when the policy decides to delete a value, it returns markers that the cache understands, so that it can then delete the values. In my hash table cache, I used pointers to the keys I was storing in my cache.

It was abstract enough that I could directly test it without dealing with any cache at all (using markers of integers, rather than string pointers).
