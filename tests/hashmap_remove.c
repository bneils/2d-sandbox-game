#include "../hashmap.h"
#include "testing.h"

int main(void)
{
	HashMap map = hashmap_new(8);

	char key[] = "hello";

	size_t hash = SuperFastHash(key, sizeof key);

	// map[key] = key
	assert(hashmap_put(map, key, sizeof key, key, hash) >= 0);

	// key in map and map[key] == key
	void **value_ptr = hashmap_get(map, key, sizeof key, hash);
	assert(value_ptr && *value_ptr == key);

	hashmap_remove(map, key, sizeof key, hash);
	value_ptr = hashmap_get(map, key, sizeof key, hash);
	assert(!value_ptr);

	hashmap_free(map);
	puts("passed");
	return 0;
}

