#include "cg.h"

#include <string.h>

#include "cg_x86_64.h"

void
compile(FILE *infp, FILE *outfp, enum arch arch, enum os os)
{
	void (**cg_fntab)(struct cgstate *);
	switch (arch) {
	case ARCH_X86_64:
		cg_fntab = cg_x86_64_fntab;
		break;
	}

	struct cgstate cgs = {
		.infp = infp,
		.outfp = outfp,
		.curlabel = 0,
		.os = os,
	};

	cg_fntab[FTE_PRELUDE](&cgs);

	for (int c; (c = getc(cgs.infp)) != EOF;) {
		switch (c) {
		case '>':
			cg_fntab[FTE_RIGHT](&cgs);
			break;
		case '<':
			cg_fntab[FTE_LEFT](&cgs);
			break;
		case '+':
			cg_fntab[FTE_INC](&cgs);
			break;
		case '-':
			cg_fntab[FTE_DEC](&cgs);
			break;
		case '.':
			cg_fntab[FTE_OUTPUT](&cgs);
			break;
		case ',':
			cg_fntab[FTE_INPUT](&cgs);
			break;
		case '[':
			cg_fntab[FTE_CONDBEGIN](&cgs);
			break;
		case ']':
			cg_fntab[FTE_CONDEND](&cgs);
			break;
		default:
			break;
		}
	}

	cg_fntab[FTE_POSTLUDE](&cgs);
}
