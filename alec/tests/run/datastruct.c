#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct tree { int key; struct tree *left, *right; } tree;
tree *insert(tree *t, int key) {
	if (!t) {
		t = calloc(1, sizeof *t);
		t->key = key;
		return t;
	}
	if (key < t->key) t->left = insert(t->left, key);
	else t->right = insert(t->right, key);
	return t;
}
void walk(tree *t) { if (!t) return; walk(t->left); printf("%d ", t->key); walk(t->right); }
int height(tree *t) { if (!t) return 0; int l = height(t->left), r = height(t->right); return 1 + (l > r ? l : r); }
void freetree(tree *t) { if (t) { freetree(t->left); freetree(t->right); free(t); } }
#define BUCKETS 31
typedef struct entry { char *key; int value; struct entry *next; } entry;
entry *table[BUCKETS];
unsigned hash(const char *s) { unsigned h = 5381; while (*s) h = h * 33 + (unsigned char)*s++; return h % BUCKETS; }
void put(const char *k, int v) {
	unsigned h = hash(k);
	for (entry *e = table[h]; e; e = e->next) if (!strcmp(e->key, k)) { e->value = v; return; }
	entry *e = malloc(sizeof *e);
	e->key = strdup(k);
	e->value = v;
	e->next = table[h];
	table[h] = e;
}
int get(const char *k) { for (entry *e = table[hash(k)]; e; e = e->next) if (!strcmp(e->key, k)) return e->value; return -1; }
int main(void) {
	tree *t = 0;
	int keys[] = { 50, 30, 70, 20, 40, 60, 80, 35, 45, 65 };
	for (int i = 0; i < 10; i++) t = insert(t, keys[i]);
	walk(t);
	printf("\nheight %d\n", height(t));
	freetree(t);
	const char *words[] = { "apple", "banana", "cherry", "apple", "date", "banana", "apple" };
	for (int i = 0; i < 7; i++) put(words[i], get(words[i]) < 0 ? 1 : get(words[i]) + 1);
	printf("%d %d %d %d %d\n", get("apple"), get("banana"), get("cherry"), get("date"), get("fig"));
	int *v = 0;
	int n = 0, cap = 0;
	for (int i = 0; i < 1000; i++) {
		if (n == cap) { cap = cap ? cap * 2 : 4; v = realloc(v, cap * sizeof *v); }
		v[n++] = i * i;
	}
	long long s = 0;
	for (int i = 0; i < n; i++) s += v[i];
	printf("%lld %d\n", s, cap);
	free(v);
	return 0;
}
