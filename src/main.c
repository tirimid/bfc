#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "cg.h"
#include "util.h"

// for ease of implementation, the code assumes that these will accept the
// usual GNU-style command line arguments, `-o` and such, and that they will
// return 0 on success.
#define ASSEMBLER "/usr/bin/as"
#define LINKER "/usr/bin/ld"

bool flag_b = false, flag_o = false;

static void usage(char const *name);

int
main(int argc, char const *argv[])
{
	int ch;
	while ((ch = getopt(argc, (char *const *)argv, "bho")) != -1) {
		switch (ch) {
		case 'b':
			flag_b = true;
			break;
		case 'h':
			usage(argv[0]);
			return 0;
		case 'o':
			flag_o = true;
			break;
		default:
			return 1;
		}
	}

	if (flag_b && !flag_o) {
		fputs("'-b' flag requires '-o'!\n", stderr);
		return 1;
	}
	
	int firstarg = 1;
	while (firstarg < argc
	       && *argv[firstarg] == '-'
	       && strcmp(argv[firstarg - 1], "--")) {
		++firstarg;
	}
	
	if (argc - firstarg != 2) {
		fprintf(stderr, "usage: %s [options] input output\n", argv[0]);
		return 1;
	}

	FILE *infp = fopen(argv[firstarg], "rb");
	if (!infp) {
		fputs("failed to open input file for reading!\n", stderr);
		return 1;
	}

	FILE *outfp = fopen(argv[firstarg + 1], "wb");
	if (!outfp) {
		fclose(infp);
		fputs("failed to open output file for writing!\n", stderr);
		return 1;
	}

	compile(infp, outfp, ARCH_X86_64, OS_LINUX);

	fclose(infp);
	fclose(outfp);

	char const *obj = tmpnam(NULL); // dangerous but i dont care.
	if (flag_o) {
		char cmd[4096];
		snprintf(cmd, 4096, ASSEMBLER " -o \"%s\" \"%s\"", obj, argv[firstarg + 1]);

		if (system(cmd)) {
			unlink(obj);
			fputs("failed to assemble generated code!\n", stderr);
			return 1;
		}
	}

	if (flag_b) {
		char cmd[4096];
		snprintf(cmd, 4096, LINKER " -o \"%s\" \"%s\"", argv[firstarg + 1], obj);

		if (system(cmd)) {
			unlink(obj);
			fputs("failed to link assembled object!\n", stderr);
			return 1;
		}
	} else if (flag_o) {
		if (cpfile(obj, argv[firstarg + 1])) {
			unlink(obj);
			fputs("failed to copy assembled object!\n", stderr);
			return 1;
		}
	}
	
	unlink(obj);
	
	return 0;
}

static void
usage(char const *name)
{
	printf("usage:\n"
	       "\t%s [options] input output\n"
	       "options:\n"
	       "\t-b  link assembled object (requires -o)\n"
	       "\t-h  display this menu\n"
	       "\t-o  assemble compiler output\n", name);
}
