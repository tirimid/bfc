#include "cg.h"

#include "cg_x86_64.h"

enum fn_ind {
	FN_IND_PRELUDE = 0,
	FN_IND_POSTLUDE,
	FN_IND_PTR_RIGHT,
	FN_IND_PTR_LEFT,
	FN_IND_INC,
	FN_IND_DEC,
	FN_IND_OUTPUT,
	FN_IND_INPUT,
	FN_IND_COND_BEGIN,
	FN_IND_COND_END,
	FN_IND_LAST__,
};

void
compile(FILE *in_fp, FILE *out_fp, enum target_arch arch, enum target_os os)
{
	void (*cg_fns[FN_IND_LAST__])(struct cg_state *);

	switch (arch) {
	case TARGET_ARCH_X86_64:
		cg_fns[FN_IND_PRELUDE] = cg_x86_64_prelude;
		cg_fns[FN_IND_POSTLUDE] = cg_x86_64_postlude;
		cg_fns[FN_IND_PTR_RIGHT] = cg_x86_64_ptr_right;
		cg_fns[FN_IND_PTR_LEFT] = cg_x86_64_ptr_left;
		cg_fns[FN_IND_INC] = cg_x86_64_inc;
		cg_fns[FN_IND_DEC] = cg_x86_64_dec;
		cg_fns[FN_IND_OUTPUT] = cg_x86_64_output;
		cg_fns[FN_IND_INPUT] = cg_x86_64_input;
		cg_fns[FN_IND_COND_BEGIN] = cg_x86_64_cond_begin;
		cg_fns[FN_IND_COND_END] = cg_x86_64_cond_end;
		break;
	}

	struct cg_state cgs = {
		.in_fp = in_fp,
		.out_fp = out_fp,
		.cur_label = 0,
		.os = os,
	};

	cg_fns[FN_IND_PRELUDE](&cgs);

	for (int c; (c = getc(cgs.in_fp)) != EOF;) {
		switch (c) {
		case '>':
			cg_fns[FN_IND_PTR_RIGHT](&cgs);
			break;
		case '<':
			cg_fns[FN_IND_PTR_LEFT](&cgs);
			break;
		case '+':
			cg_fns[FN_IND_INC](&cgs);
			break;
		case '-':
			cg_fns[FN_IND_DEC](&cgs);
			break;
		case '.':
			cg_fns[FN_IND_OUTPUT](&cgs);
			break;
		case ',':
			cg_fns[FN_IND_INPUT](&cgs);
			break;
		case '[':
			cg_fns[FN_IND_COND_BEGIN](&cgs);
			break;
		case ']':
			cg_fns[FN_IND_COND_END](&cgs);
			break;
		}
	}

	cg_fns[FN_IND_POSTLUDE](&cgs);
}
