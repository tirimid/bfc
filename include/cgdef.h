#ifndef CGDEF_H__
#define CGDEF_H__

#include <stdio.h>

enum arch {
	ARCH_X86_64,
};

enum os {
	OS_LINUX,
};

enum fntabent {
	FTE_PRELUDE = 0,
	FTE_POSTLUDE,
	FTE_RIGHT,
	FTE_LEFT,
	FTE_INC,
	FTE_DEC,
	FTE_OUTPUT,
	FTE_INPUT,
	FTE_CONDBEGIN,
	FTE_CONDEND,
};

struct cgstate {
	FILE *infp, *outfp;
	unsigned curlabel;
	enum os os;
};

#endif
