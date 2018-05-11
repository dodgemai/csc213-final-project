CFLAGS=-Wall -std=gnu99 -pg -g # -fsanitize=address -gp
CXXFLAGS=-Wall -g -fsanitize=address -isystem $(ROOT)/deps/gtest/include/
CC=gcc
CXX=clang++
GTEST_CFLAGS = `pkg-config --cflags gtest_main`
GTEST_LIBS = `pkg-config --libs gtest_main`
GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/local/include
ROOT=.

.PHONY: all tidy clean

all: server mcache_test mcache_iptest

server: server.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c key_list.c key_list.h
	${CC} ${CFLAGS} -o server server.c socket_list.c hashmap.c entry_list.c mcache.c key_list.c -lpthread

server_test: server_test.c server.h mcache_types.h server
	${CC} ${CFLAGS} -o server_test server_test.c -lpthread

hashmap_test: hashmap_test.c hashmap.h hashmap.c entry_list.h entry_list.c mcache_types.h
	${CC} ${CFLAGS} -o hashmap_test hashmap_test.c hashmap.c entry_list.c -lpthread

mcache_test: mcache_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o mcache_test mcache_test.c mcache.c -lpthread
client_old: client_basic.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o client_old client_basic.c mcache.c -lpthread

mcache_iptest: mcache_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache_inclient.c key_list.c key_list.h
	${CC} ${CFLAGS} -o mcache_iptest mcache_test.c mcache_inclient.c hashmap.c entry_list.c key_list.c -lpthread

mcache_test_fancy: mcache_test_parallel.cc server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CXX} ${CXXFLAGS} -MMD -MP -c mcache_test_parallel.cc socket_list.c hashmap.c entry_list.c mcache.c $(GTEST_LIBS)
	${CXX} ${CXXFLAGS} -MMD -MP -o mcache_test_fancy.o $(GTEST_CFLAGS)
tidy:
	rm -f *~

clean: tidy
	rm -f server
	rm -f mcache_test
	rm -f mcache_iptest
	rm -f client_old
	rm -f hashmap_test
	rm -f server_test
	rm -rf ./*.dSYM
	rm -rf ./*.o
	rm -rf ./*.d

# Targets to fetch and build external dependencies
# Get and build libelfin
$(ROOT)/deps/gtest:
	@mkdir -p $(ROOT)/deps
	@cd $(ROOT)/deps; \
		wget https://github.com/google/googletest/archive/release-1.7.0.tar.gz; \
		tar xzf release-1.7.0.tar.gz; \
		rm release-1.7.0.tar.gz; \
		mv googletest-release-1.7.0 gtest

# Update build settings to include the gtest framework
ifneq (,$(findstring gtest,$(PREREQS)))
CXXFLAGS += -isystem $(ROOT)/deps/gtest/include/
LDFLAGS += -isystem $(ROOT)/deps/gtest/ \
					 -isystem $(ROOT)/deps/gtest/include/ \
					 $(ROOT)/deps/gtest/src/gtest-all.cc \
					 $(ROOT)/deps/gtest/src/gtest_main.cc
endif
