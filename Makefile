CFLAGS=-Wall -std=gnu99 -g # -fsanitize=address
CC=clang

.PHONY: all tidy clean

all: server mcache_test mcache_iptest

server: server.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c key_list.c key_list.h
	${CC} ${CFLAGS} -o server server.c socket_list.c hashmap.c entry_list.c mcache.c key_list.c

server_test: server_test.c server.h mcache_types.h server
	${CC} ${CFLAGS} -o server_test server_test.c

hashmap_test: hashmap_test.c hashmap.h hashmap.c entry_list.h entry_list.c mcache_types.h
	${CC} ${CFLAGS} -o hashmap_test hashmap_test.c hashmap.c entry_list.c

mcache_test: mcache_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o mcache_test mcache_test.c mcache.c
client_old: client_basic.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o client_old client_basic.c mcache.c

mcache_iptest: mcache_test.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache_inclient.c key_list.c key_list.h
	${CC} ${CFLAGS} -o mcache_iptest mcache_test.c mcache_inclient.c hashmap.c entry_list.c key_list.c
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
