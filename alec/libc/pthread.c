#include <pthread.h>
#include <stdlib.h>

int pthread_create(pthread_t *t, const pthread_attr_t *attr, void *(*fn)(void *), void *arg) {
	pthread_t th = malloc(sizeof *th);
	th->result = fn(arg);
	*t = th;
	return 0;
}

int pthread_join(pthread_t t, void **result) {
	if (result)
		*result = t->result;
	free(t);
	return 0;
}

int pthread_detach(pthread_t t) { free(t); return 0; }
pthread_t pthread_self(void) { return 0; }
int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a) { *m = 0; return 0; }
int pthread_mutex_lock(pthread_mutex_t *m) { *m = 1; return 0; }
int pthread_mutex_trylock(pthread_mutex_t *m) { if (*m) return 16; *m = 1; return 0; }
int pthread_mutex_unlock(pthread_mutex_t *m) { *m = 0; return 0; }
int pthread_mutex_destroy(pthread_mutex_t *m) { return 0; }

int pthread_once(pthread_once_t *once, void (*fn)(void)) {
	if (!*once) {
		*once = 1;
		fn();
	}
	return 0;
}
