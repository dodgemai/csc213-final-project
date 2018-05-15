# csc213-final-project
# mcache! -- it's like memcached but worse!

## Build instructions
* `make all` makes all relevant executables
* `make robust` makes a 'robust' test suite
* `make basic` makes a 'basic' test suite  
Note: executables with "ip" in the name are "in-process" executables, and can run without a server. Executables without the ip (aside from the server executable itself) require a running server. 

## Testing / data-gathering instructions
* `gather_testdata.sh`
  * can edit the file to specify the number of data points to collect, and over what range of values
* `server`
  * Run the server
  * For any non-ip executables, you must first run the server. 
* `mcache_test <n>`, `mcache_iptest <n>`
  * test mcache over n range of values
     * note: currently mcache_test includes a penalty for cache_miss, this can be taken out if the sole purpose is to test correctness and not to gather data
* `mcache_test_robust`, `mcache_iptest_robust`
  * test mcache functionality with ints, structs, and arrays
  
## mcache interface: 
* see mcache.h
* NOTE: mcache_gets is currently unimplemented

## What do all of these files do / why are they here???
* deps/gtest, test
  * for future use with google testing suite, currently pointless
* test_data
  * this is where data is put when you run gather_testdata.sh
* Makefile
  * you should know this one
* client_basic.c 
  * outdated testing file
* entry_list.c, entry_list.h 
  * To help with the internals of hashmap -- chaining was used, and these are just for a linked-list (ordered)
* fibonacci_test.c 
  * Attempt to gather data, but fibonacci sequence is trivialized as long as the cache can hold 3 values (current, and i - 1, and i - 2)
* gather_testdata.sh
  * script to gather data -- data is put into test_data folder
    * .ip* is for in-process implementation data, and non-ip are for server implementation
  * can comment / uncomment lines as necessary depending on the data you are trying to collect
* hashmap.c, hashmap.h
  * your basic hashmap -- fixed-size, thread-safe, chaining. For use with efficiently storing/retrieving data from cache.
* hashmap_test.c 
  * very barebones testing for correctness with hashmap -- outdated
* key_list.c, key_list.h
  * doubly-linked queue, thread-safe (poorly done, but done nonetheless) -- to deal with eviction
* mcache.h, mcache.c, mcache_inclient.c
  * mcache.h contains the interface to be used by the client
  * mcache.c is the implementation to talk to the server
  * mcache_inclient.c is the implementation to work within the client process
    * this essentially contains much of the same code as from server.c, as in this case the client is acting as it's own caching mechanism, so everything is internal
    * SMALL_FACTOR macro at the start determines the factor that this cache will be smaller than the network cache
* mcache_types.h
  * contains useful macros to change things like cache_size, and has other things that are necessary for mcache implementation
* mcache_test_parallel.c 
  * more robust testing suite for mcache -- attempt was made at testing parallelism but this did not work as we would have hoped, due to the nature of the server. See comments within file for more details. 
* mcache_test_parallel.cc 
  * for future google-test suite usage (currently partially unfinished)
* server.c 
  * mcache server, does what you would want it to do
* server.h 
  * just contains macro for the port that the server will listen on
* server_test.c 
  * basic server testing
* socket_list.c, socket_list.h
  * for managing child socket list within server 
  
## Notes
* currently upper limit on object size of 2^15 -- this should be fine, but can also be changed pretty easily if necessary
* cache size can be changed using macro in mcache_types.h (currently 300 bytes)
* eviction done passively with LRU -- when trying to place an item that would overfill cache, items are pushed out unitl there is enough space. In the case that the object is greater than the cache size, the object is simply not stored, and no items are evicted. 
* currently server-implementation runs *very* slowly on mathlan machines. Not sure why, it didn't before, and it runs perfectly fine on macbook pro using macOS Sierra 10.12.6 w/ 2.7 GHz Intel Core i5 and 8 GB RAM
