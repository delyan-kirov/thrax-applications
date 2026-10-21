#include <string.h>
#include <stdlib.h>
#include "sys.h"

void *memcpy(void *dst, const void *src, size_t n) {
	__builtin_wasm_memory_copy(dst, src, n);
	return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
	__builtin_wasm_memory_copy(dst, src, n);
	return dst;
}

void *memset(void *dst, int c, size_t n) {
	__builtin_wasm_memory_fill(dst, c, n);
	return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
	const unsigned char *x = a, *y = b;
	for (size_t i = 0; i < n; i++)
		if (x[i] != y[i])
			return x[i] - y[i];
	return 0;
}

void *memchr(const void *s, int c, size_t n) {
	const unsigned char *p = s;
	for (size_t i = 0; i < n; i++)
		if (p[i] == (unsigned char)c)
			return (void *)(p + i);
	return 0;
}

size_t strlen(const char *s) {
	const char *p = s;
	while (*p)
		p++;
	return p - s;
}

size_t strnlen(const char *s, size_t n) {
	size_t i = 0;
	while (i < n && s[i])
		i++;
	return i;
}

char *strcpy(char *dst, const char *src) {
	char *d = dst;
	while ((*d++ = *src++))
		;
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
	size_t i = 0;
	for (; i < n && src[i]; i++)
		dst[i] = src[i];
	for (; i < n; i++)
		dst[i] = 0;
	return dst;
}

char *strcat(char *dst, const char *src) {
	strcpy(dst + strlen(dst), src);
	return dst;
}

char *strncat(char *dst, const char *src, size_t n) {
	char *d = dst + strlen(dst);
	size_t i = 0;
	for (; i < n && src[i]; i++)
		d[i] = src[i];
	d[i] = 0;
	return dst;
}

int strcmp(const char *a, const char *b) {
	while (*a && *a == *b)
		a++, b++;
	return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (a[i] != b[i] || !a[i])
			return (unsigned char)a[i] - (unsigned char)b[i];
	}
	return 0;
}

int strcoll(const char *a, const char *b) {
	return strcmp(a, b);
}

size_t strxfrm(char *dst, const char *src, size_t n) {
	size_t len = strlen(src);
	if (n)
		strncpy(dst, src, n);
	return len;
}

char *strchr(const char *s, int c) {
	for (;; s++) {
		if (*s == (char)c)
			return (char *)s;
		if (!*s)
			return 0;
	}
}

char *strrchr(const char *s, int c) {
	const char *hit = 0;
	for (;; s++) {
		if (*s == (char)c)
			hit = s;
		if (!*s)
			return (char *)hit;
	}
}

char *strstr(const char *s, const char *needle) {
	size_t n = strlen(needle);
	for (; *s; s++)
		if (!strncmp(s, needle, n))
			return (char *)s;
	return n ? 0 : (char *)s;
}

size_t strspn(const char *s, const char *set) {
	size_t i = 0;
	while (s[i] && strchr(set, s[i]))
		i++;
	return i;
}

size_t strcspn(const char *s, const char *set) {
	size_t i = 0;
	while (s[i] && !strchr(set, s[i]))
		i++;
	return i;
}

char *strpbrk(const char *s, const char *set) {
	s += strcspn(s, set);
	return *s ? (char *)s : 0;
}

char *strtok(char *s, const char *sep) {
	static char *next;
	if (!s)
		s = next;
	if (!s)
		return 0;
	s += strspn(s, sep);
	if (!*s) {
		next = 0;
		return 0;
	}
	char *end = s + strcspn(s, sep);
	if (*end)
		*end++ = 0;
	else
		end = 0;
	next = end;
	return s;
}

char *strdup(const char *s) {
	size_t n = strlen(s) + 1;
	char *p = malloc(n);
	if (p)
		memcpy(p, s, n);
	return p;
}

char *strndup(const char *s, size_t n) {
	size_t len = strnlen(s, n);
	char *p = malloc(len + 1);
	if (p) {
		memcpy(p, s, len);
		p[len] = 0;
	}
	return p;
}

char *strerror(int err) {
	switch (err) {
	case 0: return "Success";
	case 1: return "Operation not permitted";
	case 2: return "No such file or directory";
	case 5: return "Input/output error";
	case 9: return "Bad file descriptor";
	case 12: return "Cannot allocate memory";
	case 13: return "Permission denied";
	case 22: return "Invalid argument";
	case 33: return "Numerical argument out of domain";
	case 34: return "Numerical result out of range";
	default: return "Unknown error";
	}
}
