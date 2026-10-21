#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
typedef unsigned char byte;
typedef int (*binop)(int, int);
typedef struct { int w, h; } Size;
enum color { RED, GREEN = 10, BLUE, ALPHA = -1 };
typedef enum { OFF, ON } toggle;
int plus(int a, int b) { return a + b; }
struct msg { int len; char data[]; };
struct outer { int tag; union { int i; double d; }; struct { short lo, hi; }; };
int main(void) {
	byte b = 255;
	b++;
	binop f = plus;
	Size s = { 3, 4 };
	printf("%d %d %d\n", b, f(2, 3), s.w * s.h);
	printf("%d %d %d %d\n", RED, GREEN, BLUE, ALPHA);
	toggle t = ON;
	printf("%d %zu\n", t, sizeof(enum color));
	printf("%zu %zu %zu %zu\n", sizeof(char), sizeof(short), sizeof(int), sizeof(long long));
	printf("%zu %zu %zu\n", sizeof(float), sizeof(double), sizeof(Size));
	printf("%zu %zu\n", offsetof(struct outer, d), offsetof(struct outer, hi));
	struct outer o = { .tag = 1, .d = 2.5, .hi = 7 };
	printf("%d %.1f %d\n", o.tag, o.d, o.hi);
	printf("%zu\n", sizeof(struct msg));
	bool yes = 5, no = 0;
	printf("%d %d %d\n", yes, no, (bool)0.5);
	int8_t i8 = -128;
	uint16_t u16 = 65535;
	int64_t i64 = INT64_MAX;
	printf("%d %u %lld\n", i8, u16, (long long)i64);
	printf("%d\n", (int)sizeof(int[10]));
	int *p = (int[]){ 1, 2, 3 };
	Size *sp = &(Size){ 7, 8 };
	printf("%d %d\n", p[2], sp->h);
	_Static_assert(sizeof(int) == 4, "int is 4 bytes");
	printf("%zu\n", _Alignof(double));
	const volatile int cv = 9;
	printf("%d\n", cv);
	return 0;
}
