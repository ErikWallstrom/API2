#ifndef ARGPARSER_H
#define ARGPARSER_H

#include "vec.h"

#define ARGPARSER_NOSHORTOPT '\0'
#define ARGPARSER_NOLONGOPT NULL

struct ArgParserOpt
{
	Vec(const char*) args;
	const char* longopt;
	const char* info;
	size_t numargs;
	int used;
	char shortopt;
};

struct ArgParserOpt* argparseropt_ctor(
	struct ArgParserOpt* self,
	char shortopt,
	const char* longopt,
	const char* info,
	size_t numargs
);
int argparser_parse(
	const char* argv[], 
	int argc, 
	struct ArgParserOpt* options, 
	size_t numoptions
);

#endif
