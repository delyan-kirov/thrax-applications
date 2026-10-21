#include <stdio.h>
#include <stdarg.h>
int sum(int n, ...) {
	va_list ap;
	va_start(ap, n);
	int s = 0;
	for (int i = 0; i < n; i++) s += va_arg(ap, int);
	va_end(ap);
	return s;
}
double avg(int n, ...) {
	va_list ap;
	va_start(ap, n);
	double s = 0;
	for (int i = 0; i < n; i++) s += va_arg(ap, double);
	va_end(ap);
	return s / n;
}
void logf2(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}
int main(void) {
	printf("%d %d\n", sum(3, 1, 2, 3), sum(0));
	printf("%.2f\n", avg(4, 1.0, 2.0, 3.0, 4.5));
	logf2("%s=%d (%c) %lld\n", "x", 5, 'y', 1LL << 50);
	return 0;
}
