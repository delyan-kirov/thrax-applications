#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
int work(int fail_at) {
	int rc = -1;
	char *a = malloc(10);
	if (!a) goto out;
	if (fail_at == 1) goto free_a;
	char *b = malloc(20);
	if (fail_at == 2) goto free_b;
	rc = 0;
free_b:
	free(b);
free_a:
	free(a);
out:
	return rc;
}
void bye(void) { printf("bye\n"); }
void bye2(void) { printf("bye2\n"); }
int main(void) {
	atexit(bye);
	atexit(bye2);
	printf("%d %d %d\n", work(0), work(1), work(2));
	assert(1 + 1 == 2);
	fflush(stdout);
	exit(3);
}
