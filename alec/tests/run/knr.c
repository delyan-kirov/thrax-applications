#include <stdio.h>
int add(a, b)
int a;
int b;
{
	return a + b;
}
double half(x)
double x;
{
	return x / 2;
}
int count(s) char *s; { int n = 0; while (*s++) n++; return n; }
int main(void) {
	printf("%d %.2f %d\n", add(2, 3), half(5.0), count("hello"));
	return 0;
}
