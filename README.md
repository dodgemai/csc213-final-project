# csc213-final-project

notes:  
* currently upper limit on object size of 2^15 -- this should be fine, but can also be changed pretty easily if necessary
* cache size can be changed using macro in mcache_types.h (currently 100 bytes)
* eviction done passively with LRU -- when trying to place an item that would overfill cache, items are pushed out unitl there is enough space. In the case that the object is greater than the cache size, the object is simply not stored, and no items are evicted. 
* also another use-case for this would be as an intermediary between a more persistent storage option, such as disk or some sort of persistent database, and having to manually manage your own memory
