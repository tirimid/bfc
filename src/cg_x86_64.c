#include "cg_x86_64.h"

#include <stddef.h>
#include <stdlib.h>

#include "util.h"

void (*cg_x86_64_fntab[10])(struct cg_state *) = {
	cg_x86_64_prelude,
	cg_x86_64_postlude,
	cg_x86_64_right,
	cg_x86_64_left,
	cg_x86_64_inc,
	cg_x86_64_dec,
	cg_x86_64_output,
	cg_x86_64_input,
	cg_x86_64_cond_begin,
	cg_x86_64_cond_end,
};

void
cg_x86_64_prelude(struct cg_state *cgs)
{
	char const *prelude_indep =
		"\t.set TAPE_SIZE, 4194304\n"
		"\t.set PTR_START, 2097152\n"
		"\t.section .data\n"
		"bf_err_ptr:\n"
		"\t.ascii \"[BF] data pointer left acceptable range of cells\\n\"\n"
		"\t.section .bss\n"
		"bf_input_buf:\n"
		"\t.skip 32\n"
		"\t.section .text\n"
		"error:\n"
		"\tcall print\n"
		"\tmov $1, %rdi\n"
		"\tjmp quit\n"
		".Lptr_err:\n"
		"\tmov $bf_err_ptr, %rdi\n"
		"\tmov $50, %rsi\n"
		"\tcall error\n"
		"bf_ptr_right:\n"
		"\tinc %r13\n"
		"\tmov %r12, %rax\n"
		"\tadd $TAPE_SIZE, %rax\n"
		"\tcmp %rax, %r13\n"
		"\tjg .Lptr_err\n"
		"\tret\n"
		"bf_ptr_left:\n"
		"\tdec %r13\n"
		"\tcmp %r12, %r13\n"
		"\tjl .Lptr_err\n"
		"\tret\n"
		"bf_output:\n"
		"\tmov %r13, %rdi\n"
		"\tmov $1, %rsi\n"
		"\tcall print\n"
		"\tret\n"
		"bf_input:\n"
		"\tcall read\n"
		"\tmovb bf_input_buf, %al\n"
		"\tmovb %al, (%r13)\n"
		"\tret\n";

	char const *prelude_os_dep;
	switch (cgs->os) {
	case TARGET_OS_LINUX:
		prelude_os_dep =
			"print:\n"
			"\tpush %rsi\n"
			"\tpush %rdi\n"
			"\tmov $1, %rax\n"
			"\tmov $0, %rdi\n"
			"\tpop %rsi\n"
			"\tpop %rdx\n"
			"\tsyscall\n"
			"\tret\n"
			"read:\n"
			"\tmov $0, %rax\n"
			"\tmov $0, %rdi\n"
			"\tmov $bf_input_buf, %rsi\n"
			"\tmov $32, %rdx\n"
			"\tsyscall\n"
			"\tret\n"
			"quit:\n"
			"\tmov $60, %rax\n"
			"\tsyscall\n";
		break;
	}

	char const *prelude_start;
	switch (cgs->os) {
	case TARGET_OS_LINUX:
		prelude_start =
			"\t.global _start\n"
			"_start:\n"
			"\tmov $12, %rax\n"
			"\tmov $0, %rdi\n"
			"\tsyscall\n"
			"\tmov %rax, %r12\n"
			"\tmov %rax, %rdi\n"
			"\tadd $TAPE_SIZE, %rdi\n"
			"\tmov $12, %rax\n"
			"\tsyscall\n"
			"\tmov %r12, %rdi\n"
			"\tmov $TAPE_SIZE, %rcx\n"
			"\txor %rax, %rax\n"
			"\trep stosb\n"
			"\tmov %r12, %r13\n"
			"\tadd $PTR_START, %r13\n";
		break;
	}

	fputs(prelude_indep, cgs->out_fp);
	fputs(prelude_os_dep, cgs->out_fp);
	fputs(prelude_start, cgs->out_fp);
}

void
cg_x86_64_postlude(struct cg_state *cgs)
{
	char const *postlude =
		"\tmov $0, %rdi\n"
		"\tjmp quit\n";

	fputs(postlude, cgs->out_fp);
}

void
cg_x86_64_right(struct cg_state *cgs)
{
	fputs("\tcall bf_ptr_right\n", cgs->out_fp);
}

void
cg_x86_64_left(struct cg_state *cgs)
{
	fputs("\tcall bf_ptr_left\n", cgs->out_fp);
}

void
cg_x86_64_inc(struct cg_state *cgs)
{
	unsigned inc_cnt = 1;
	
	while (fgetc(cgs->in_fp) == '+')
		++inc_cnt;

	fgetc_back(cgs->in_fp);

	char add[32];
	sprintf(add, "\taddb $0x%x, (%%r13)\n", inc_cnt);
	fputs(add, cgs->out_fp);
}

void
cg_x86_64_dec(struct cg_state *cgs)
{
	unsigned dec_cnt = 1;
	
	while (fgetc(cgs->in_fp) == '-')
		++dec_cnt;

	fgetc_back(cgs->in_fp);

	char sub[32];
	sprintf(sub, "\tsubb $0x%x, (%%r13)\n", dec_cnt);
	fputs(sub, cgs->out_fp);
}

void
cg_x86_64_output(struct cg_state *cgs)
{
	fputs("\tcall bf_output\n", cgs->out_fp);
}

void
cg_x86_64_input(struct cg_state *cgs)
{
	fputs("\tcall bf_input\n", cgs->out_fp);
}

void
cg_x86_64_cond_begin(struct cg_state *cgs)
{
	size_t reset_pt = ftell(cgs->in_fp);

	unsigned jmp_label = cgs->cur_label;
	size_t nests_rem = 1;
	for (int c; (c = fgetc(cgs->in_fp)) != EOF && nests_rem;) {
		jmp_label += c == '[' || c == ']';
		nests_rem += c == '[';
		nests_rem -= c == ']';
	}

	fseek(cgs->in_fp, reset_pt, SEEK_SET);

	if (nests_rem) {
		size_t line, col;
		file_pos(cgs->in_fp, &line, &col);
		fprintf(stderr, "%zu:%zu - unmatched conditional [-bracket!\n", line, col);
		exit(-1);
	}

	fputs("\tcmpb $0, (%r13)\n", cgs->out_fp);

	char jmp[32] = {0};
	sprintf(jmp, "\tje .Lce_%x\n", jmp_label);
	fputs(jmp, cgs->out_fp);

	char label[32] = {0};
	sprintf(label, ".Lcb_%x:\n", cgs->cur_label++);
	fputs(label, cgs->out_fp);
}

void
cg_x86_64_cond_end(struct cg_state *cgs)
{
	size_t reset_pt = ftell(cgs->in_fp);

	unsigned jmp_label = cgs->cur_label;
	size_t nests_rem = 1;
	for (int c; (c = fgetc_back(cgs->in_fp)) != EOF && nests_rem;) {
		jmp_label -= c == '[' || c == ']';
		nests_rem -= c == '[';
		nests_rem += c == ']';
	}

	fseek(cgs->in_fp, reset_pt, SEEK_SET);

	if (nests_rem) {
		size_t line, col;
		file_pos(cgs->in_fp, &line, &col);
		fprintf(stderr, "%zu:%zu - unmatched conditional ]-bracket!\n", line, col);
		exit(-1);
	}

	fputs("\tcmpb $0, (%r13)\n", cgs->out_fp);

	char jmp[32] = {0};
	sprintf(jmp, "\tjne .Lcb_%x\n", jmp_label);
	fputs(jmp, cgs->out_fp);

	char label[32] = {0};
	sprintf(label, ".Lce_%x:\n", cgs->cur_label++);
	fputs(label, cgs->out_fp);
}
