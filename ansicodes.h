#ifndef ANSICODES_H
#define ANSICODES_H

#include <stdarg.h>

struct ANSICodeFg 
{
	const char* name;
};

struct ANSICodeBg
{
	const char* name;
};

struct ANSICode
{
	struct ANSICodeFg* fg;
	struct ANSICodeBg* bg;
	int bold;
	int italic;
	int underline;
};

extern struct ANSICodeFg* ANSICODE_FG_BLACK;
extern struct ANSICodeFg* ANSICODE_FG_RED;
extern struct ANSICodeFg* ANSICODE_FG_GREEN;
extern struct ANSICodeFg* ANSICODE_FG_YELLOW;
extern struct ANSICodeFg* ANSICODE_FG_BLUE;
extern struct ANSICodeFg* ANSICODE_FG_MAGENTA;
extern struct ANSICodeFg* ANSICODE_FG_CYAN;
extern struct ANSICodeFg* ANSICODE_FG_WHITE;
extern struct ANSICodeFg* ANSICODE_FG_DEFAULT;

extern struct ANSICodeBg* ANSICODE_BG_BLACK;
extern struct ANSICodeBg* ANSICODE_BG_RED;
extern struct ANSICodeBg* ANSICODE_BG_GREEN;
extern struct ANSICodeBg* ANSICODE_BG_YELLOW;
extern struct ANSICodeBg* ANSICODE_BG_BLUE;
extern struct ANSICodeBg* ANSICODE_BG_MAGENTA;
extern struct ANSICodeBg* ANSICODE_BG_CYAN;
extern struct ANSICodeBg* ANSICODE_BG_WHITE;
extern struct ANSICodeBg* ANSICODE_BG_DEFAULT;

void ansicode_printf(struct ANSICode* self, const char* fmt, ...)
	__attribute__((format(printf, 2, 3)));
void ansicode_vprintf(struct ANSICode* self, const char* fmt, va_list args)
	__attribute__((format(printf, 2, 0)));

#endif
