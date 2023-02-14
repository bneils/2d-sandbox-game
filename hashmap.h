#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdint.h>

struct HashMapNode {
	void *key;
	size_t key_size;
	void *value;
	struct HashMapNode *next;
	struct HashMapNode *last;
};

typedef struct HashMap *HashMap;

struct HashMapIterator {
	size_t bucket_index;
	struct HashMapNode *current_node;
	HashMap hashmap;
};

HashMap hashmap_new(size_t num_buckets);
void hashmap_free(HashMap);
int hashmap_put(HashMap, const void *k, size_t key_size, const void *value,
	size_t hash);
void **hashmap_get(HashMap, const void *k, size_t key_size, size_t hash);
void hashmap_remove(HashMap, const void *key, size_t key_size, size_t hash);

void hashmap_iterator_init(struct HashMapIterator *, HashMap);
struct HashMapNode *hashmap_iterate(struct HashMapIterator *);

uint32_t SuperFastHash (const char * data, int len);

#endif // HASHMAP_H
