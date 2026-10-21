#include <stdio.h>
int classify(int n) {
	switch (n) {
	case 0: return 100;
	case 1:
	case 2: return 200;
	case 5 ... 9: return 300;
	default: return -1;
	}
}
int dense(int n) {
	int r = 0;
	switch (n) {
	case 1: r += 1;
	case 2: r += 2; break;
	case 3: r += 3; break;
	case 4: r += 4;
	case 5: r += 5; break;
	case 6: r = 66; break;
	}
	return r;
}
int main(void) {
	for (int i = -1; i < 11; i++) printf("%d ", classify(i));
	printf("\n");
	for (int i = 0; i < 8; i++) printf("%d ", dense(i));
	printf("\n");
	int i = 0, s = 0;
	while (1) {
		i++;
		if (i % 2) continue;
		if (i > 10) break;
		s += i;
	}
	printf("%d\n", s);
	do { s--; } while (s > 25);
	printf("%d\n", s);
	int n = 0;
	for (int a = 0; a < 5; a++)
		for (int b = 0; b < 5; b++) {
			if (b > a) break;
			n += a * b;
		}
	printf("%d\n", n);
	int k = 0;
again:
	k++;
	if (k < 5) goto again;
	printf("%d\n", k);
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 3; b++) {
			if (a * b == 2) goto done;
			printf("(%d,%d)", a, b);
		}
	}
done:
	printf("\n");
	int x = 3;
	if (x > 5) printf("big\n"); else if (x > 2) printf("medium\n"); else printf("small\n");
	return 7;
}
