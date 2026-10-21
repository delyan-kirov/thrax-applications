#include <stdio.h>
#include <string.h>
int main(void) {
	const char *path = "tests/out/files_test.txt";
	FILE *f = fopen(path, "w");
	if (!f) { printf("cannot open\n"); return 1; }
	for (int i = 1; i <= 5; i++) fprintf(f, "line %d: %d\n", i, i * i);
	fputs("end\n", f);
	fclose(f);
	f = fopen(path, "r");
	char buf[64];
	int lines = 0;
	while (fgets(buf, sizeof buf, f)) { lines++; if (lines == 3) printf("third: %s", buf); }
	printf("%d lines\n", lines);
	fseek(f, 0, SEEK_SET);
	int c, count = 0;
	while ((c = fgetc(f)) != EOF) if (c == ':') count++;
	printf("%d colons, at %ld\n", count, ftell(f));
	fclose(f);
	f = fopen(path, "a");
	fputs("appended\n", f);
	fclose(f);
	f = fopen(path, "r");
	fseek(f, -9, SEEK_END);
	fgets(buf, sizeof buf, f);
	printf("tail: %s", buf);
	fclose(f);
	printf("%d\n", remove(path));
	printf("%d\n", fopen("tests/out/does/not/exist", "r") == NULL);
	return 0;
}
