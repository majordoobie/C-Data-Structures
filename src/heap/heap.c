typedef struct
{
    int length;
	int heap_size;
	int (* compare)(const void *key1, const void *key2);
	void (* destroy)(void *data);
} heap_t;