#include <stdio.h>
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define STR(x) #x
#define XSTR(x) STR(x)
#define CAT(a, b) a ## b
#define VERSION 3
#define LOG(fmt, ...) printf("[log] " fmt "\n", ##__VA_ARGS__)
#if VERSION >= 3 && defined(SQUARE)
#define MODE "new"
#else
#define MODE "old"
#endif
#ifdef UNDEFINED_THING
#error not here
#endif
int CAT(my, var) = 7;
int main(void) {
	printf("%d %d\n", SQUARE(3 + 1), MAX(4, 9));
	printf("%s %s %s\n", STR(a + b), XSTR(VERSION), MODE);
	printf("%d\n", myvar);
	LOG("plain");
	LOG("%d and %s", 1, "two");
	printf("%d %s\n", __LINE__, __FILE__ + 0 == 0 ? "?" : "file");
#undef VERSION
#ifndef VERSION
	printf("undefined now\n");
#endif
	return 0;
}
