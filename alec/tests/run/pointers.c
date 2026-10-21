#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }
int apply(int (*f)(int, int), int x, int y) { return f(x, y); }
int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }
int cmp(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }
int main(void) {
	int a = 1, b = 2;
	swap(&a, &b);
	printf("%d %d\n", a, b);
	int arr[5] = { 5, 3, 9, 1, 7 };
	int *p = arr;
	printf("%d %d %d\n", *p, *(p + 2), p[4]);
	p += 3;
	printf("%d %td\n", *p, p - arr);
	int (*ops[2])(int, int) = { add, mul };
	printf("%d %d\n", apply(ops[0], 6, 7), apply(ops[1], 6, 7));
	qsort(arr, 5, sizeof arr[0], cmp);
	for (int i = 0; i < 5; i++) printf("%d ", arr[i]);
	printf("\n");
	char *s = malloc(32);
	strcpy(s, "hello");
	strcat(s, ", world");
	printf("%s %zu\n", s, strlen(s));
	char *dup = strdup(s);
	dup[0] = 'H';
	printf("%s %s %d\n", s, dup, strcmp(s, dup) > 0);
	free(s);
	free(dup);
	int m[3][4];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = i * 10 + j;
	int (*row)[4] = m + 1;
	printf("%d %d %zu\n", row[0][2], m[2][3], sizeof m);
	void *v = &m[0][1];
	printf("%d\n", *(int *)v);
	char buf[16];
	char *q = buf;
	for (const char *t = "abc"; *t; t++) *q++ = *t + 1;
	*q = 0;
	printf("%s\n", buf);
	int *np = NULL;
	printf("%d\n", np == 0);
	int **pp = &p;
	**pp = 42;
	printf("%d\n", arr[3]);
	return 0;
}
