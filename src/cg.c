#include "cg.h"

#include "cg_x86_64.h"

void
assemble(FILE *in_fp, FILE *out_fp, enum target_arch arch, enum target_os os)
{
	void (*cg_fn_indep[4])(FILE *);
	void (*cg_fn_os_dep[4])(FILE *, enum target_os);
	void (*cg_fn_input_dep[2])(FILE *, FILE *);

	switch (arch) {
	case TARGET_ARCH_X86_64:
		cg_fn_indep[0] = cg_x86_64_ptr_right;
		cg_fn_indep[1] = cg_x86_64_ptr_left;
		cg_fn_indep[2] = cg_x86_64_inc;
		cg_fn_indep[3] = cg_x86_64_dec;
		cg_fn_os_dep[0] = cg_x86_64_prelude;
		cg_fn_os_dep[1] = cg_x86_64_postlude;
		cg_fn_os_dep[2] = cg_x86_64_output;
		cg_fn_os_dep[3] = cg_x86_64_input;
		cg_fn_input_dep[0] = cg_x86_64_cond_begin;
		cg_fn_input_dep[1] = cg_x86_64_cond_end;
		break;
	}

	cg_fn_os_dep[0](out_fp, os);

	for (int c; (c = getc(in_fp)) != EOF;) {
		switch (c) {
		case '>':
			cg_fn_indep[0](out_fp);
			break;
		case '<':
			cg_fn_indep[1](out_fp);
			break;
		case '+':
			cg_fn_indep[2](out_fp);
			break;
		case '-':
			cg_fn_indep[3](out_fp);
			break;
		case '.':
			cg_fn_os_dep[2](out_fp, os);
			break;
		case ',':
			cg_fn_os_dep[3](out_fp, os);
			break;
		case '[':
			cg_fn_input_dep[0](in_fp, out_fp);
			break;
		case ']':
			cg_fn_input_dep[1](in_fp, out_fp);
			break;
		}
	}

	cg_fn_os_dep[1](out_fp, os);
}
