/*
 * 2017 (c) NadiaF0rever
 */

#include "lru_dict.h"

#ifdef DEV
#include <stdio.h>
#define log_dev(fmt, ...)   printf("[DEV] "fmt"\n", ##__VA_ARGS__)
#else
#define log_dev(fmt, ...)   do{}while(0)
#endif

struct lru_dict_s {
    GHashTable  *hash;
    GList           *list;
    GDestroyNotify  key_destory_func;
    GDestroyNotify  value_destroy_func;
    guint           max_size;
};

struct lru_dict_node_s;
typedef struct lru_dict_node_s lru_dict_node_t;
struct lru_dict_node_s {
    gpointer    key;
    gpointer    value;
};

static void lru_dict_evicit(lru_dict_t *);
static void lru_dict_move_node_front(lru_dict_t *, GList *);
static void lru_dict_node_free_cb(gpointer p);

lru_dict_t *
lru_dict_new(guint max_size, GHashFunc hash_func, GEqualFunc key_equal_func)
{
    return lru_dict_new_full(max_size, hash_func, key_equal_func, NULL, NULL);
}

lru_dict_t *
lru_dict_new_full(guint max_size, GHashFunc hash_func, GEqualFunc key_equal_func,
        GDestroyNotify key_destory_func, GDestroyNotify value_destroy_func)
{
    lru_dict_t      *ret = g_new0(lru_dict_t, 1);

    ret->hash = g_hash_table_new_full(hash_func, key_equal_func, NULL, NULL);

    ret->max_size = max_size;
    ret->list = NULL;
    ret->key_destory_func = key_destory_func;
    ret->value_destroy_func = value_destroy_func;

    return ret;
}

void
lru_dict_free(lru_dict_t *lru)
{
    g_return_if_fail(lru != NULL);
    g_hash_table_destroy(lru->hash);
    g_list_free_full(lru->list, lru_dict_node_free_cb);
    g_free(lru);
}


void
lru_dict_remove(lru_dict_t *lru, gconstpointer key)
{
    g_return_if_fail(lru != NULL);

    GList * node = (GList *)g_hash_table_lookup(lru->hash, key);
    if(!node){
        return;
    }

    lru_dict_node_t *data = (lru_dict_node_t *)node->data;
    g_hash_table_remove(lru->hash, data->key);

    if(lru->key_destory_func){
        lru->key_destory_func(data->key);
    }

    if(lru->value_destroy_func){
        lru->value_destroy_func(data->value);
    }

    lru_dict_node_free_cb(data);

    lru->list = g_list_delete_link(lru->list, node);
}


void
lru_dict_insert(lru_dict_t *lru, gpointer key, gpointer value)
{
    g_return_if_fail(lru != NULL);

    GList *node = (GList *)g_hash_table_lookup(lru->hash, key);

    if(node){
        lru_dict_move_node_front(lru, node);
        return;
    }

    if(lru->max_size != 0 && g_hash_table_size(lru->hash) >= lru->max_size){
        lru_dict_evicit(lru);
    }

    lru_dict_node_t *data = g_slice_new(lru_dict_node_t);
    data->key = key;
    data->value = value;
    
    node = g_list_prepend(lru->list, data);
    lru->list = node;
    g_hash_table_insert(lru->hash, key, node);
}

gpointer
lru_dict_lookup(lru_dict_t *lru, gconstpointer key)
{
    g_return_val_if_fail(lru != NULL, NULL);
    GList *node = (GList *)g_hash_table_lookup(lru->hash, key);
    if(node){
        lru_dict_move_node_front(lru, node);
        lru_dict_node_t *data = (lru_dict_node_t *)node->data;
        return data->value;
    }

    return NULL;
}

guint
lru_dict_size(lru_dict_t *lru)
{
    g_return_val_if_fail(lru != NULL, 0);
    return g_hash_table_size(lru->hash);
}

static void
lru_dict_evicit(lru_dict_t *lru)
{
    GList *tail = g_list_last(lru->list);
    g_assert(tail != NULL);
    lru_dict_node_t *data = (lru_dict_node_t*)tail->data;
    log_dev("EVICIT %p", data->key);

    lru_dict_remove(lru, data->key);
}

static void
lru_dict_move_node_front(lru_dict_t *lru, GList *node)
{
    lru->list = g_list_remove_link(lru->list, node);
    lru->list = g_list_concat(node, lru->list);
}

static void
lru_dict_node_free_cb(gpointer p)
{
    g_slice_free(lru_dict_node_t, p);
}
