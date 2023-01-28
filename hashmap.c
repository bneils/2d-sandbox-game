#include "hashmap.h"
#include <stdlib.h>
#include <string.h>

struct HashMapNode {
	void *key;
	size_t key_size;
	void *value;
	struct HashMapNode *next;
	struct HashMapNode *last;
};

struct HashMap {
	struct HashMapNode **buckets;
	size_t num_buckets;
};

static struct HashMapNode *hashmap_get_node(HashMap hashmap,
	const void *key, size_t key_size, size_t hash);

/* `hashmap_new` creates a new hashmap with a fixed number of buckets */
HashMap hashmap_new(size_t num_buckets)
{
	HashMap hashmap = malloc(sizeof(*hashmap));
	if (!hashmap)
		return NULL;

	hashmap->buckets = calloc(num_buckets, sizeof(*hashmap->buckets));
	if (!hashmap->buckets) {
		free(hashmap);
		return NULL;
	}
	hashmap->num_buckets = num_buckets;

	return hashmap;
}

/* `hashmap_free` frees all resources allocated for the hashmap. */
void hashmap_free(HashMap hashmap)
{
	if (!hashmap)
		return;
	free(hashmap->buckets);
	free(hashmap);
}

/* `hashmap_put` creates or changes an entry inside of a hashmap to point to
 * a value.
 */
int hashmap_put(HashMap hashmap, const void *key, size_t key_size,
	const void *value, size_t hash)
{
	if (!hashmap)
		return -1;

	const void **entry_value = (const void **) hashmap_get(hashmap, key, key_size, hash);
	if (entry_value) {
		*entry_value = value;
		return 0;
	}

	struct HashMapNode **head_ptr =
		&hashmap->buckets[hash % hashmap->num_buckets];
	struct HashMapNode *node = malloc(sizeof(*node));
	if (!node)
		return -1;
	node->key = (void *) key;
	node->key_size = key_size;
	node->value = (void *) value;
	node->next = *head_ptr;
	node->last = NULL;
	if (*head_ptr)
		(*head_ptr)->last = node;
	*head_ptr = node;

	return 0;
}

/* `hashmap_get_node` gets the internal node used in the hash map for a certain
 * key. */
static struct HashMapNode *hashmap_get_node(HashMap hashmap, const void *key,
	size_t key_size, size_t hash)
{
	if (!hashmap)
		return NULL;

	struct HashMapNode *head =
		hashmap->buckets[hash % hashmap->num_buckets];
	while (head) {
		if (head->key_size == key_size &&
			memcmp(head->key, key, key_size) == 0)
			return head;
		head = head->next;
	}

	return NULL;
}

/* `hashmap_get` returns a pointer to the value corresponding to key.
 * This value is not immutable and can change whenever a new put is made for
 * the same key. If the key is removed, the pointer becomes invalid. */
void **hashmap_get(HashMap hashmap, const void *key, size_t key_size,
	size_t hash)
{
	struct HashMapNode *node =
		hashmap_get_node(hashmap, key, key_size, hash);
	if (!node)
		return NULL;
	return &node->value;
}

void hashmap_remove(HashMap hashmap, const void *key, size_t key_size,
	size_t hash)
{
	struct HashMapNode *node =
		hashmap_get_node(hashmap, key, key_size, hash);
	if (!node)
		return;

	if (node->last)
	// Node is not the first item in the list
		node->last->next = node->next;
	else
	// Since node is the first, we need to adjust the bucket list
		hashmap->buckets[hash % hashmap->num_buckets] = node->next;

	if (node->next)
		node->next->last = node->last;
}
