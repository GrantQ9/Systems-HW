Naomi Boss and Grant Quattlebaum

Part 1: basic cache operations

We implemented a basic cache in our cache.cc file. Most of the functionality
of this cache was implicit in the C++ functionality of unordered_map.


Part 2: Testing
Many tests were included in the cache; their function is commented in the code itself. These tests all worked when run, and are described
in the comments.


Part 3: Performance
We implemented a simple hash function which takes the ascii value of the given key.
Since the hash function has only a key as an input, we couldn't take into consideration the size of the map 
or data values. Thus it's really difficult to take into consideration collision and the spread of data.


Part 4: Collision resolution
Collision resolution is already included in unordered_map, so we didn'tt have to implement this ourselves.

Part 5: Dynamic resizing
Dynamic resizing is already included in unordered_map, so we didn't have to implement this ourselves.


Part 6: Eviction policy
For the eviction policy, we implemented FIFO using a queue embedded in a larger struct. This allows for easier changes to the eviction
policy; if you want to change to LIFO, you only need to change the struct, and not the rest of the file. Implementing more complex
policies like Last Used First Out requires adding more inputs and functions to the struct, but since we don't have any storage of time used
in the cache at all right now, that seems reasonable, since we'd have to change the cache code anyway to implement that no matter what.

The old evictor is there too as a dummy to keep the .hh file happy, but it's not actually used for anything. 

One quirk of this implementation is that when a value is deleted, it isn't immediatly removed from the queue. Instead, when it is returned
by the evictor, it is rejected (since it no longer belongs to the cache) and the evictor is run again. This still takes constant time;
the time spent per deletion operation performed is constant, but that time is spent later when the evictor is run.
