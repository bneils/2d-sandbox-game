#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "globals.h"

struct HashMap {
	struct HashMapNode **buckets;
	size_t num_buckets;
};

static struct HashMapNode *hashmap_get_node(HashMap hashmap,
	const void *key, size_t key_size, size_t hash);

/**
 * Creates a new hashmap with a fixed number of buckets
 * @param num_buckets The number of buckets used internally for the hashmap,
 * 	with more meaning less key depth and better fetch times
 * @return The hash map, or NULL if an error occurred
 */
HashMap hashmap_new(size_t num_buckets)
{
	HashMap hashmap = malloc(sizeof(*hashmap));
	if (!hashmap) {
		g_error_message = "malloc failed";
		return NULL;
	}

	hashmap->buckets = calloc(num_buckets, sizeof(*hashmap->buckets));
	if (!hashmap->buckets) {
		g_error_message = "malloc failed";
		free(hashmap);
		return NULL;
	}
	hashmap->num_buckets = num_buckets;

	return hashmap;
}

/**
 * Frees all resources allocated for the hashmap
 * @param hashmap The hashmap to be freed
 */
void hashmap_free(HashMap hashmap)
{
	if (!hashmap)
		return;

	free(hashmap->buckets);
	free(hashmap);
}

/**
 * Creates or changes an entry inside of a hashmap to point to
 * a value
 * @param hashmap The hash map to modify
 * @param key A pointer to any data to use for the key
 * @param key_size The size of key
 * @value The pointer to return when accessing with this key
 * @hash The hashed value of this key
 * 	Only a single hashing function should be used for a hashmap
 * @return 0 on success and a negative value on error
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
	if (!node) {
		g_error_message = "malloc failed";
		return -1;
	}
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

/**
 * Gets the internal node used in the hash map for a certain key
 * @param hashmap The hash map to get from
 * @param key A pointer to data
 * @param key_size The size of key
 * @param hash The hashed value of the key
 * @return The HashMapNode with a matching key or NULL if none matched
 */
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

/**
 * Fetches from a hash map
 * @param hashmap The hash map being used
 * @param key A pointer to data
 * @param key_size The size of key
 * @param hash A hash of key
 * @return A pointer to the pointer value or NULL if it doesn't exist.
 * Can be dereferenced to be used as a r or l-value and represents the
 * tentative address within this entry that will become invalid once this entry
 * is removed or the hashmap is freed.
 */
void **
hashmap_get(HashMap hashmap, const void *key, size_t key_size, size_t hash)
{
	struct HashMapNode *node =
		hashmap_get_node(hashmap, key, key_size, hash);
	if (!node)
		return NULL;
	return &node->value;
}

/**
 * Removes an entry from a hashmap
 * @param hashmap The hashmap to remove from
 * @param key The key that identifies the entry
 * @param key_size The size of the key
 * @param hash The key's hash
 */
void
hashmap_remove(HashMap hashmap, const void *key, size_t key_size, size_t hash)
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

/**
 * Initializes a hashmap iterator to begin iterating
 * @param it A pointer to the uninitialized hash map iterator structure
 * @param hashmap The hashmap to iterate
 */
void hashmap_iterator_init(struct HashMapIterator *it, HashMap hashmap)
{
	if (!it)
		return;
	it->bucket_index = 0;
	it->hashmap = hashmap;
	it->current_node = (hashmap) ? hashmap->buckets[0] : NULL;
}

/**
 * Gets the next item in the iterator
 * @param it An initialized iterator
 * @return A pointer to the HashMapNode, or NULL if no more exist
 */
struct HashMapNode *hashmap_iterate(struct HashMapIterator *it)
{
	if (!it || !it->hashmap)
		return NULL;

	// Find the next bucket to iterate through
	while (!it->current_node) {
		if (it->bucket_index >= it->hashmap->num_buckets - 1)
			return NULL;
		it->current_node = it->hashmap->buckets[++it->bucket_index];
	}

	struct HashMapNode *element = it->current_node;
	if (!element)
		return NULL;
	it->current_node = it->current_node->next;
	return element;
}
