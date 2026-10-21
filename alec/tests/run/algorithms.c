#include <stdio.h>
#include <string.h>
#define N 8
int queens(int row, int cols, int d1, int d2) {
	if (row == N) return 1;
	int count = 0;
	for (int c = 0; c < N; c++) {
		if (cols & (1 << c) || d1 & (1 << (row + c)) || d2 & (1 << (row - c + N))) continue;
		count += queens(row + 1, cols | 1 << c, d1 | 1 << (row + c), d2 | 1 << (row - c + N));
	}
	return count;
}
void sieve(int n) {
	char comp[200] = { 0 };
	int count = 0, last = 0;
	for (int i = 2; i < n; i++) {
		if (comp[i]) continue;
		count++;
		last = i;
		for (int j = i * i; j < n; j += i) comp[j] = 1;
	}
	printf("%d primes below %d, largest %d\n", count, n, last);
}
void matmul(void) {
	double a[3][3] = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } }, b[3][3], c[3][3] = { { 0 } };
	for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) b[i][j] = i == j ? 2 : 0;
	for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) for (int k = 0; k < 3; k++) c[i][j] += a[i][k] * b[k][j];
	for (int i = 0; i < 3; i++) printf("%.0f %.0f %.0f\n", c[i][0], c[i][1], c[i][2]);
}
void mandel(void) {
	for (int y = -6; y <= 6; y++) {
		for (int x = -20; x <= 8; x++) {
			double cr = x / 10.0, ci = y / 6.0, zr = 0, zi = 0;
			int k = 0;
			while (k < 50 && zr * zr + zi * zi < 4) { double t = zr * zr - zi * zi + cr; zi = 2 * zr * zi + ci; zr = t; k++; }
			putchar(k == 50 ? '#' : k > 10 ? '+' : '.');
		}
		putchar('\n');
	}
}
unsigned crc32(const char *s) {
	unsigned crc = 0xFFFFFFFFu;
	for (; *s; s++) { crc ^= (unsigned char)*s; for (int k = 0; k < 8; k++) crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320u : crc >> 1; }
	return ~crc;
}
int main(void) {
	printf("%d\n", queens(0, 0, 0, 0));
	sieve(200);
	matmul();
	mandel();
	printf("%08x\n", crc32("The quick brown fox jumps over the lazy dog"));
	return 0;
}
