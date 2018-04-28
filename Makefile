CFLAGS=-Wall -std=gnu99 # -fsanitize=address -g
CC=clang

server: server.c server.h socket_list.c socket_list.h hashmap.h hashmap.c entry_list.c entry_list.h mcache_types.h
	${CC} ${CFLAGS} -o server server.c socket_list.c hashmap.c entry_list.c

server_test: server_test.c server.h mcache_types.h server
	${CC} ${CFLAGS} -o server_test server_test.c

hashmap_test: hashmap_test.c hashmap.h hashmap.c entry_list.h entry_list.c mcache_types.h
	${CC} ${CFLAGS} -o hashmap_test hashmap_test.c hashmap.c entry_list.c

tidy:
	rm -f *~

clean: tidy
	rm -f server
	rm -f hashmap_test
	rm -f server_test
