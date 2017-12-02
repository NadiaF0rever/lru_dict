/*
 * 2017 (c) NadiaF0rever
 */

#ifndef TCPREASSEMBLY_LRU_DICT_H
#define TCPREASSEMBLY_LRU_DICT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>

struct lru_dict_s;
typedef struct lru_dict_s lru_dict_t;

lru_dict_t * lru_dict_new(guint max_size, GHashFunc hash_func, GEqualFunc key_equal_func);

lru_dict_t * lru_dict_new_full(guint max_size,
        GHashFunc hash_func, GEqualFunc key_equal_func,
        GDestroyNotify key_destory_func, GDestroyNotify value_destroy_func);

void lru_dict_free(lru_dict_t *);

gpointer lru_dict_lookup(lru_dict_t *, gconstpointer key);

void lru_dict_remove(lru_dict_t *, gconstpointer key);

void lru_dict_insert(lru_dict_t *, gpointer key, gpointer value);

guint lru_dict_size(lru_dict_t *);

#ifdef __cplusplus
}
#endif

#endif
