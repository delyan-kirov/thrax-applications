#define str(s) # s
#define xstr(s) str(s)
#define debug(s, t) printf("x" # s "= %d, x" # t "= %s", \
 x ## s, x ## t)
#define INCFILE(n) vers ## n
#define glue(a, b) a ## b
#define xglue(a, b) glue(a, b)
#define HIGHLOW "hello"
#define LOW LOW ", world"
debug(1, 2);
fputs(str(strncmp("abc\0d", "abc", '\4') // this goes away
 == 0) str(: @\n), s);
xstr(INCFILE(2).h)
glue(HIGH, LOW);
xglue(HIGH, LOW)
#define hash_hash # ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)
char p[] = join(x, y);
#define t(x,y,z) x ## y ## z
int j[] = { t(1,2,3), t(,4,5), t(6,,7), t(8,9,),
 t(10,,), t(,11,), t(,,12), t(,,) };
#define OBJ_LIKE (1-1)
#define F(...) f(0 __VA_OPT__(,) __VA_ARGS__)
#define showlist(...) puts(#__VA_ARGS__)
#define report(test, ...) ((test)?puts(#test): printf(__VA_ARGS__))
F(a,b,c) F()
showlist(The first, second, and third items.);
report(x>y, "x is %d but y is %d", x, y);
#define e(fmt, ...) printf(fmt, ## __VA_ARGS__)
e("a") e("b", 1, 2)
#if defined(OBJ_LIKE) && OBJ_LIKE == 0 && (1 ? 2 : 1/0) && -1 < 0u == 0 && (1 << 4) == 16
ok __LINE__ __FILE__
#elif 1
bad
#else
bad
#endif
#ifdef nothing
#error should be skipped
#else
#line 100 "renamed.c"
line __LINE__ __FILE__
#endif
