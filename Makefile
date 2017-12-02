CFLAGS=`pkg-config --cflags glib-2.0` -Wall -g
LIBS= `pkg-config --libs glib-2.0`

test_lru: test_lru.c lru_dict.o
	gcc ${CFLAGS} -o $@ $^ ${LIBS}

lru_dict.o: lru_dict.c
	gcc ${CFLAGS} -c $^

clean:
	rm -rf lru_dict.o
	rm -rf test_lru
