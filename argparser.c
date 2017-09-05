#include "argparser.h"
#include "log.h"

struct ArgParserOpt* argparseropt_ctor(
	struct ArgParserOpt* self,
	char shortopt,
	const char* longopt,
	const char* info,
	size_t numargs
)
{
	self->info = info;
	self->numargs = numargs;
	self->longopt = longopt;
	self->shortopt = shortopt;
	self->used = 0;
	self->args = vec_ctor(const char*, numargs);

	return self;
}

void argparser_printhelp(
	struct ArgParserOpt* options, 
	size_t numoptions,
	const char* progname
)
{
	log_assert(options, "is NULL");
	log_assert(numoptions > 0, "are you retarded? 0 options makes no sense");
	log_assert(progname, "is NULL");

	printf("Usage: %s [OPTIONS -", progname);
	for(size_t i = 0; i < numoptions; i++)
	{
		putchar(options[i].shortopt);
	}

	puts("]\nOptions:");
	for(size_t i = 0; i < numoptions; i++)
	{
		printf("    ");
		if(options[i].shortopt != '\0')
		{
			printf("-%c, ", options[i].shortopt);
		}
		else
		{
			printf("    ");
		}

		if(options[i].longopt)
		{
			printf("--%s", options[i].longopt);
		}

		if(options[i].info)
		{
			int gap = (options[i].longopt) ? 
				15 - strlen(options[i].longopt) : 15;

			for(int j = 0; j < gap; j++)
			{
				putchar(' ');
			}

			puts(options[i].info);
		}
		else
		{
			putchar('\n');
		}
	}

	putchar('\n');
}

int argparser_parse(
	const char* argv[], 
	int argc, 
	struct ArgParserOpt* options, 
	size_t numoptions
)
{
	(void)argv;
	(void)argc;
	(void)options;
	(void)numoptions;

	//TODO: Complete this. I am to tired to implement this function
	/*
	log_assert(argv, "is NULL");
	log_assert(options, "is NULL");
	log_assert(numoptions > 0, "are you retarded? 0 options makes no sense");

	int result = 1;
	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			if(argv[i][1] == '-')
			{
				if(argv[i][2] == '\0') //Next is argument
				{

				}
				else //Long option
				{

				}
			}
			else //Short option
			{
				for(size_t j = 0; i < numoptions; i++)
				{
					if(argv[i])
				}
			}
		}
		else //Normal argument
		{

		}
	}

	return result;
	*/
	return 0; //XXX
}

