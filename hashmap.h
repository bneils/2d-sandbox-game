#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct HashMap *HashMap;

HashMap hashmap_new(size_t num_buckets);
void hashmap_free(HashMap);
int hashmap_put(HashMap, const void *k, size_t key_size, const void *value,
	size_t hash);
void **hashmap_get(HashMap, const void *k, size_t key_size, size_t hash);
void hashmap_remove(HashMap, const void *key, size_t key_size, size_t hash);

#endif // HASHMAP_H
