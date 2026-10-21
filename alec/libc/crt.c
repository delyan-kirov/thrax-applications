/* Program startup: collect the arguments from the host, run main, exit with
   its status. `__alec_main` is written by the linker; it calls main with the
   parameters main declares. */
#include <stdlib.h>
#include "sys.h"

int __alec_main(int argc, char **argv);

void _start(void) {
	int argc = __alec_argc();
	char **argv = malloc((argc + 1) * sizeof(char *));
	for (int i = 0; i < argc; i++) {
		size_t n = __alec_arglen(i);
		argv[i] = malloc(n + 1);
		__alec_argcopy(i, argv[i]);
	}
	argv[argc] = 0;
	exit(__alec_main(argc, argv));
}
