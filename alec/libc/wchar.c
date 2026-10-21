/* Wide strings: wchar_t is a 32-bit code point; the multibyte encoding is
   UTF-8. */
#include <wchar.h>

size_t wcslen(const wchar_t *s) { size_t n = 0; while (s[n]) n++; return n; }
wchar_t *wcscpy(wchar_t *dst, const wchar_t *src) { wchar_t *d = dst; while ((*d++ = *src++)); return dst; }
wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n) {
	size_t i = 0;
	for (; i < n && src[i]; i++) dst[i] = src[i];
	for (; i < n; i++) dst[i] = 0;
	return dst;
}
wchar_t *wcscat(wchar_t *dst, const wchar_t *src) { wcscpy(dst + wcslen(dst), src); return dst; }
int wcscmp(const wchar_t *a, const wchar_t *b) {
	while (*a && *a == *b) a++, b++;
	return *a < *b ? -1 : *a > *b;
}
int wcsncmp(const wchar_t *a, const wchar_t *b, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
		if (!a[i]) return 0;
	}
	return 0;
}
wchar_t *wcschr(const wchar_t *s, wchar_t c) {
	for (;; s++) {
		if (*s == c) return (wchar_t *)s;
		if (!*s) return 0;
	}
}
wchar_t *wmemcpy(wchar_t *dst, const wchar_t *src, size_t n) { for (size_t i = 0; i < n; i++) dst[i] = src[i]; return dst; }
wchar_t *wmemset(wchar_t *dst, wchar_t c, size_t n) { for (size_t i = 0; i < n; i++) dst[i] = c; return dst; }

size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
	if (!s) return 0;
	const unsigned char *u = (const unsigned char *)s;
	if (n == 0) return (size_t)-2;
	int len = u[0] < 0x80 ? 1 : u[0] < 0xe0 ? 2 : u[0] < 0xf0 ? 3 : 4;
	if ((size_t)len > n) return (size_t)-2;
	wchar_t c = len == 1 ? u[0] : len == 2 ? u[0] & 0x1f : len == 3 ? u[0] & 0x0f : u[0] & 0x07;
	for (int i = 1; i < len; i++) c = (c << 6) | (u[i] & 0x3f);
	if (pwc) *pwc = c;
	return c ? (size_t)len : 0;
}

size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps) {
	unsigned c = wc;
	if (c < 0x80) { s[0] = c; return 1; }
	if (c < 0x800) { s[0] = 0xc0 | c >> 6; s[1] = 0x80 | (c & 0x3f); return 2; }
	if (c < 0x10000) { s[0] = 0xe0 | c >> 12; s[1] = 0x80 | (c >> 6 & 0x3f); s[2] = 0x80 | (c & 0x3f); return 3; }
	s[0] = 0xf0 | c >> 18; s[1] = 0x80 | (c >> 12 & 0x3f); s[2] = 0x80 | (c >> 6 & 0x3f); s[3] = 0x80 | (c & 0x3f);
	return 4;
}
