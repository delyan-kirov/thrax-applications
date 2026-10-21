#ifndef _PTHREAD_H
#define _PTHREAD_H
/* WebAssembly here has one thread. `pthread_create` runs the thread's
   function to completion at once, which is one legal schedule; joining it
   then returns its result. Mutexes and condition variables are no-ops. */
typedef struct { void *result; } *pthread_t;
typedef int pthread_attr_t;
typedef int pthread_mutex_t;
typedef int pthread_mutexattr_t;
typedef int pthread_cond_t;
typedef int pthread_condattr_t;
typedef int pthread_once_t;
#define PTHREAD_MUTEX_INITIALIZER 0
#define PTHREAD_COND_INITIALIZER 0
#define PTHREAD_ONCE_INIT 0
int pthread_create(pthread_t *t, const pthread_attr_t *attr, void *(*fn)(void *), void *arg);
int pthread_join(pthread_t t, void **result);
int pthread_detach(pthread_t t);
pthread_t pthread_self(void);
int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a);
int pthread_mutex_lock(pthread_mutex_t *m);
int pthread_mutex_trylock(pthread_mutex_t *m);
int pthread_mutex_unlock(pthread_mutex_t *m);
int pthread_mutex_destroy(pthread_mutex_t *m);
int pthread_once(pthread_once_t *once, void (*fn)(void));
#endif
