/* Buffered streams over the host's file descriptors, and the printf and
   scanf families. A stream buffers in one direction at a time; stdout is
   line-buffered, stderr unbuffered. Formatting writes through a sink, so
   printf, fprintf and snprintf share one implementation. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "sys.h"

struct __alec_file {
	int fd;
	int mode;       /* _IOFBF, _IOLBF or _IONBF */
	int eof, err;
	int ungot;      /* a pushed-back byte, or -1 */
	int reading;    /* the buffer holds input, not output */
	long long pos;  /* the file offset of buf[0] when reading */
	unsigned char *buf;
	int len, at;
	int open;
};

static unsigned char stdout_buf[BUFSIZ];
static unsigned char stdin_buf[BUFSIZ];
static struct __alec_file files[FOPEN_MAX] = {
	{ 0, _IOFBF, 0, 0, -1, 1, 0, stdin_buf, 0, 0, 1 },
	{ 1, _IOLBF, 0, 0, -1, 0, 0, stdout_buf, 0, 0, 1 },
	{ 2, _IONBF, 0, 0, -1, 0, 0, 0, 0, 0, 1 },
};

FILE *stdin = &files[0];
FILE *stdout = &files[1];
FILE *stderr = &files[2];

static int flush_out(FILE *f) {
	int at = 0;
	while (at < f->len) {
		long n = __alec_write(f->fd, f->buf + at, f->len - at);
		if (n <= 0) {
			f->err = 1;
			f->len = 0;
			return EOF;
		}
		at += n;
	}
	f->len = 0;
	return 0;
}

int fflush(FILE *f) {
	if (!f) {
		__alec_flush_all();
		return 0;
	}
	if (f->reading) {
		f->pos += f->at;
		f->len = f->at = 0;
		f->ungot = -1;
		return 0;
	}
	return flush_out(f);
}

void __alec_flush_all(void) {
	for (int i = 0; i < FOPEN_MAX; i++)
		if (files[i].open && !files[i].reading)
			flush_out(&files[i]);
}

static void to_writing(FILE *f) {
	if (f->reading) {
		f->pos += f->at;
		f->len = f->at = 0;
		f->ungot = -1;
		f->reading = 0;
	}
}

int fputc(int c, FILE *f) {
	unsigned char b = c;
	to_writing(f);
	if (f->mode == _IONBF || !f->buf) {
		if (__alec_write(f->fd, &b, 1) != 1) {
			f->err = 1;
			return EOF;
		}
		return b;
	}
	if (f->len == BUFSIZ && flush_out(f))
		return EOF;
	f->buf[f->len++] = b;
	if (f->mode == _IOLBF && b == '\n' && flush_out(f))
		return EOF;
	return b;
}

int putc(int c, FILE *f) { return fputc(c, f); }
int putchar(int c) { return fputc(c, stdout); }

static int put_n(FILE *f, const char *s, size_t n) {
	to_writing(f);
	if (f->mode == _IONBF || !f->buf) {
		while (n) {
			long w = __alec_write(f->fd, s, n);
			if (w <= 0) {
				f->err = 1;
				return EOF;
			}
			s += w;
			n -= w;
		}
		return 0;
	}
	for (size_t i = 0; i < n; i++)
		if (fputc(s[i], f) == EOF)
			return EOF;
	return 0;
}

int fputs(const char *s, FILE *f) { return put_n(f, s, strlen(s)) == EOF ? EOF : 1; }

int puts(const char *s) {
	if (fputs(s, stdout) == EOF)
		return EOF;
	return fputc('\n', stdout) == EOF ? EOF : 1;
}

size_t fwrite(const void *p, size_t size, size_t n, FILE *f) {
	if (!size || !n)
		return 0;
	return put_n(f, p, size * n) == EOF ? 0 : n;
}

static int refill(FILE *f) {
	if (!f->reading) {
		flush_out(f);
		f->reading = 1;
		f->len = f->at = 0;
	}
	if (f == stdin)
		fflush(stdout);
	f->pos += f->len;
	long n = f->fd == 0 ? __alec_read(0, f->buf, BUFSIZ) : __alec_pread(f->fd, f->buf, BUFSIZ, f->pos);
	f->at = 0;
	if (n <= 0) {
		f->len = 0;
		if (n < 0)
			f->err = 1;
		else
			f->eof = 1;
		return EOF;
	}
	f->len = n;
	return 0;
}

int fgetc(FILE *f) {
	if (f->ungot >= 0) {
		int c = f->ungot;
		f->ungot = -1;
		return c;
	}
	if (!f->reading || f->at >= f->len)
		if (refill(f) == EOF)
			return EOF;
	return f->buf[f->at++];
}

int getc(FILE *f) { return fgetc(f); }
int getchar(void) { return fgetc(stdin); }

int ungetc(int c, FILE *f) {
	if (c == EOF)
		return EOF;
	f->ungot = (unsigned char)c;
	f->eof = 0;
	return f->ungot;
}

char *fgets(char *s, int n, FILE *f) {
	int i = 0;
	while (i < n - 1) {
		int c = fgetc(f);
		if (c == EOF)
			break;
		s[i++] = c;
		if (c == '\n')
			break;
	}
	if (i == 0)
		return 0;
	s[i] = 0;
	return s;
}

size_t fread(void *p, size_t size, size_t n, FILE *f) {
	unsigned char *d = p;
	size_t total = size * n, i = 0;
	for (; i < total; i++) {
		int c = fgetc(f);
		if (c == EOF)
			break;
		d[i] = c;
	}
	return size ? i / size : 0;
}

FILE *fopen(const char *path, const char *mode) {
	int fd = __alec_open(path, mode);
	if (fd < 0)
		return 0;
	for (int i = 3; i < FOPEN_MAX; i++) {
		if (!files[i].open) {
			FILE *f = &files[i];
			unsigned char *buf = malloc(BUFSIZ);
			*f = (struct __alec_file){ fd, _IOFBF, 0, 0, -1, mode[0] == 'r', 0, buf, 0, 0, 1 };
			if (mode[0] == 'a')
				f->pos = __alec_filesize(fd);
			return f;
		}
	}
	__alec_close(fd);
	return 0;
}

FILE *freopen(const char *path, const char *mode, FILE *f) {
	fclose(f);
	return fopen(path, mode);
}

int fclose(FILE *f) {
	int r = fflush(f);
	if (f->fd > 2) {
		__alec_close(f->fd);
		free(f->buf);
		f->open = 0;
	}
	return r;
}

int feof(FILE *f) { return f->eof; }
int ferror(FILE *f) { return f->err; }
void clearerr(FILE *f) { f->eof = f->err = 0; }

long ftell(FILE *f) {
	if (f->reading)
		return f->pos + f->at - (f->ungot >= 0);
	return f->pos + f->len;
}

int fseek(FILE *f, long off, int whence) {
	long long base = whence == SEEK_SET ? 0 : whence == SEEK_CUR ? ftell(f) : __alec_filesize(f->fd);
	fflush(f);
	f->pos = base + off;
	f->len = f->at = 0;
	f->ungot = -1;
	f->eof = 0;
	return 0;
}

void rewind(FILE *f) {
	fseek(f, 0, SEEK_SET);
	f->err = 0;
}

int fgetpos(FILE *f, fpos_t *pos) {
	*pos = ftell(f);
	return 0;
}

int fsetpos(FILE *f, const fpos_t *pos) { return fseek(f, *pos, SEEK_SET); }

int setvbuf(FILE *f, char *buf, int mode, size_t size) {
	f->mode = mode;
	return 0;
}

void setbuf(FILE *f, char *buf) { setvbuf(f, buf, buf ? _IOFBF : _IONBF, BUFSIZ); }

int remove(const char *path) { return __alec_remove(path); }
int rename(const char *from, const char *to) { return -1; }
FILE *tmpfile(void) { return 0; }

void perror(const char *s) {
	extern int errno;
	if (s && *s) {
		fputs(s, stderr);
		fputs(": ", stderr);
	}
	fputs(strerror(errno), stderr);
	fputc('\n', stderr);
}

