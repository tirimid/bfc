#include <stdio.h>

#include "cg.h"

int
main(int argc, char const *argv[])
{
	if (argc != 3) {
		printf("usage: %s <input file> <output file>\n", argv[0]);
		return -1;
	}

	FILE *in_fp = fopen(argv[1], "rb");
	if (!in_fp) {
		printf("failed to open input file for reading!\n");
		return -1;
	}

	FILE *out_fp = fopen(argv[2], "wb");
	if (!out_fp) {
		fclose(in_fp);
		printf("failed to open output file for writing!\n");
		return -1;
	}

	compile(in_fp, out_fp, TARGET_ARCH_X86_64, TARGET_OS_LINUX);

	fclose(in_fp);
	fclose(out_fp);
	
	return 0;
}
