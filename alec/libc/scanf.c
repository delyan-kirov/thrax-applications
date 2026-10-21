/* The scanf family: one scanner reading from a source, which is a stream or
   a string. */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

/* -- scanning ---------------------------------------------------------- */

typedef struct {
	FILE *f;
	const char *s;
	int count;
} Src;

static int get(Src *in) {
	int c;
	if (in->f)
		c = fgetc(in->f);
	else
		c = *in->s ? (unsigned char)*in->s++ : EOF;
	if (c != EOF)
		in->count++;
	return c;
}

static void unget(Src *in, int c) {
	if (c == EOF)
		return;
	in->count--;
	if (in->f)
		ungetc(c, in->f);
	else
		in->s--;
}

static int skip_ws(Src *in) {
	int c;
	while ((c = get(in)) != EOF && isspace(c))
		;
	unget(in, c);
	return c;
}

static int digit_val(int c) {
	if (isdigit(c)) return c - '0';
	if (isalpha(c)) return (c | 32) - 'a' + 10;
	return 99;
}

static int scan(Src *in, const char *fmt, va_list ap) {
	int assigned = 0;
	for (const char *p = fmt; *p; p++) {
		if (isspace(*p)) {
			skip_ws(in);
			continue;
		}
		if (*p != '%' || p[1] == '%') {
			if (*p == '%')
				p++;
			int c = get(in);
			if (c != *p) {
				unget(in, c);
				return assigned ? assigned : (c == EOF ? EOF : 0);
			}
			continue;
		}
		p++;
		int skip = 0, width = 0, len = 0;
		if (*p == '*') {
			skip = 1;
			p++;
		}
		while (isdigit(*p))
			width = width * 10 + (*p++ - '0');
		if (*p == 'h') {
			len = 2;
			if (*++p == 'h') {
				len = 1;
				p++;
			}
		} else if (*p == 'l') {
			len = 3;
			if (*++p == 'l') {
				len = 4;
				p++;
			}
		} else if (*p == 'z' || *p == 'j' || *p == 't' || *p == 'L') {
			len = *p == 'j' ? 4 : *p == 'L' ? 3 : 5;
			p++;
		}
		char conv = *p;
		if (!width)
			width = INT_MAX;
		if (conv == 'n') {
			if (!skip)
				*va_arg(ap, int *) = in->count;
			continue;
		}
		if (conv != 'c' && conv != '[' && skip_ws(in) == EOF)
			return assigned ? assigned : EOF;
		if (conv == 'd' || conv == 'i' || conv == 'u' || conv == 'x' || conv == 'X' || conv == 'o') {
			int base = conv == 'x' || conv == 'X' ? 16 : conv == 'o' ? 8 : conv == 'i' ? 0 : 10;
			int c = get(in), n = 0, neg = 0;
			if ((c == '-' || c == '+') && n < width) {
				neg = c == '-';
				c = get(in);
				n++;
			}
			if (base == 0) {
				base = 10;
				if (c == '0') {
					base = 8;
					int d = get(in);
					if (d == 'x' || d == 'X') {
						base = 16;
						c = get(in);
					} else {
						unget(in, d);
					}
				}
			} else if (base == 16 && c == '0') {
				int d = get(in);
				if (d == 'x' || d == 'X')
					c = get(in);
				else {
					unget(in, d);
				}
			}
			unsigned long long v = 0;
			int any = 0;
			while (n < width && c != EOF && digit_val(c) < base) {
				v = v * base + digit_val(c);
				any = 1;
				n++;
				c = get(in);
			}
			unget(in, c);
			if (!any)
				return assigned;
			if (neg)
				v = 0ULL - v;
			if (!skip) {
				void *dst = va_arg(ap, void *);
				if (len == 1) *(char *)dst = v;
				else if (len == 2) *(short *)dst = v;
				else if (len == 3 || len == 5) *(long *)dst = v;
				else if (len == 4) *(long long *)dst = v;
				else *(int *)dst = v;
				assigned++;
			}
		} else if (conv == 'f' || conv == 'e' || conv == 'g' || conv == 'E' || conv == 'G' || conv == 'a') {
			char buf[128];
			int n = 0, c = get(in);
			while (c != EOF && n < width && n < 127 &&
				(isdigit(c) || c == '.' || c == 'e' || c == 'E' || ((c == '-' || c == '+') && (n == 0 || buf[n - 1] == 'e' || buf[n - 1] == 'E')))) {
				buf[n++] = c;
				c = get(in);
			}
			unget(in, c);
			buf[n] = 0;
			if (!n)
				return assigned;
			double v = strtod(buf, 0);
			if (!skip) {
				if (len == 3) *va_arg(ap, double *) = v;
				else *va_arg(ap, float *) = v;
				assigned++;
			}
		} else if (conv == 's') {
			char *dst = skip ? 0 : va_arg(ap, char *);
			int c = get(in), n = 0;
			while (c != EOF && !isspace(c) && n < width) {
				if (dst)
					dst[n] = c;
				n++;
				c = get(in);
			}
			unget(in, c);
			if (dst) {
				dst[n] = 0;
				assigned++;
			}
		} else if (conv == 'c') {
			char *dst = skip ? 0 : va_arg(ap, char *);
			if (width == INT_MAX)
				width = 1;
			for (int i = 0; i < width; i++) {
				int c = get(in);
				if (c == EOF)
					return assigned ? assigned : EOF;
				if (dst)
					dst[i] = c;
			}
			if (dst)
				assigned++;
		} else if (conv == '[') {
			p++;
			int neg = 0;
			if (*p == '^') {
				neg = 1;
				p++;
			}
			const char *set = p;
			if (*p == ']')
				p++;
			while (*p && *p != ']')
				p++;
			int setlen = p - set;
			char *dst = skip ? 0 : va_arg(ap, char *);
			int c = get(in), n = 0;
			while (c != EOF && n < width) {
				int in_set = 0;
				for (int i = 0; i < setlen; i++) {
					if (i + 2 < setlen && set[i + 1] == '-') {
						if (c >= set[i] && c <= set[i + 2])
							in_set = 1;
						i += 2;
					} else if (c == set[i])
						in_set = 1;
				}
				if (in_set == neg)
					break;
				if (dst)
					dst[n] = c;
				n++;
				c = get(in);
			}
			unget(in, c);
			if (!n)
				return assigned;
			if (dst) {
				dst[n] = 0;
				assigned++;
			}
		}
	}
	return assigned;
}

int vsscanf(const char *s, const char *fmt, va_list ap) {
	Src in = { 0, s, 0 };
	return scan(&in, fmt, ap);
}

int vfscanf(FILE *f, const char *fmt, va_list ap) {
	Src in = { f, 0, 0 };
	return scan(&in, fmt, ap);
}

int sscanf(const char *s, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vsscanf(s, fmt, ap);
	va_end(ap);
	return r;
}

int fscanf(FILE *f, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vfscanf(f, fmt, ap);
	va_end(ap);
	return r;
}

int scanf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vfscanf(stdin, fmt, ap);
	va_end(ap);
	return r;
}
