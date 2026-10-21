/* The POSIX file-descriptor calls, over the host's descriptors. */
#include <unistd.h>
#include <fcntl.h>
#include "sys.h"

int open(const char *path, int flags, ...) {
	const char *mode;
	int acc = flags & 3;
	if (flags & O_APPEND) mode = acc == O_RDWR ? "a+" : "a";
	else if (flags & O_TRUNC || (flags & O_CREAT && acc != O_RDONLY)) mode = acc == O_RDWR ? "w+" : "w";
	else mode = acc == O_RDONLY ? "r" : "r+";
	return __alec_open(path, mode);
}

int creat(const char *path, int mode) { return open(path, O_WRONLY | O_CREAT | O_TRUNC); }
ssize_t read(int fd, void *buf, size_t n) { return __alec_read(fd, buf, n); }
ssize_t write(int fd, const void *buf, size_t n) { return __alec_write(fd, buf, n); }
int close(int fd) { return __alec_close(fd); }
off_t lseek(int fd, off_t off, int whence) { return whence == 2 ? __alec_filesize(fd) + off : -1; }
int unlink(const char *path) { return __alec_remove(path); }
int isatty(int fd) { return fd < 3; }
int getpid(void) { return 1; }
