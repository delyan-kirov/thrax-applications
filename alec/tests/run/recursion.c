#include <stdio.h>
int fib(int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }
long long fact(int n) { return n <= 1 ? 1 : n * fact(n - 1); }
int ack(int m, int n) {
	if (m == 0) return n + 1;
	if (n == 0) return ack(m - 1, 1);
	return ack(m - 1, ack(m, n - 1));
}
int is_even(int n);
int is_odd(int n) { return n == 0 ? 0 : is_even(n - 1); }
int is_even(int n) { return n == 0 ? 1 : is_odd(n - 1); }
void hanoi(int n, char a, char b, char c, int *moves) {
	if (n == 0) return;
	hanoi(n - 1, a, c, b, moves);
	(*moves)++;
	hanoi(n - 1, c, b, a, moves);
}
int main(void) {
	printf("%d %lld %d\n", fib(20), fact(20), ack(2, 3));
	printf("%d %d\n", is_even(10), is_odd(7));
	int moves = 0;
	hanoi(10, 'A', 'B', 'C', &moves);
	printf("%d\n", moves);
	return 0;
}
