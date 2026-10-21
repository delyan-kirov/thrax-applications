#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <inttypes.h>
#include "sys.h"

#define MAX_ATEXIT 32
static void (*exit_fns[MAX_ATEXIT])(void);
static int n_exit;

int atexit(void (*fn)(void)) {
	if (n_exit == MAX_ATEXIT)
		return -1;
	exit_fns[n_exit++] = fn;
	return 0;
}

void exit(int status) {
	while (n_exit > 0)
		exit_fns[--n_exit]();
	__alec_flush_all();
	__alec_exit(status);
}

void _Exit(int status) {
	__alec_exit(status);
}

void abort(void) {
	__alec_flush_all();
	__alec_exit(134);
}

int abs(int x) { return x < 0 ? -x : x; }
long labs(long x) { return x < 0 ? -x : x; }
long long llabs(long long x) { return x < 0 ? -x : x; }

div_t div(int a, int b) { div_t r = { a / b, a % b }; return r; }
ldiv_t ldiv(long a, long b) { ldiv_t r = { a / b, a % b }; return r; }
lldiv_t lldiv(long long a, long long b) { lldiv_t r = { a / b, a % b }; return r; }

/* The digits of an integer in `base` (0 picks 8, 10 or 16 by prefix), as an
   unsigned magnitude that saturates at `limit`; `neg` reports a minus sign. */
static unsigned long long parse_uint(const char *s, char **end, int base, int *neg, unsigned long long limit, int *over) {
	const char *p = s;
	while (isspace(*p))
		p++;
	*neg = 0;
	if (*p == '+' || *p == '-')
		*neg = *p++ == '-';
	if ((base == 0 || base == 16) && p[0] == '0' && (p[1] | 32) == 'x' && isxdigit(p[2])) {
		p += 2;
		base = 16;
	} else if (base == 0)
		base = *p == '0' ? 8 : 10;
	unsigned long long v = 0;
	int any = 0;
	*over = 0;
	for (;; p++) {
		int d = isdigit(*p) ? *p - '0' : isalpha(*p) ? (*p | 32) - 'a' + 10 : 99;
		if (d >= base)
			break;
		any = 1;
		if (v > (limit - d) / base)
			*over = 1;
		else
			v = v * base + d;
	}
	if (end)
		*end = (char *)(any ? p : s);
	return v;
}

long long strtoll(const char *s, char **end, int base) {
	int neg, over;
	unsigned long long v = parse_uint(s, end, base, &neg, (unsigned long long)LLONG_MAX + 1, &over);
	if (over || (!neg && v > LLONG_MAX)) {
		errno = ERANGE;
		return neg ? LLONG_MIN : LLONG_MAX;
	}
	return neg ? (long long)(0ULL - v) : (long long)v;
}

unsigned long long strtoull(const char *s, char **end, int base) {
	int neg, over;
	unsigned long long v = parse_uint(s, end, base, &neg, ULLONG_MAX, &over);
	if (over) {
		errno = ERANGE;
		return ULLONG_MAX;
	}
	return neg ? 0ULL - v : v;
}

long strtol(const char *s, char **end, int base) {
	long long v = strtoll(s, end, base);
	if (v > LONG_MAX) {
		errno = ERANGE;
		return LONG_MAX;
	}
	if (v < LONG_MIN) {
		errno = ERANGE;
		return LONG_MIN;
	}
	return v;
}

unsigned long strtoul(const char *s, char **end, int base) {
	unsigned long long v = strtoull(s, end, base);
	if (v > ULONG_MAX && v < 0ULL - (unsigned long long)ULONG_MAX) {
		errno = ERANGE;
		return ULONG_MAX;
	}
	return v;
}

intmax_t strtoimax(const char *s, char **end, int base) { return strtoll(s, end, base); }
uintmax_t strtoumax(const char *s, char **end, int base) { return strtoull(s, end, base); }

int atoi(const char *s) { return (int)strtol(s, 0, 10); }
long atol(const char *s) { return strtol(s, 0, 10); }
long long atoll(const char *s) { return strtoll(s, 0, 10); }

static double scale10(double v, int e) {
	double b = 10, r = 1;
	int neg = e < 0;
	if (neg)
		e = -e;
	while (e) {
		if (e & 1)
			r *= b;
		b *= b;
		e >>= 1;
	}
	return neg ? v / r : v * r;
}

double strtod(const char *s, char **end) {
	const char *p = s;
	while (isspace(*p))
		p++;
	int neg = 0;
	if (*p == '+' || *p == '-')
		neg = *p++ == '-';
	double v = 0;
	int any = 0;
	if ((p[0] | 32) == 'i' && (p[1] | 32) == 'n' && (p[2] | 32) == 'f') {
		p += 3;
		if ((p[0] | 32) == 'i' && (p[1] | 32) == 'n' && (p[2] | 32) == 'i' && (p[3] | 32) == 't' && (p[4] | 32) == 'y')
			p += 5;
		if (end)
			*end = (char *)p;
		return neg ? -1.0 / 0.0 : 1.0 / 0.0;
	}
	if ((p[0] | 32) == 'n' && (p[1] | 32) == 'a' && (p[2] | 32) == 'n') {
		if (end)
			*end = (char *)(p + 3);
		return 0.0 / 0.0;
	}
	if (p[0] == '0' && (p[1] | 32) == 'x') {
		p += 2;
		int e = 0;
		for (; isxdigit(*p); p++, any = 1)
			v = v * 16 + (isdigit(*p) ? *p - '0' : (*p | 32) - 'a' + 10);
		if (*p == '.')
			for (p++; isxdigit(*p); p++, any = 1, e -= 4)
				v = v * 16 + (isdigit(*p) ? *p - '0' : (*p | 32) - 'a' + 10);
		if ((*p | 32) == 'p') {
			int en = 0, eneg = 0;
			p++;
			if (*p == '+' || *p == '-')
				eneg = *p++ == '-';
			while (isdigit(*p))
				en = en * 10 + (*p++ - '0');
			e += eneg ? -en : en;
		}
		while (e > 0) {
			v *= 2;
			e--;
		}
		while (e < 0) {
			v /= 2;
			e++;
		}
	} else {
		/* Up to 19 significant digits are gathered exactly as an integer,
		   then scaled by the decimal exponent once. */
		unsigned long long mant = 0;
		int digits = 0, e = 0;
		for (; isdigit(*p); p++, any = 1) {
			if (digits < 19) {
				mant = mant * 10 + (*p - '0');
				if (mant)
					digits++;
			} else
				e++;
		}
		if (*p == '.') {
			for (p++; isdigit(*p); p++, any = 1) {
				if (digits < 19) {
					mant = mant * 10 + (*p - '0');
					if (mant)
						digits++;
					e--;
				}
			}
		}
		if (any && (*p | 32) == 'e') {
			const char *q = p + 1;
			int en = 0, eneg = 0;
			if (*q == '+' || *q == '-')
				eneg = *q++ == '-';
			if (isdigit(*q)) {
				while (isdigit(*q))
					en = en * 10 + (*q++ - '0');
				e += eneg ? -en : en;
				p = q;
			}
		}
		v = (double)mant;
		if (e < -300) {
			v = scale10(v, -300);
			e += 300;
		}
		v = scale10(v, e);
	}
	if (end)
		*end = (char *)(any ? p : s);
	return neg ? -v : v;
}

float strtof(const char *s, char **end) { return (float)strtod(s, end); }
long double strtold(const char *s, char **end) { return strtod(s, end); }
double atof(const char *s) { return strtod(s, 0); }

static void swap_bytes(char *a, char *b, size_t n) {
	for (size_t i = 0; i < n; i++) {
		char t = a[i];
		a[i] = b[i];
		b[i] = t;
	}
}

/* Quicksort on the middle element, insertion sort for short runs. */
static void sort(char *base, size_t n, size_t size, int (*cmp)(const void *, const void *)) {
	while (n > 8) {
		swap_bytes(base, base + (n / 2) * size, size);
		size_t last = 0;
		for (size_t i = 1; i < n; i++)
			if (cmp(base + i * size, base) < 0)
				swap_bytes(base + ++last * size, base + i * size, size);
		swap_bytes(base, base + last * size, size);
		if (last < n - last) {
			sort(base, last, size, cmp);
			base += (last + 1) * size;
			n -= last + 1;
		} else {
			sort(base + (last + 1) * size, n - last - 1, size, cmp);
			n = last;
		}
	}
	for (size_t i = 1; i < n; i++)
		for (size_t j = i; j > 0 && cmp(base + (j - 1) * size, base + j * size) > 0; j--)
			swap_bytes(base + (j - 1) * size, base + j * size, size);
}

void qsort(void *base, size_t n, size_t size, int (*cmp)(const void *, const void *)) {
	sort(base, n, size, cmp);
}

void *bsearch(const void *key, const void *base, size_t n, size_t size, int (*cmp)(const void *, const void *)) {
	const char *b = base;
	size_t lo = 0, hi = n;
	while (lo < hi) {
		size_t mid = lo + (hi - lo) / 2;
		int c = cmp(key, b + mid * size);
		if (c == 0)
			return (void *)(b + mid * size);
		if (c < 0)
			hi = mid;
		else
			lo = mid + 1;
	}
	return 0;
}

static unsigned long long rand_state = 1;

int rand(void) {
	rand_state = rand_state * 6364136223846793005ULL + 1442695040888963407ULL;
	return (int)(rand_state >> 33);
}

void srand(unsigned seed) { rand_state = seed; }

char *getenv(const char *name) {
	int n = __alec_getenv_len(name);
	if (n < 0)
		return 0;
	char *v = malloc(n + 1);
	__alec_getenv_copy(name, v);
	return v;
}

int system(const char *cmd) { return -1; }
