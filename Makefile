CFLAGS=`pkg-config --cflags glib-2.0` -Wall -g
LIBS= `pkg-config --libs glib-2.0`

test_lru: test_lru.c lru_dict.c
	gcc ${CFLAGS} -o $@ $^ ${LIBS}
