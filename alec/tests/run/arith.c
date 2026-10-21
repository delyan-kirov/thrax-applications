#include <stdio.h>
int main(void) {
	int a = 17, b = 5;
	printf("%d %d %d %d %d\n", a + b, a - b, a * b, a / b, a % b);
	printf("%d %d %d\n", -a / b, -a % b, a / -b);
	unsigned u = 4000000000u;
	printf("%u %u %u\n", u, u / 3, u % 7);
	printf("%d %d %d %d\n", a & b, a | b, a ^ b, ~a);
	printf("%d %d %u\n", a << 3, -a >> 2, u >> 4);
	long long big = 1LL << 40;
	printf("%lld %lld\n", big, big * 3 - 7);
	unsigned long long ub = 18446744073709551615ULL;
	printf("%llu %llx\n", ub, ub / 3);
	char c = 200;
	unsigned char uc = 200;
	short s = 70000;
	printf("%d %d %d\n", c, uc, s);
	int i = 2147483647;
	i = i + 1;
	printf("%d\n", i);
	printf("%d %d\n", 7 > 3, 3 >= 4);
	printf("%d %d %d\n", !0, !5, (a > b) && (b > 0) || 0);
	int x = 10;
	x += 5; x -= 3; x *= 2; x /= 4; x %= 4; x <<= 3; x >>= 1; x |= 1; x &= 7; x ^= 2;
	printf("%d\n", x);
	int y = 5;
	int z = y++;
	z += ++y;
	printf("%d %d\n", y, z);
	printf("%d\n", (1, 2, 3));
	printf("%d\n", a > b ? a : b);
	return 0;
}
