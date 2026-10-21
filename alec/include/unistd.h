#ifndef _UNISTD_H
#define _UNISTD_H
#include <stddef.h>
typedef long ssize_t;
typedef long long off_t;
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
ssize_t read(int fd, void *buf, size_t n);
ssize_t write(int fd, const void *buf, size_t n);
int close(int fd);
off_t lseek(int fd, off_t off, int whence);
int unlink(const char *path);
int isatty(int fd);
int getpid(void);
#endif
