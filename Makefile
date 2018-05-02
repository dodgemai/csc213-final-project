CFLAGS=-Wall -std=gnu99 # -fsanitize=address -g
CC=clang -g

server: server.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o server server.c socket_list.c hashmap.c entry_list.c mcache.c

server_test: server_test.c server.h mcache_types.h server
	${CC} ${CFLAGS} -o server_test server_test.c

hashmap_test: hashmap_test.c hashmap.h hashmap.c entry_list.h entry_list.c mcache_types.h
	${CC} ${CFLAGS} -o hashmap_test hashmap_test.c hashmap.c entry_list.c

client: client.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h mcache.h mcache.c
	${CC} ${CFLAGS} -o client client.c mcache.c

tidy:
	rm -f *~

clean: tidy
	rm -f server
	rm -f client
	rm -f hashmap_test
	rm -f server_test
	rm -rf ./*.dSYM
