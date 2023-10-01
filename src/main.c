#include <stdio.h>

#include "cg.h"

int
main(int argc, char const *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s <input file> <output file>\n", argv[0]);
		return 1;
	}

	FILE *in_fp = fopen(argv[1], "rb");
	if (!in_fp) {
		fputs("failed to open input file for reading!\n", stderr);
		return 1;
	}

	FILE *out_fp = fopen(argv[2], "wb");
	if (!out_fp) {
		fclose(in_fp);
		fputs("failed to open output file for writing!\n", stderr);
		return 1;
	}

	compile(in_fp, out_fp, TARGET_ARCH_X86_64, TARGET_OS_LINUX);

	fclose(in_fp);
	fclose(out_fp);
	
	return 0;
}
