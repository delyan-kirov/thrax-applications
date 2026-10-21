#include <stdio.h>
#define TYPE_NAME(x) _Generic((x), int: "int", double: "double", char *: "string", default: "other")
#define MAX(a, b) ({ typeof(a) _a = (a); typeof(b) _b = (b); _a > _b ? _a : _b; })
int sum_vla(int n) {
	int a[n];
	for (int i = 0; i < n; i++) a[i] = i * i;
	int s = 0;
	for (int i = 0; i < n; i++) s += a[i];
	return s + (int)(sizeof a / sizeof a[0]);
}
int main(void) {
	printf("%s %s %s %s\n", TYPE_NAME(1), TYPE_NAME(2.0), TYPE_NAME("s"), TYPE_NAME('c'));
	int i = 3;
	printf("%d %d\n", MAX(i++, 2), i);
	printf("%d\n", sum_vla(10));
	int v = ({ int t = 4; t * t; });
	printf("%d\n", v);
	typeof(v) w = v + 1;
	printf("%d\n", w);
	int x = 0 ?: 5;
	printf("%d\n", x);
	for (int k = 0; k < 2; k++) {
		static int seen;
		seen += 10;
		printf("%d ", seen);
	}
	printf("\n");
	int arr[] = { [3] = 30, [1] = 10, 11 };
	printf("%zu %d %d %d\n", sizeof arr / sizeof *arr, arr[1], arr[2], arr[3]);
	int grid[3][3] = { [1][1] = 5, [2] = { 7, 8, 9 } };
	printf("%d %d %d\n", grid[1][1], grid[2][0], grid[0][0]);
	int nums[10] = { [2 ... 5] = 1, [8] = 2 };
	for (int k = 0; k < 10; k++) printf("%d", nums[k]);
	printf("\n");
	return 0;
}
