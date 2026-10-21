#ifndef _STDATOMIC_H
#define _STDATOMIC_H
/* A wasm module here runs on one thread, so every atomic operation is the
   plain operation it names, and every memory order is trivially satisfied. */
#include <stddef.h>
#include <stdint.h>
typedef enum { memory_order_relaxed, memory_order_consume, memory_order_acquire,
	memory_order_release, memory_order_acq_rel, memory_order_seq_cst } memory_order;
typedef _Atomic _Bool atomic_bool;
typedef _Atomic char atomic_char;
typedef _Atomic int atomic_int;
typedef _Atomic unsigned atomic_uint;
typedef _Atomic long atomic_long;
typedef _Atomic unsigned long atomic_ulong;
typedef _Atomic long long atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;
typedef _Atomic size_t atomic_size_t;
typedef _Atomic intptr_t atomic_intptr_t;
typedef struct { _Bool v; } atomic_flag;
#define ATOMIC_FLAG_INIT { 0 }
#define ATOMIC_VAR_INIT(v) (v)
#define atomic_init(p, v) (*(p) = (v))
#define atomic_thread_fence(o) ((void)0)
#define atomic_signal_fence(o) ((void)0)
#define atomic_is_lock_free(p) 1
#define atomic_store(p, v) (*(p) = (v))
#define atomic_store_explicit(p, v, o) (*(p) = (v))
#define atomic_load(p) (*(p))
#define atomic_load_explicit(p, o) (*(p))
#define atomic_exchange(p, v) ({ __typeof__(*(p)) __old = *(p); *(p) = (v); __old; })
#define atomic_exchange_explicit(p, v, o) atomic_exchange(p, v)
#define atomic_compare_exchange_strong(p, e, v) \
	({ int __ok = *(p) == *(e); if (__ok) *(p) = (v); else *(e) = *(p); __ok; })
#define atomic_compare_exchange_weak(p, e, v) atomic_compare_exchange_strong(p, e, v)
#define atomic_compare_exchange_strong_explicit(p, e, v, s, f) atomic_compare_exchange_strong(p, e, v)
#define atomic_compare_exchange_weak_explicit(p, e, v, s, f) atomic_compare_exchange_strong(p, e, v)
#define atomic_fetch_add(p, v) ({ __typeof__(*(p)) __old = *(p); *(p) += (v); __old; })
#define atomic_fetch_sub(p, v) ({ __typeof__(*(p)) __old = *(p); *(p) -= (v); __old; })
#define atomic_fetch_or(p, v) ({ __typeof__(*(p)) __old = *(p); *(p) |= (v); __old; })
#define atomic_fetch_xor(p, v) ({ __typeof__(*(p)) __old = *(p); *(p) ^= (v); __old; })
#define atomic_fetch_and(p, v) ({ __typeof__(*(p)) __old = *(p); *(p) &= (v); __old; })
#define atomic_fetch_add_explicit(p, v, o) atomic_fetch_add(p, v)
#define atomic_fetch_sub_explicit(p, v, o) atomic_fetch_sub(p, v)
#define atomic_flag_test_and_set(f) ({ _Bool __old = (f)->v; (f)->v = 1; __old; })
#define atomic_flag_clear(f) ((f)->v = 0)
#endif
