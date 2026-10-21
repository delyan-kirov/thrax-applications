/* A size-class allocator: every block is a power of two, at least 16 bytes,
   with an 8-byte header holding its class. Freed blocks go on their class's
   list and are reused first; new ones are carved from the top of the heap,
   which grows with memory.grow. */
#include <stdlib.h>
#include <string.h>
#include "sys.h"

#define HEADER 8
#define MIN_CLASS 4
#define NCLASS 48

static char *free_lists[NCLASS];
static char *heap_cur;
static char *heap_end;

static char *carve(size_t n) {
	if (!heap_cur) {
		heap_cur = __heap_base;
		heap_end = (char *)(__builtin_wasm_memory_size() * 65536);
	}
	while ((size_t)(heap_end - heap_cur) < n) {
		size_t pages = (n - (size_t)(heap_end - heap_cur) + 65535) / 65536;
		if (__builtin_wasm_memory_grow(pages) == (size_t)-1)
			return 0;
		heap_end += pages * 65536;
	}
	char *p = heap_cur;
	heap_cur += n;
	return p;
}

static int class_of(size_t total) {
	int k = MIN_CLASS;
	while (((size_t)1 << k) < total)
		k++;
	return k;
}

void *malloc(size_t n) {
	if (n > ((size_t)1 << (sizeof(size_t) * 8 - 2)))
		return 0;
	int k = class_of(n + HEADER);
	char *b = free_lists[k];
	if (b)
		free_lists[k] = *(char **)(b + HEADER);
	else if (!(b = carve((size_t)1 << k)))
		return 0;
	*(size_t *)b = k;
	return b + HEADER;
}

void free(void *p) {
	if (!p)
		return;
	char *b = (char *)p - HEADER;
	int k = *(size_t *)b;
	*(char **)(b + HEADER) = free_lists[k];
	free_lists[k] = b;
}

void *calloc(size_t count, size_t size) {
	if (size && count > (size_t)-1 / size)
		return 0;
	void *p = malloc(count * size);
	if (p)
		memset(p, 0, count * size);
	return p;
}

void *realloc(void *p, size_t n) {
	if (!p)
		return malloc(n);
	size_t cap = ((size_t)1 << *(size_t *)((char *)p - HEADER)) - HEADER;
	if (n <= cap)
		return p;
	void *q = malloc(n);
	if (!q)
		return 0;
	memcpy(q, p, cap);
	free(p);
	return q;
}

void *aligned_alloc(size_t align, size_t n) {
	return malloc(n < align ? align : n);
}
