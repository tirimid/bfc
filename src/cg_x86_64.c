#include "cg_x86_64.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

static unsigned cur_label = 0;

void
cg_x86_64_prelude(FILE *out_fp, enum target_os os)
{
	char const *prelude =
		"\t.set TAPE_SIZE, 4194304\n"
		"\t.set PTR_START, 2097152\n"
		"\t.section .data\n"
		"bf_err_ptr:\n"
		"\t.ascii \"[BF] data pointer left acceptable range of cells\\n\"\n"
		"\t.section .text\n"
		"print:\n"
		"\tpush %rsi\n"
		"\tpush %rdi\n"
		"\tmov $1, %rax\n"
		"\tmov $0, %rdi\n"
		"\tpop %rsi\n"
		"\tpop %rdx\n"
		"\tsyscall\n"
		"\tret\n"
		"error:\n"
		"\tcall print\n"
		"\tmov $60, %rax\n"
		"\tmov $1, %rdi\n"
		"\tsyscall\n"
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
		"\tret\n"
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
		"\tmov %r12, %r13\n"
		"\tadd $PTR_START, %r13\n"
		"\txor %r14, %r14\n";

	fputs(prelude, out_fp);
}

void
cg_x86_64_postlude(FILE *out_fp, enum target_os os)
{
	char const *postlude =
		"\tmov $60, %rax\n"
		"\tmov $0, %rdi\n"
		"\tsyscall\n";

	fputs(postlude, out_fp);
}

void
cg_x86_64_ptr_right(FILE *out_fp)
{
	fputs("\tcall bf_ptr_right\n", out_fp);
}

void
cg_x86_64_ptr_left(FILE *out_fp)
{
	fputs("\tcall bf_ptr_left\n", out_fp);
}

void
cg_x86_64_inc(FILE *out_fp)
{
	fputs("\tincb (%r13)\n", out_fp);
}

void
cg_x86_64_dec(FILE *out_fp)
{
	fputs("\tdecb (%r13)\n", out_fp);
}

void
cg_x86_64_output(FILE *out_fp, enum target_os os)
{
	fputs("\tcall bf_output\n", out_fp);
}

void
cg_x86_64_input(FILE *out_fp, enum target_os os)
{
	fputs("\tcall bf_input\n", out_fp);
}

void
cg_x86_64_cond_begin(FILE *in_fp, FILE *out_fp)
{
	size_t reset_pt = ftell(in_fp);

	unsigned jmp_label = cur_label;
	size_t nests_rem = 1;
	for (int c; (c = fgetc(in_fp)) != EOF && nests_rem;) {
		jmp_label += c == '[' || c == ']';
		nests_rem += c == '[';
		nests_rem -= c == ']';
	}

	if (nests_rem) {
		printf("unmatched conditional [-bracket!\n");
		exit(-1);
	}
	
	fseek(in_fp, reset_pt, SEEK_SET);

	fputs("\tcmpb $0, (%r13)\n", out_fp);

	char jmp[32] = {0};
	sprintf(jmp, "\tje .Lce_%x\n", jmp_label);
	fputs(jmp, out_fp);

	char label[32] = {0};
	sprintf(label, ".Lcb_%x:\n", cur_label++);
	fputs(label, out_fp);
}

static int
fgetc_back(FILE *fp)
{
	if (ftell(fp) == 0)
		return EOF;

	uint8_t c;
	fseek(fp, -2, SEEK_CUR);
	fread(&c, 1, 1, fp);
	fseek(fp, -1, SEEK_CUR);
	return c;
}

void
cg_x86_64_cond_end(FILE *in_fp, FILE *out_fp)
{
	size_t reset_pt = ftell(in_fp);

	unsigned jmp_label = cur_label;
	size_t nests_rem = 1;
	for (int c; (c = fgetc_back(in_fp)) != EOF && nests_rem;) {
		jmp_label -= c == '[' || c == ']';
		nests_rem -= c == '[';
		nests_rem += c == ']';
	}

	if (nests_rem) {
		printf("unmatched conditional ]-bracket!\n");
		exit(-1);
	}
	
	fseek(in_fp, reset_pt, SEEK_SET);

	fputs("\tcmpb $0, (%r13)\n", out_fp);

	char jmp[32] = {0};
	sprintf(jmp, "\tjne .Lcb_%x\n", jmp_label);
	fputs(jmp, out_fp);

	char label[32] = {0};
	sprintf(label, ".Lce_%x:\n", cur_label++);
	fputs(label, out_fp);
}
