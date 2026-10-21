#include <stdio.h>
#include <string.h>
struct point { int x, y; };
struct rect { struct point a, b; char name[8]; };
union num { int i; float f; unsigned char bytes[4]; };
typedef struct node { int value; struct node *next; } node;

struct point mid(struct point p, struct point q) {
	struct point r = { (p.x + q.x) / 2, (p.y + q.y) / 2 };
	return r;
}

int area(const struct rect *r) { return (r->b.x - r->a.x) * (r->b.y - r->a.y); }

int sum(node *n) { int s = 0; for (; n; n = n->next) s += n->value; return s; }

struct flags { unsigned a : 3; unsigned b : 5; signed c : 4; unsigned d : 1; };

int main(void) {
	struct rect r = { { 1, 2 }, { 11, 7 }, "box" };
	printf("%d %s\n", area(&r), r.name);
	struct point m = mid(r.a, r.b);
	printf("%d %d\n", m.x, m.y);
	struct rect copy = r;
	copy.a.x = 100;
	printf("%d %d\n", r.a.x, copy.a.x);
	union num n;
	n.f = 1.0f;
	printf("%x %d\n", n.i, n.bytes[3]);
	node c = { 3, 0 }, b = { 2, &c }, a = { 1, &b };
	printf("%d\n", sum(&a));
	struct flags fl = { 5, 17, -3, 1 };
	printf("%u %u %d %u\n", fl.a, fl.b, fl.c, fl.d);
	fl.b += 20;
	fl.c = 7;
	printf("%u %d %zu\n", fl.b, fl.c, sizeof fl);
	struct point pts[3] = { [2] = { 5, 6 }, [0].y = 9 };
	printf("%d %d %d %d\n", pts[0].x, pts[0].y, pts[2].x, pts[2].y);
	printf("%zu %zu\n", sizeof(struct point), sizeof(union num));
	struct { int k; char c; } anon = { 42, 'z' };
	printf("%d %c\n", anon.k, anon.c);
	struct point *pp = &pts[2];
	pp->x *= 10;
	printf("%d\n", pts[2].x);
	return 0;
}
