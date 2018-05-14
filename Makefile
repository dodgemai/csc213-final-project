CFLAGS=-Wall -std=gnu99  # -g  # -fsanitize=address # -gp
CXXFLAGS=-Wall -g -fsanitize=address -isystem $(ROOT)/deps/gtest/include/
CC=gcc
CXX=clang++
GTEST_CFLAGS = `pkg-config --cflags gtest_main`
GTEST_LIBS = `pkg-config --libs gtest_main`
GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/local/include
ROOT=.

.PHONY: all tidy clean

all: server basic fibonacci robust

basic: mcache_test mcache_iptest
fibonacci: fibonacci_test fibonacci_iptest
robust: mcache_test_robust mcache_iptest_robust

server: server.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c key_list.c key_list.h
	${CC} ${CFLAGS} -o server server.c socket_list.c hashmap.c entry_list.c mcache.c key_list.c -lpthread

server_test: server_test.c server.h mcache_types.h server
	${CC} ${CFLAGS} -o server_test server_test.c -lpthread

hashmap_test: hashmap_test.c hashmap.h hashmap.c entry_list.h entry_list.c mcache_types.h
	${CC} ${CFLAGS} -o hashmap_test hashmap_test.c hashmap.c entry_list.c -lpthread
client_old: client_basic.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o client_old client_basic.c mcache.c -lpthread
mcache_test: mcache_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o mcache_test mcache_test.c mcache.c -lpthread
mcache_iptest: mcache_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache_inclient.c key_list.c key_list.h
	${CC} ${CFLAGS} -o mcache_iptest mcache_test.c mcache_inclient.c hashmap.c entry_list.c key_list.c -lpthread

mcache_test_robust: mcache_test_parallel.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o mcache_test_robust mcache_test_parallel.c mcache.c -lpthread
mcache_iptest_robust: mcache_test_parallel.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache_inclient.c key_list.c key_list.h
	${CC} ${CFLAGS} -o mcache_iptest_robust mcache_test_parallel.c mcache_inclient.c hashmap.c entry_list.c key_list.c -lpthread

fibonacci_iptest: fibonacci_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache_inclient.c key_list.c key_list.h
	${CC} ${CFLAGS} -o fibonacci_iptest fibonacci_test.c mcache_inclient.c hashmap.c entry_list.c key_list.c -lpthread

fibonacci_test: fibonacci_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c key_list.c key_list.h
	${CC} ${CFLAGS} -o fibonacci_test fibonacci_test.c mcache.c hashmap.c entry_list.c key_list.c -lpthread

mcache_test_fancy_deps: server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -c socket_list.c hashmap.c entry_list.c mcache.c -lpthread
mcache_test_fancy: mcache_test_parallel.cc mcache_test_fancy_deps
	${CXX} ${CXXFLAGS}  -c mcache_test_parallel.cc socket_list.o entry_list.o hashmap.o mcache.o
	${CXX} ${CXXFLAGS}  -o mcache_test_fancy mcache_test_parallel.o mcache.o
tidy:
	rm -f *~

clean: tidy
	rm -f server
	rm -f mcache_test
	rm -f mcache_iptest
	rm -f mcache_test_robust
	rm -f mcache_iptest_robust
	rm -f client_old
	rm -f hashmap_test
	rm -f server_test
	rm -f fibonacci_test
	rm -f fibonacci_iptest
	rm -f mcache_test_fancy
	rm -rf ./*.dSYM
	rm -rf ./*.o
	rm -rf ./*.d

include $(ROOT)/deps.mk
