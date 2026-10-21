/* The printf family: one formatter writing through a sink, which is a
   stream or a bounded buffer. */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/* -- formatting -------------------------------------------------------- */

typedef struct {
	FILE *f;
	char *buf;
	size_t cap, len;
} Sink;

static void emit(Sink *s, char c) {
	if (s->f)
		fputc(c, s->f);
	else if (s->len + 1 < s->cap)
		s->buf[s->len] = c;
	s->len++;
}

static void emit_n(Sink *s, const char *p, int n) {
	for (int i = 0; i < n; i++)
		emit(s, p[i]);
}

static void pad(Sink *s, char c, int n) {
	for (int i = 0; i < n; i++)
		emit(s, c);
}

typedef struct {
	int left, plus, space, alt, zero;
	int width, prec;
} Spec;

/* Write a field: sign/prefix, then digits, honouring width and the
   zero-padding flag. */
static void field(Sink *s, Spec *sp, const char *prefix, const char *body, int n) {
	int pl = strlen(prefix);
	int padn = sp->width > pl + n ? sp->width - pl - n : 0;
	if (!sp->left && !sp->zero)
		pad(s, ' ', padn);
	emit_n(s, prefix, pl);
	if (!sp->left && sp->zero)
		pad(s, '0', padn);
	emit_n(s, body, n);
	if (sp->left)
		pad(s, ' ', padn);
}

static int utoa(unsigned long long v, int base, int upper, char *out) {
	char tmp[72];
	int n = 0;
	const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
	do {
		tmp[n++] = digits[v % base];
		v /= base;
	} while (v);
	for (int i = 0; i < n; i++)
		out[i] = tmp[n - 1 - i];
	return n;
}

static void fmt_int(Sink *s, Spec *sp, unsigned long long v, int neg, int base, int upper, int is_signed) {
	char digits[80];
	int n = 0;
	if (!(sp->prec == 0 && v == 0))
		n = utoa(v, base, upper, digits);
	char body[160];
	int zeros = sp->prec > n ? sp->prec - n : 0;
	if (base == 8 && sp->alt && zeros == 0 && (n == 0 || digits[0] != '0'))
		zeros = 1;
	int bl = 0;
	for (int i = 0; i < zeros; i++)
		body[bl++] = '0';
	for (int i = 0; i < n; i++)
		body[bl++] = digits[i];
	const char *prefix = "";
	if (is_signed)
		prefix = neg ? "-" : sp->plus ? "+" : sp->space ? " " : "";
	else if (base == 16 && sp->alt && v)
		prefix = upper ? "0X" : "0x";
	if (sp->prec >= 0)
		sp->zero = 0;
	field(s, sp, prefix, body, bl);
}

static double p10(int n) {
	double r = 1;
	double b = 10;
	int neg = n < 0;
	if (neg)
		n = -n;
	while (n) {
		if (n & 1)
			r *= b;
		b *= b;
		n >>= 1;
	}
	return neg ? 1 / r : r;
}

/* The decimal digits of a nonnegative finite `v` rounded to `prec` places
   after the point, into `out` (no sign); returns the length. */
static int fixed_digits(double v, int prec, int alt, char *out) {
	int n = 0;
	if (v < 9.0e18 / p10(prec > 18 ? 18 : prec) && prec <= 18) {
		double scaled = v * p10(prec);
		unsigned long long all = (unsigned long long)scaled;
		double frac = scaled - (double)all;
		if (frac > 0.5 || (frac == 0.5 && (all & 1)))
			all++;
		unsigned long long scale = 1;
		for (int i = 0; i < prec; i++)
			scale *= 10;
		n = utoa(all / scale, 10, 0, out);
		if (prec > 0 || alt)
			out[n++] = '.';
		unsigned long long frac = all % scale;
		for (int i = prec - 1; i >= 0; i--) {
			out[n + i] = '0' + frac % 10;
			frac /= 10;
		}
		return n + prec;
	}
	/* Too large for 64-bit scaling: peel off decimal digits from the top. */
	int e = 0;
	while (p10(e + 1) <= v)
		e++;
	for (int i = e; i >= 0; i--) {
		double d = p10(i);
		int digit = (int)(v / d);
		if (digit > 9)
			digit = 9;
		out[n++] = '0' + digit;
		v -= digit * d;
	}
	if (prec > 0 || alt)
		out[n++] = '.';
	for (int i = 0; i < prec; i++) {
		v *= 10;
		int digit = (int)v;
		out[n++] = '0' + digit;
		v -= digit;
	}
	return n;
}

/* Mantissa digits and decimal exponent of a positive finite `v` rounded to
   `sig` significant digits. */
static int sci_digits(double v, int sig, char *out) {
	int e = 0;
	if (v != 0) {
		while (v >= p10(e + 1))
			e++;
		while (v < p10(e))
			e--;
	}
	if (sig > 17) {
		for (int i = 17; i < sig; i++)
			out[i] = '0';
	}
	int use = sig > 17 ? 17 : sig;
	double m = v / p10(e - use + 1);
	unsigned long long d = (unsigned long long)m;
	double frac = m - (double)d;
	if (frac > 0.5 || (frac == 0.5 && (d & 1)))
		d++;
	unsigned long long lim = 1;
	for (int i = 0; i < use; i++)
		lim *= 10;
	if (d >= lim) {
		d /= 10;
		e++;
	}
	for (int i = use - 1; i >= 0; i--) {
		out[i] = '0' + d % 10;
		d /= 10;
	}
	return e;
}

static int exp_suffix(char *out, int e, int upper) {
	int n = 0;
	out[n++] = upper ? 'E' : 'e';
	out[n++] = e < 0 ? '-' : '+';
	if (e < 0)
		e = -e;
	if (e < 10)
		out[n++] = '0';
	n += utoa(e, 10, 0, out + n);
	return n;
}

static void fmt_float(Sink *s, Spec *sp, double v, char conv) {
	int upper = conv == 'F' || conv == 'E' || conv == 'G';
	int neg = v < 0 || (v == 0 && 1 / v < 0);
	if (neg)
		v = -v;
	const char *prefix = neg ? "-" : sp->plus ? "+" : sp->space ? " " : "";
	char body[400];
	int n = 0;
	if (v != v || v - v != 0) {
		const char *w = v != v ? (upper ? "NAN" : "nan") : (upper ? "INF" : "inf");
		sp->zero = 0;
		field(s, sp, prefix, w, 3);
		return;
	}
	int prec = sp->prec < 0 ? 6 : sp->prec;
	char c = conv | 32;
	if (c == 'f') {
		n = fixed_digits(v, prec, sp->alt, body);
	} else if (c == 'e') {
		char dg[40];
		int e = sci_digits(v, prec + 1, dg);
		body[n++] = dg[0];
		if (prec > 0 || sp->alt)
			body[n++] = '.';
		for (int i = 1; i <= prec; i++)
			body[n++] = dg[i];
		n += exp_suffix(body + n, e, upper);
	} else {
		int P = prec == 0 ? 1 : prec;
		char dg[40];
		int x = v == 0 ? 0 : sci_digits(v, P, dg);
		if (P > x && x >= -4) {
			n = fixed_digits(v, P - 1 - x, sp->alt, body);
		} else {
			body[n++] = dg[0];
			if (P > 1 || sp->alt)
				body[n++] = '.';
			for (int i = 1; i < P; i++)
				body[n++] = dg[i];
			if (!sp->alt) {
				while (n > 0 && body[n - 1] == '0')
					n--;
				if (n > 0 && body[n - 1] == '.')
					n--;
			}
			n += exp_suffix(body + n, x, upper);
			field(s, sp, prefix, body, n);
			return;
		}
		if (!sp->alt) {
			int dot = 0;
			for (int i = 0; i < n; i++)
				if (body[i] == '.')
					dot = 1;
			if (dot) {
				while (body[n - 1] == '0')
					n--;
				if (body[n - 1] == '.')
					n--;
			}
		}
	}
	field(s, sp, prefix, body, n);
}

static int format(Sink *s, const char *fmt, va_list ap) {
	for (const char *p = fmt; *p; p++) {
		if (*p != '%') {
			emit(s, *p);
			continue;
		}
		p++;
		Spec sp = { 0, 0, 0, 0, 0, 0, -1 };
		for (;; p++) {
			if (*p == '-') sp.left = 1;
			else if (*p == '+') sp.plus = 1;
			else if (*p == ' ') sp.space = 1;
			else if (*p == '#') sp.alt = 1;
			else if (*p == '0') sp.zero = 1;
			else break;
		}
		if (*p == '*') {
			sp.width = va_arg(ap, int);
			if (sp.width < 0) {
				sp.left = 1;
				sp.width = -sp.width;
			}
			p++;
		} else
			while (isdigit(*p))
				sp.width = sp.width * 10 + (*p++ - '0');
		if (*p == '.') {
			p++;
			sp.prec = 0;
			if (*p == '*') {
				sp.prec = va_arg(ap, int);
				p++;
			} else
				while (isdigit(*p))
					sp.prec = sp.prec * 10 + (*p++ - '0');
		}
		if (sp.left)
			sp.zero = 0;
		int len = 0; /* 1 hh, 2 h, 3 l, 4 ll, 5 z/t */
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
		} else if (*p == 'j' || *p == 'q') {
			len = 4;
			p++;
		} else if (*p == 'z' || *p == 't') {
			len = 5;
			p++;
		} else if (*p == 'L') {
			p++;
		}
		char conv = *p;
		if (conv == 'd' || conv == 'i') {
			long long v;
			if (len == 4) v = va_arg(ap, long long);
			else if (len == 3 || len == 5) v = va_arg(ap, long);
			else v = va_arg(ap, int);
			if (len == 1) v = (signed char)v;
			else if (len == 2) v = (short)v;
			unsigned long long mag = v < 0 ? 0ULL - (unsigned long long)v : (unsigned long long)v;
			fmt_int(s, &sp, mag, v < 0, 10, 0, 1);
		} else if (conv == 'u' || conv == 'x' || conv == 'X' || conv == 'o' || conv == 'b') {
			unsigned long long v;
			if (len == 4) v = va_arg(ap, unsigned long long);
			else if (len == 3 || len == 5) v = va_arg(ap, unsigned long);
			else v = va_arg(ap, unsigned int);
			if (len == 1) v = (unsigned char)v;
			else if (len == 2) v = (unsigned short)v;
			int base = conv == 'u' ? 10 : conv == 'o' ? 8 : conv == 'b' ? 2 : 16;
			fmt_int(s, &sp, v, 0, base, conv == 'X', 0);
		} else if (conv == 'c') {
			char ch = va_arg(ap, int);
			sp.zero = 0;
			field(s, &sp, "", &ch, 1);
		} else if (conv == 's') {
			const char *str = va_arg(ap, const char *);
			if (!str)
				str = "(null)";
			int n = sp.prec >= 0 ? (int)strnlen(str, sp.prec) : (int)strlen(str);
			sp.zero = 0;
			field(s, &sp, "", str, n);
		} else if (conv == 'p') {
			void *ptr = va_arg(ap, void *);
			sp.alt = 1;
			fmt_int(s, &sp, (unsigned long long)(size_t)ptr, 0, 16, 0, 0);
		} else if (conv == 'f' || conv == 'F' || conv == 'e' || conv == 'E' || conv == 'g' || conv == 'G') {
			fmt_float(s, &sp, va_arg(ap, double), conv);
		} else if (conv == 'n') {
			int *np = va_arg(ap, int *);
			*np = s->len;
		} else if (conv == '%') {
			emit(s, '%');
		} else if (conv == 0) {
			break;
		} else {
			emit(s, '%');
			emit(s, conv);
		}
	}
	return s->len;
}

int vfprintf(FILE *f, const char *fmt, va_list ap) {
	Sink s = { f, 0, 0, 0 };
	return format(&s, fmt, ap);
}

int vprintf(const char *fmt, va_list ap) { return vfprintf(stdout, fmt, ap); }

int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap) {
	Sink s = { 0, buf, n, 0 };
	int r = format(&s, fmt, ap);
	if (n)
		buf[s.len < n ? s.len : n - 1] = 0;
	return r;
}

int vsprintf(char *buf, const char *fmt, va_list ap) { return vsnprintf(buf, (size_t)INT_MAX, fmt, ap); }

int printf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vfprintf(stdout, fmt, ap);
	va_end(ap);
	return r;
}

int fprintf(FILE *f, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vfprintf(f, fmt, ap);
	va_end(ap);
	return r;
}

int sprintf(char *buf, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vsprintf(buf, fmt, ap);
	va_end(ap);
	return r;
}

int snprintf(char *buf, size_t n, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vsnprintf(buf, n, fmt, ap);
	va_end(ap);
	return r;
}

