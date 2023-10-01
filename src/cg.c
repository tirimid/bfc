#include "cg.h"

#include <string.h>

#include "cg_x86_64.h"

void
compile(FILE *in_fp, FILE *out_fp, enum target_arch arch, enum target_os os)
{
	void (*cg_fntab[FNTAB_ENT_LAST__])(struct cg_state *);
	switch (arch) {
	case TARGET_ARCH_X86_64:
		memcpy(cg_fntab, cg_x86_64_fntab, sizeof(cg_fntab));
		break;
	}

	struct cg_state cgs = {
		.in_fp = in_fp,
		.out_fp = out_fp,
		.cur_label = 0,
		.os = os,
	};

	cg_fntab[FNTAB_ENT_PRELUDE](&cgs);

	for (int c; (c = getc(cgs.in_fp)) != EOF;) {
		switch (c) {
		case '>':
			cg_fntab[FNTAB_ENT_RIGHT](&cgs);
			break;
		case '<':
			cg_fntab[FNTAB_ENT_LEFT](&cgs);
			break;
		case '+':
			cg_fntab[FNTAB_ENT_INC](&cgs);
			break;
		case '-':
			cg_fntab[FNTAB_ENT_DEC](&cgs);
			break;
		case '.':
			cg_fntab[FNTAB_ENT_OUTPUT](&cgs);
			break;
		case ',':
			cg_fntab[FNTAB_ENT_INPUT](&cgs);
			break;
		case '[':
			cg_fntab[FNTAB_ENT_COND_BEGIN](&cgs);
			break;
		case ']':
			cg_fntab[FNTAB_ENT_COND_END](&cgs);
			break;
		default:
			break;
		}
	}

	cg_fntab[FNTAB_ENT_POSTLUDE](&cgs);
}
