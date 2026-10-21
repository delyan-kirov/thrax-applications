#ifndef _MATH_H
#define _MATH_H
#define M_E 2.7182818284590452354
#define M_LOG2E 1.4426950408889634074
#define M_LOG10E 0.43429448190325182765
#define M_LN2 0.69314718055994530942
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.78539816339744830962
#define M_1_PI 0.31830988618379067154
#define M_2_PI 0.63661977236758134308
#define M_SQRT2 1.41421356237309504880
#define M_SQRT1_2 0.70710678118654752440
#define HUGE_VAL (1.0 / 0.0)
#define HUGE_VALF (1.0f / 0.0f)
#define INFINITY (1.0f / 0.0f)
#define NAN (0.0f / 0.0f)
#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4
#define isnan(x) ((x) != (x))
#define isinf(x) (!isnan(x) && isnan((x) - (x)))
#define isfinite(x) (!isnan((x) - (x)))
#define isnormal(x) (isfinite(x) && (x) != 0)
#define signbit(x) __alec_signbit((double)(x))
#define fpclassify(x) (isnan(x) ? FP_NAN : isinf(x) ? FP_INFINITE : (x) == 0 ? FP_ZERO : FP_NORMAL)
#define isgreater(a, b) ((a) > (b))
#define isless(a, b) ((a) < (b))

double __builtin_sqrt(double);
float __builtin_sqrtf(float);
double __builtin_fabs(double);
float __builtin_fabsf(float);
double __builtin_floor(double);
float __builtin_floorf(float);
double __builtin_ceil(double);
float __builtin_ceilf(float);
double __builtin_trunc(double);
float __builtin_truncf(float);
double __builtin_nearbyint(double);
double __builtin_copysign(double, double);
double __builtin_fmin(double, double);
double __builtin_fmax(double, double);
int __alec_signbit(double x);

double sqrt(double x);
double fabs(double x);
double floor(double x);
double ceil(double x);
double trunc(double x);
double round(double x);
double rint(double x);
double nearbyint(double x);
long lround(double x);
long long llround(double x);
long lrint(double x);
double fmod(double x, double y);
double remainder(double x, double y);
double copysign(double x, double y);
double fmin(double x, double y);
double fmax(double x, double y);
double fdim(double x, double y);
double fma(double x, double y, double z);
double ldexp(double x, int e);
double scalbn(double x, int e);
double frexp(double x, int *e);
double modf(double x, double *ip);
int ilogb(double x);
double logb(double x);
double pow(double x, double y);
double exp(double x);
double exp2(double x);
double expm1(double x);
double log(double x);
double log10(double x);
double log2(double x);
double log1p(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);
double sinh(double x);
double cosh(double x);
double tanh(double x);
double asinh(double x);
double acosh(double x);
double atanh(double x);
double cbrt(double x);
double hypot(double x, double y);
float sqrtf(float x);
float fabsf(float x);
float floorf(float x);
float ceilf(float x);
float truncf(float x);
float roundf(float x);
float fmodf(float x, float y);
float powf(float x, float y);
float expf(float x);
float logf(float x);
float log10f(float x);
float log2f(float x);
float sinf(float x);
float cosf(float x);
float tanf(float x);
float atanf(float x);
float atan2f(float y, float x);
float hypotf(float x, float y);
float fminf(float x, float y);
float fmaxf(float x, float y);
float copysignf(float x, float y);
#endif
