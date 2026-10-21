#include <stdio.h>
int counter;
int table[] = { 1, 2, 3, 4 };
const char *names[] = { "zero", "one", "two" };
static int hidden = 5;
double scale = 2.5;
struct cfg { const char *name; int level; int *ptr; } config = { "default", 3, &table[2] };
int *tp = table + 1;
char greeting[16] = "hi";
int matrix[2][2] = { 1, 2, 3, 4 };
int bump(void) { static int calls = 0; return ++calls + hidden; }
int main(void) {
	counter += 10;
	printf("%d %zu %s\n", counter, sizeof table / sizeof table[0], names[2]);
	printf("%s %d %d\n", config.name, config.level, *config.ptr);
	printf("%d %f %s\n", *tp, scale, greeting);
	int b1 = bump(), b2 = bump(), b3 = bump();
	printf("%d %d %d\n", b1, b2, b3);
	printf("%d\n", matrix[1][0]);
	table[0] = 99;
	printf("%d %d\n", table[0], tp[-1]);
	return 0;
}
