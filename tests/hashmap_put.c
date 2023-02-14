#include "../hashmap.h"
#include "testing.h"

int main(void)
{
	HashMap map = hashmap_new(16);

	char key[] = "hello";

	// Insert the key:value pair
	size_t hash = SuperFastHash(key, sizeof key);
	assert(hashmap_put(map, key, sizeof key, key, hash) >= 0);

	// Make sure it's in there
	void **value_ptr = hashmap_get(map, key, sizeof key, hash);
	assert(value_ptr && *value_ptr == key);

	hashmap_free(map);
	puts("passed");
	return 0;
}

