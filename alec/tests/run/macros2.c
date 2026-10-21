#include <stdio.h>
#include "macros2.h"
#include "macros2.h"
#define COLORS(X) X(red, 1) X(green, 2) X(blue, 4)
#define AS_ENUM(name, val) C_##name = val,
#define AS_STR(name, val) #name,
enum { COLORS(AS_ENUM) C_COUNT };
static const char *color_names[] = { COLORS(AS_STR) };
#define REPEAT3(x) x x x
#define EMPTY
#define DEFER(m) m EMPTY
#define PRINT_ALL(...) printf(#__VA_ARGS__ "\n")
#define COUNT_ARGS(...) COUNT_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)
#define COUNT_IMPL(a, b, c, d, e, N, ...) N
#define F(x) G(x)
#define G(x) x + 1
#define OBJ F(2)
int main(void) {
	printf("%d %d %d %s\n", C_red, C_green, C_blue, color_names[2]);
	REPEAT3(printf("hi ");)
	printf("\n");
	PRINT_ALL(a, b,   c);
	printf("%d %d %d\n", COUNT_ARGS(x), COUNT_ARGS(x, y, z), COUNT_ARGS(1, 2, 3, 4, 5));
	printf("%d\n", OBJ * 2);
	printf("%d %d\n", HEADER_VALUE, twice(21));
	printf("%d %d\n", __COUNTER__, __COUNTER__);
#if defined(HEADER_VALUE) && HEADER_VALUE > 10 || 0
	printf("header ok\n");
#elif 1
	printf("wrong\n");
#endif
#if 0
	this is not even C
#endif
	return 0;
}
