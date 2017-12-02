#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lru_dict.h"

guint
int_hash(gconstpointer v)
{
    return (guint)v;
}

gboolean
int_equal(gconstpointer v1, gconstpointer v2)
{
    return ((guint)v1 == (guint)v2);
}

void
test1()
{
    printf("test1 begin!\n");
    lru_dict_t  *lru = lru_dict_new(10, int_hash, int_equal);

    gpointer ret = lru_dict_lookup(lru, GINT_TO_POINTER(10));
    g_assert(ret == NULL);

    lru_dict_insert(lru, GINT_TO_POINTER(10), GINT_TO_POINTER(20));
    ret = lru_dict_lookup(lru, GINT_TO_POINTER(10));
    g_assert(ret == GINT_TO_POINTER(20));



    int i;
    for(i = 0; i < 10; i++){
        lru_dict_insert(lru, GINT_TO_POINTER(11 + i), GINT_TO_POINTER(21 + i));
    }

    ret = lru_dict_lookup(lru, GINT_TO_POINTER(10));
    g_assert(ret == NULL);

    ret = lru_dict_lookup(lru, GINT_TO_POINTER(11));
    g_assert(ret == GINT_TO_POINTER(21));

    lru_dict_insert(lru, GINT_TO_POINTER(10), GINT_TO_POINTER(20));
    lru_dict_insert(lru, GINT_TO_POINTER(9), GINT_TO_POINTER(19));

    ret = lru_dict_lookup(lru, GINT_TO_POINTER(12));
    g_assert(ret == NULL);
    ret = lru_dict_lookup(lru, GINT_TO_POINTER(13));
    g_assert(ret == NULL);

    ret = lru_dict_lookup(lru, GINT_TO_POINTER(11));
    g_assert(ret == GINT_TO_POINTER(21));

    g_assert(lru_dict_size(lru) == 10);

    lru_dict_remove(lru, GINT_TO_POINTER(11));
    ret = lru_dict_lookup(lru, GINT_TO_POINTER(11));
    g_assert(ret == NULL);

    lru_dict_free(lru);
    printf("test1 end!\n");
}

static void
test2()
{
    printf("test2 begin\n");

    char    key_buf[128], value_buf[128];
    int     i;

    lru_dict_t  *lru = lru_dict_new_full(0, g_str_hash, g_str_equal, free, free);

    for(i = 0; i < 100000; i++){
        sprintf(key_buf, "this is key buf %08d", i);
        sprintf(value_buf, "this is value buf of this is key buf %08d", i);

        lru_dict_insert(lru, strdup(key_buf), strdup(value_buf));
    }


    gpointer p;
    for(i = 0; i < 100000; i++){
        sprintf(key_buf, "this is key buf %08d", i);
        p = lru_dict_lookup(lru, key_buf);
        g_assert(p != NULL);
        sprintf(value_buf, "this is value buf of this is key buf %08d", i);

        g_assert(strcmp(value_buf, (char *)p) == 0);
    }


    for(i = 0; i < 100000; i++){
        sprintf(key_buf, "this is key buf %08d", i);
        lru_dict_remove(lru, key_buf);
    }

    g_assert(lru_dict_size(lru) == 0);
    lru_dict_free(lru);
    printf("test2 end\n");
}

int
main(void)
{
    test1();

    test2();

    return 0;
}
