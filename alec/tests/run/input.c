#include <stdio.h>
#include <string.h>
int main(void) {
	int n;
	scanf("%d", &n);
	int total = 0;
	for (int i = 0; i < n; i++) {
		int x;
		scanf("%d", &x);
		total += x;
	}
	char word[32];
	double d;
	scanf("%31s %lf", word, &d);
	printf("sum=%d word=%s d=%.2f\n", total, word, d);
	char line[64];
	fgets(line, sizeof line, stdin);
	while (fgets(line, sizeof line, stdin)) {
		line[strcspn(line, "\n")] = 0;
		printf("line: [%s]\n", line);
	}
	int c = getchar();
	printf("eof=%d\n", c == EOF);
	return 0;
}
