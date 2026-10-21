#include <stdio.h>
#include <string.h>
typedef struct { int a[8]; char tag[12]; } big;
typedef struct { double x, y; } vec;
big make(int seed) {
	big b;
	for (int i = 0; i < 8; i++) b.a[i] = seed * i;
	strcpy(b.tag, "made");
	return b;
}
int total(big b) { int s = 0; for (int i = 0; i < 8; i++) s += b.a[i]; b.a[0] = 999; return s; }
vec add(vec p, vec q) { return (vec){ p.x + q.x, p.y + q.y }; }
vec scale(vec v, double k) { v.x *= k; v.y *= k; return v; }
vec (*vop)(vec, vec) = add;
struct pair { char c; long long n; };
struct pair swap_pair(struct pair p) { struct pair r = { (char)p.n, p.c }; return r; }
int main(void) {
	big b = make(3);
	printf("%d %s %d\n", total(b), b.tag, b.a[0]);
	vec v = add((vec){ 1, 2 }, (vec){ 3, 4.5 });
	printf("%.1f %.1f\n", v.x, v.y);
	vec w = scale(vop(v, v), 0.5);
	printf("%.1f %.1f\n", w.x, w.y);
	printf("%.1f\n", add(v, w).y);
	struct pair p = swap_pair((struct pair){ 'A', 66 });
	printf("%c %lld\n", p.c, p.n);
	big arr[2] = { make(1), make(2) };
	arr[0] = arr[1];
	printf("%d\n", arr[0].a[7]);
	return 0;
}
