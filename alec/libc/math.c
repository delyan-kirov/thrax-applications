/* The libm functions wasm has instructions for, and those built from them.
   The transcendental ones (sin, exp, pow, ...) are imported from the host. */
#include <math.h>

double sqrt(double x) { return __builtin_sqrt(x); }
double fabs(double x) { return __builtin_fabs(x); }
double floor(double x) { return __builtin_floor(x); }
double ceil(double x) { return __builtin_ceil(x); }
double trunc(double x) { return __builtin_trunc(x); }
double rint(double x) { return __builtin_nearbyint(x); }
double nearbyint(double x) { return __builtin_nearbyint(x); }
double round(double x) { return x < 0 ? -__builtin_floor(-x + 0.5) : __builtin_floor(x + 0.5); }
long lround(double x) { return (long)round(x); }
long long llround(double x) { return (long long)round(x); }
long lrint(double x) { return (long)rint(x); }
double copysign(double x, double y) { return __builtin_copysign(x, y); }
double fmin(double x, double y) { return x != x ? y : y != y ? x : x < y ? x : y; }
double fmax(double x, double y) { return x != x ? y : y != y ? x : x > y ? x : y; }
double fdim(double x, double y) { return x > y ? x - y : 0; }
double fma(double x, double y, double z) { return x * y + z; }
double remainder(double x, double y) { return x - rint(x / y) * y; }
int __alec_signbit(double x) { return __builtin_copysign(1.0, x) < 0; }

double ldexp(double x, int e) {
	while (e > 0) {
		x *= 2;
		e--;
	}
	while (e < 0) {
		x /= 2;
		e++;
	}
	return x;
}

double scalbn(double x, int e) { return ldexp(x, e); }

double frexp(double x, int *e) {
	*e = 0;
	if (x == 0 || x != x || x - x != 0)
		return x;
	double a = fabs(x);
	while (a >= 1) {
		a /= 2;
		(*e)++;
	}
	while (a < 0.5) {
		a *= 2;
		(*e)--;
	}
	return x < 0 ? -a : a;
}

int ilogb(double x) {
	int e;
	frexp(x, &e);
	return e - 1;
}

double logb(double x) { return ilogb(x); }

double modf(double x, double *ip) {
	*ip = trunc(x);
	return x - *ip;
}

float sqrtf(float x) { return __builtin_sqrtf(x); }
float fabsf(float x) { return __builtin_fabsf(x); }
float floorf(float x) { return __builtin_floorf(x); }
float ceilf(float x) { return __builtin_ceilf(x); }
float truncf(float x) { return __builtin_truncf(x); }
float roundf(float x) { return (float)round(x); }
float fmodf(float x, float y) { return (float)fmod(x, y); }
float powf(float x, float y) { return (float)pow(x, y); }
float expf(float x) { return (float)exp(x); }
float logf(float x) { return (float)log(x); }
float log10f(float x) { return (float)log10(x); }
float log2f(float x) { return (float)log2(x); }
float sinf(float x) { return (float)sin(x); }
float cosf(float x) { return (float)cos(x); }
float tanf(float x) { return (float)tan(x); }
float atanf(float x) { return (float)atan(x); }
float atan2f(float y, float x) { return (float)atan2(y, x); }
float hypotf(float x, float y) { return (float)hypot(x, y); }
float fminf(float x, float y) { return (float)fmin(x, y); }
float fmaxf(float x, float y) { return (float)fmax(x, y); }
float copysignf(float x, float y) { return (float)copysign(x, y); }
