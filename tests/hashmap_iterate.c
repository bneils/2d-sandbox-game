#include "../hashmap.h"
#include "../macros.h"
#include "testing.h"
#include <string.h>

int main(void)
{
	HashMap map = hashmap_new(512);

	char *keys[] = {
		"hello",
		"world",
		"ahhh",
		"abaa",
		"never",
		"gonna",
		"let",
		"you",
		"down",
	};

	for (size_t i = 0; i < ARRAY_LEN(keys); ++i) {
		char *key = keys[i];
		size_t key_size = strlen(key) + 1;
		size_t hash = SuperFastHash(key, key_size);
		assert(hashmap_put(map, key, key_size, key, hash) >= 0);
	}

	struct HashMapIterator it;
	struct HashMapNode *node;

	hashmap_iterator_init(&it, map);
	int num_items = 0;
	// Get the key,value pairs in an arbitrary order than we put them
	while ((node = hashmap_iterate(&it))) {
		assert(node->value == node->key &&
			node->key_size == strlen(node->key) + 1);
		++num_items;
	}

	assert(num_items == ARRAY_LEN(keys));
	hashmap_free(map);
	puts("passed");
	return 0;
}

