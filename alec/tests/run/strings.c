#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
int main(void) {
	char s[] = "The quick brown fox";
	printf("%zu %zu\n", sizeof s, strlen(s));
	char *w = strchr(s, 'q');
	printf("%s|%s\n", w, strrchr(s, 'o'));
	printf("%s\n", strstr(s, "brown"));
	char copy[32];
	strncpy(copy, s, 9);
	copy[9] = 0;
	printf("[%s]\n", copy);
	for (char *p = copy; *p; p++) *p = toupper(*p);
	printf("%s\n", copy);
	printf("%d %d %d\n", strcmp("abc", "abd") < 0, strncmp("abcdef", "abcxyz", 3), memcmp("ab", "ab", 2));
	char buf[64];
	int n = sprintf(buf, "%s-%05d-%x-%c", "id", 42, 255, 'Q');
	printf("%s %d\n", buf, n);
	snprintf(buf, 6, "%s", "truncate me");
	printf("%s\n", buf);
	char tok[] = "a,b,,c";
	for (char *t = strtok(tok, ","); t; t = strtok(0, ",")) printf("<%s>", t);
	printf("\n");
	printf("%ld %ld %lu\n", strtol("  -123xyz", 0, 10), strtol("0x1F", 0, 0), strtoul("777", 0, 8));
	printf("%d %d\n", atoi("42"), atoi("-17abc"));
	printf("%.3f\n", atof("3.14159"));
	char *end;
	double d = strtod("2.5e3 rest", &end);
	printf("%.1f [%s]\n", d, end);
	printf("%zu %zu\n", strspn("aaabcd", "ab"), strcspn("hello world", " "));
	memset(buf, 'x', 5);
	buf[5] = 0;
	printf("%s\n", buf);
	memmove(buf + 1, buf, 3);
	printf("%s\n", buf);
	printf("%-6s|%6s|%.2s\n", "ab", "cd", "efgh");
	const char *wide = "caf\xc3\xa9";
	printf("%zu\n", strlen(wide));
	printf("%c%c%c\n", 'a' + 1, "xyz"[1], *"hi");
	return 0;
}
