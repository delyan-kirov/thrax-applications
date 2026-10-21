#ifndef _STDARG_H
#define _STDARG_H
/* Variadic arguments arrive as a pointer to an area of 8-byte slots. */
typedef char *va_list;
typedef char *__gnuc_va_list;
#define va_start(ap, ...) __builtin_va_start(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
#endif
