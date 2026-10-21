/* The host interface alec's C library is built on: functions with no
   definition become imports from the host's `env` module (see run.mjs), and
   the `__builtin_wasm_*` names compile to single wasm instructions. */
#ifndef _ALEC_SYS_H
#define _ALEC_SYS_H
#include <stddef.h>

long __alec_write(int fd, const void *buf, size_t len);
long __alec_read(int fd, void *buf, size_t len);
_Noreturn void __alec_exit(int code);
int __alec_argc(void);
size_t __alec_arglen(int i);
void __alec_argcopy(int i, char *dst);
int __alec_open(const char *path, const char *mode);
int __alec_close(int fd);
long long __alec_filesize(int fd);
long __alec_pread(int fd, void *buf, size_t len, long long off);
int __alec_remove(const char *path);
long long __alec_time(void);
long long __alec_clock(void);
int __alec_getenv_len(const char *name);
void __alec_getenv_copy(const char *name, char *dst);

size_t __builtin_wasm_memory_size(void);
size_t __builtin_wasm_memory_grow(size_t pages);
void __builtin_wasm_memory_copy(void *dst, const void *src, size_t n);
void __builtin_wasm_memory_fill(void *dst, int c, size_t n);

/* The first byte past the shadow stack; the heap grows up from here. */
extern char __heap_base[];

void __alec_flush_all(void);
#endif
