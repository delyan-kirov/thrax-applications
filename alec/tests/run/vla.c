#include <stdio.h>
void fill(int n, int m, int a[n][m]) {
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			a[i][j] = i * 10 + j;
}
int trace(int n, int a[n][n]) {
	int t = 0;
	for (int i = 0; i < n; i++) t += a[i][i];
	return t;
}
void matmul(int n, double a[n][n], double b[n][n], double c[n][n]) {
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) {
			double s = 0;
			for (int k = 0; k < n; k++) s += a[i][k] * b[k][j];
			c[i][j] = s;
		}
}
int main(void) {
	int n = 3, m = 4;
	int grid[n][m];
	fill(n, m, grid);
	printf("%d %d %d %zu %zu\n", grid[0][3], grid[2][1], grid[1][2], sizeof grid, sizeof grid[0]);
	int sq[4][4];
	fill(4, 4, sq);
	printf("%d\n", trace(4, sq));
	double a[2][2] = { { 1, 2 }, { 3, 4 } }, b[2][2] = { { 5, 6 }, { 7, 8 } }, c[2][2];
	matmul(2, a, b, c);
	printf("%.0f %.0f %.0f %.0f\n", c[0][0], c[0][1], c[1][0], c[1][1]);
	int (*p)[m] = grid;
	p++;
	printf("%d %td\n", p[0][1], p - grid);
	for (int k = 1; k <= 3; k++) {
		char buf[k * 2];
		printf("%zu ", sizeof buf);
	}
	printf("\n");
	return 0;
}
