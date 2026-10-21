#include <stdio.h>
#include <math.h>
int main(void) {
	double d = 3.75;
	float f = 1.5f;
	printf("%f %f\n", d, f);
	printf("%.2f %.0f %.3f\n", d * 2, d, f / 3);
	printf("%e %E\n", 12345.678, 0.000123);
	printf("%g %g %g %g\n", 100000.0, 1000000.0, 0.0001, 0.00001);
	printf("%g %g\n", 3.14159265358979, 1.0 / 3);
	int i = (int)d;
	double back = i;
	printf("%d %f\n", i, back);
	printf("%d %d\n", (int)-3.9, (int)3.9);
	unsigned u = (unsigned)3000000000.0;
	printf("%u\n", u);
	printf("%f %f\n", sqrt(2.0), floor(-2.5));
	printf("%f %f %f\n", fabs(-4.25), ceil(1.01), round(2.5));
	printf("%.4f %.4f\n", sin(1.0), pow(2.0, 10.0));
	printf("%d %d\n", 1.5 > 1.25, 2.0 == 2);
	long long l = 9007199254740993LL;
	double ld = (double)l;
	printf("%.1f\n", ld);
	float g = 16777217;
	printf("%.1f\n", g);
	printf("%5.1f|%-8.3f|%+.2f\n", 3.14159, 2.5, 7.0);
	return 0;
}
