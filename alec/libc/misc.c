/* errno, assert, time, signal and locale: small enough to share a file. */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <locale.h>
#include "sys.h"

int errno;

void __assert_fail(const char *expr, const char *file, int line, const char *func) {
	fprintf(stderr, "%s:%d: %s: Assertion `%s' failed.\n", file, line, func, expr);
	abort();
}

time_t time(time_t *t) {
	time_t now = __alec_time();
	if (t)
		*t = now;
	return now;
}

clock_t clock(void) { return __alec_clock(); }

double difftime(time_t a, time_t b) { return (double)(a - b); }

static struct tm tm_buf;

/* Days to a civil date (Howard Hinnant's algorithm). */
struct tm *gmtime(const time_t *t) {
	long long s = *t;
	long long days = s / 86400, rem = s % 86400;
	if (rem < 0) {
		rem += 86400;
		days--;
	}
	tm_buf.tm_hour = rem / 3600;
	tm_buf.tm_min = rem % 3600 / 60;
	tm_buf.tm_sec = rem % 60;
	tm_buf.tm_wday = (int)((days % 7 + 11) % 7);
	long long z = days + 719468;
	long long era = (z >= 0 ? z : z - 146096) / 146097;
	long long doe = z - era * 146097;
	long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	long long y = yoe + era * 400;
	long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
	long long mp = (5 * doy + 2) / 153;
	long long d = doy - (153 * mp + 2) / 5 + 1;
	long long m = mp < 10 ? mp + 3 : mp - 9;
	tm_buf.tm_year = (int)(y + (m <= 2) - 1900);
	tm_buf.tm_mon = (int)(m - 1);
	tm_buf.tm_mday = (int)d;
	tm_buf.tm_isdst = 0;
	return &tm_buf;
}

struct tm *localtime(const time_t *t) { return gmtime(t); }

time_t mktime(struct tm *tm) {
	long long y = tm->tm_year + 1900, m = tm->tm_mon + 1;
	y -= m <= 2;
	long long era = (y >= 0 ? y : y - 399) / 400;
	long long yoe = y - era * 400;
	long long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + tm->tm_mday - 1;
	long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	long long days = era * 146097 + doe - 719468;
	return days * 86400 + tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
}

static const char *wday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const char *mon[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

char *asctime(const struct tm *tm) {
	static char buf[32];
	snprintf(buf, sizeof buf, "%s %s %2d %02d:%02d:%02d %d\n", wday[tm->tm_wday], mon[tm->tm_mon],
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_year + 1900);
	return buf;
}

char *ctime(const time_t *t) { return asctime(localtime(t)); }

size_t strftime(char *s, size_t n, const char *fmt, const struct tm *tm) {
	size_t len = 0;
	for (; *fmt && len + 1 < n; fmt++) {
		if (*fmt != '%') {
			s[len++] = *fmt;
			continue;
		}
		char tmp[32];
		fmt++;
		switch (*fmt) {
		case 'Y': snprintf(tmp, sizeof tmp, "%d", tm->tm_year + 1900); break;
		case 'm': snprintf(tmp, sizeof tmp, "%02d", tm->tm_mon + 1); break;
		case 'd': snprintf(tmp, sizeof tmp, "%02d", tm->tm_mday); break;
		case 'H': snprintf(tmp, sizeof tmp, "%02d", tm->tm_hour); break;
		case 'M': snprintf(tmp, sizeof tmp, "%02d", tm->tm_min); break;
		case 'S': snprintf(tmp, sizeof tmp, "%02d", tm->tm_sec); break;
		case 'a': snprintf(tmp, sizeof tmp, "%s", wday[tm->tm_wday]); break;
		case 'b': snprintf(tmp, sizeof tmp, "%s", mon[tm->tm_mon]); break;
		case '%': snprintf(tmp, sizeof tmp, "%%"); break;
		default: snprintf(tmp, sizeof tmp, "%%%c", *fmt); break;
		}
		for (char *p = tmp; *p && len + 1 < n; p++)
			s[len++] = *p;
	}
	s[len] = 0;
	return len;
}

__sighandler_t signal(int sig, __sighandler_t handler) { return SIG_DFL; }

int raise(int sig) {
	if (sig == SIGABRT)
		abort();
	return 0;
}

char *setlocale(int category, const char *locale) { return "C"; }

struct lconv *localeconv(void) {
	static struct lconv c = { ".", "", "" };
	return &c;
}
