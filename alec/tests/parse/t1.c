typedef struct node { int val; struct node *next; } Node;
enum color { RED, GREEN = 5, BLUE };
static int counter = 3;
char *names[] = { "alpha", "beta" };
int table[2][3] = { {1, 2, 3}, [1] = {4, [2] = 6} };
struct pt { int x, y; unsigned flags : 3; };
struct pt origin = { .y = 2, .x = 1, .flags = 5 };
double half = 1.0 / 2;
int add(int a, int b) { return a + b; }
int sum(Node *n) {
	int s = 0;
	for (; n; n = n->next) s += n->val;
	return s;
}
int main(void) {
	Node b = { 2, 0 }, a = { 1, &b };
	int arr[] = { 1, 2, 3 };
	char msg[] = "hi";
	long x = sizeof arr / sizeof arr[0];
	unsigned char c = 300;
	switch (BLUE) { case 6: x++; break; default: x--; }
	x += add(sum(&a), arr[2]) * GREEN;
	return x > 0 ? (int)x : -1;
}
