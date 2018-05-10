ROOT = .
DIRS = .
TARGETS = mcache_test_fancy
PREREQS = $(ROOT)/deps/gtest ../mcache_test_parallel.cc ../hashmap.c ../key_list.c ../entry_list.c ../mcache_inclient.c ../socket_list.c

include $(ROOT)/common.mk

test:: mcache_test_fancy
	./mcache_test_fancy
