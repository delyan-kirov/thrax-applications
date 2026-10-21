#include <stdio.h>
int main(void) {
	unsigned u = 1;
	int i = -1;
	printf("%d\n", i < u);
	printf("%d\n", (long long)i < (long long)u);
	unsigned char c = 255;
	printf("%d %d\n", c + 1, (unsigned char)(c + 1));
	signed char sc = -1;
	printf("%u %d\n", (unsigned)sc, (unsigned char)sc);
	short s = -2;
	printf("%d %u\n", s >> 1, (unsigned short)s);
	long long ll = -5;
	printf("%lld %llu\n", ll / 2, (unsigned long long)ll >> 60);
	printf("%d\n", 'A' + 2);
	double d = 1e10;
	printf("%lld\n", (long long)d);
	float f = 0.1f;
	printf("%.10f\n", f);
	printf("%d %d\n", 7 / 2 * 2, 7 / 2.0 > 3);
	int big = 1 << 30;
	printf("%lld\n", (long long)big * 4);
	unsigned long long mask = ~0ULL;
	printf("%llx %llx\n", mask, mask << 63);
	int neg = -7;
	printf("%d %d %u\n", neg / 2, neg % 3, (unsigned)neg % 3);
	char str[4] = { 'a', 'b', 'c', 0 };
	int sum = 0;
	for (char *p = str; *p; p++) sum += *p;
	printf("%d\n", sum);
	_Bool b = 256;
	printf("%d\n", b);
	printf("%d\n", (int)(char)0x1ff);
	unsigned x = 0x80000000u;
	printf("%d %u\n", (int)x, x >> 31);
	printf("%d\n", -1 == ~0);
	return 0;
}
