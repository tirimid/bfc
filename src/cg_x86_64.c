#include "cg_x86_64.h"

#include <stddef.h>
#include <stdlib.h>

#include "util.h"

void (*cg_x86_64_fntab[10])(struct cgstate *) = {
	cg_x86_64_prelude,
	cg_x86_64_postlude,
	cg_x86_64_right,
	cg_x86_64_left,
	cg_x86_64_inc,
	cg_x86_64_dec,
	cg_x86_64_output,
	cg_x86_64_input,
	cg_x86_64_condbegin,
	cg_x86_64_condend,
};

void
cg_x86_64_prelude(struct cgstate *cgs)
{
	char const *prelude_indep =
		"\t.set TAPESIZE, 4194304\n"
		"\t.set PTRSTART, 2097152\n"
		"\t.section .data\n"
		"bf_ptrerr:\n"
		"\t.ascii \"[BF] data pointer left acceptable range of cells\\n\"\n"
		"\t.section .bss\n"
		"bf_inputbuf:\n"
		"\t.skip 32\n"
		"\t.section .text\n"
		"error:\n"
		"\tcall print\n"
		"\tmov $1, %rdi\n"
		"\tjmp quit\n"
		".Lptrerr:\n"
		"\tmov $bf_ptrerr, %rdi\n"
		"\tmov $50, %rsi\n"
		"\tcall error\n"
		"bf_right:\n"
		"\tinc %r13\n"
		"\tmov %r12, %rax\n"
		"\tadd $TAPESIZE, %rax\n"
		"\tcmp %rax, %r13\n"
		"\tjg .Lptrerr\n"
		"\tret\n"
		"bf_left:\n"
		"\tdec %r13\n"
		"\tcmp %r12, %r13\n"
		"\tjl .Lptrerr\n"
		"\tret\n"
		"bf_output:\n"
		"\tmov %r13, %rdi\n"
		"\tmov $1, %rsi\n"
		"\tcall print\n"
		"\tret\n"
		"bf_input:\n"
		"\tcall read\n"
		"\tmovb bf_inputbuf, %al\n"
		"\tmovb %al, (%r13)\n"
		"\tret\n";

	char const *prelude_osdep;
	switch (cgs->os) {
	case OS_LINUX:
		prelude_osdep =
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
			"\tmov $bf_inputbuf, %rsi\n"
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
	case OS_LINUX:
		prelude_start =
			"\t.global _start\n"
			"_start:\n"
			"\tmov $12, %rax\n"
			"\tmov $0, %rdi\n"
			"\tsyscall\n"
			"\tmov %rax, %r12\n"
			"\tmov %rax, %rdi\n"
			"\tadd $TAPESIZE, %rdi\n"
			"\tmov $12, %rax\n"
			"\tsyscall\n"
			"\tmov %r12, %rdi\n"
			"\tmov $TAPESIZE, %rcx\n"
			"\txor %rax, %rax\n"
			"\trep stosb\n"
			"\tmov %r12, %r13\n"
			"\tadd $PTRSTART, %r13\n";
		break;
	}

	fputs(prelude_indep, cgs->outfp);
	fputs(prelude_osdep, cgs->outfp);
	fputs(prelude_start, cgs->outfp);
}

void
cg_x86_64_postlude(struct cgstate *cgs)
{
	fputs("\tmov $0, %rdi\n"
	      "\tjmp quit\n", cgs->outfp);
}

void
cg_x86_64_right(struct cgstate *cgs)
{
	fputs("\tcall bf_right\n", cgs->outfp);
}

void
cg_x86_64_left(struct cgstate *cgs)
{
	fputs("\tcall bf_left\n", cgs->outfp);
}

void
cg_x86_64_inc(struct cgstate *cgs)
{
	unsigned inccnt = 1;
	
	while (fgetc(cgs->infp) == '+')
		++inccnt;

	fgetc_back(cgs->infp);

	char add[32];
	sprintf(add, "\taddb $0x%x, (%%r13)\n", inccnt);
	fputs(add, cgs->outfp);
}

void
cg_x86_64_dec(struct cgstate *cgs)
{
	unsigned deccnt = 1;
	
	while (fgetc(cgs->infp) == '-')
		++deccnt;

	fgetc_back(cgs->infp);

	char sub[32];
	sprintf(sub, "\tsubb $0x%x, (%%r13)\n", deccnt);
	fputs(sub, cgs->outfp);
}

void
cg_x86_64_output(struct cgstate *cgs)
{
	fputs("\tcall bf_output\n", cgs->outfp);
}

void
cg_x86_64_input(struct cgstate *cgs)
{
	fputs("\tcall bf_input\n", cgs->outfp);
}

void
cg_x86_64_condbegin(struct cgstate *cgs)
{
	size_t resetpt = ftell(cgs->infp);

	unsigned jmplabel = cgs->curlabel;
	size_t nestsrem = 1;
	for (int c; (c = fgetc(cgs->infp)) != EOF && nestsrem;) {
		jmplabel += c == '[' || c == ']';
		nestsrem += c == '[';
		nestsrem -= c == ']';
	}

	fseek(cgs->infp, resetpt, SEEK_SET);

	if (nestsrem) {
		size_t line, col;
		filepos(cgs->infp, &line, &col);
		fprintf(stderr, "%zu:%zu - unmatched conditional [-bracket!\n", line, col);
		exit(1);
	}

	fputs("\tcmpb $0, (%r13)\n", cgs->outfp);

	char jmp[32] = {0};
	sprintf(jmp, "\tje .Lce_%x\n", jmplabel);
	fputs(jmp, cgs->outfp);

	char label[32] = {0};
	sprintf(label, ".Lcb_%x:\n", cgs->curlabel++);
	fputs(label, cgs->outfp);
}

void
cg_x86_64_condend(struct cgstate *cgs)
{
	size_t resetpt = ftell(cgs->infp);

	unsigned jmplabel = cgs->curlabel;
	size_t nestsrem = 1;
	for (int c; (c = fgetc_back(cgs->infp)) != EOF && nestsrem;) {
		jmplabel -= c == '[' || c == ']';
		nestsrem -= c == '[';
		nestsrem += c == ']';
	}

	fseek(cgs->infp, resetpt, SEEK_SET);

	if (nestsrem) {
		size_t line, col;
		filepos(cgs->infp, &line, &col);
		fprintf(stderr, "%zu:%zu - unmatched conditional ]-bracket!\n", line, col);
		exit(1);
	}

	fputs("\tcmpb $0, (%r13)\n", cgs->outfp);

	char jmp[32] = {0};
	sprintf(jmp, "\tjne .Lcb_%x\n", jmplabel);
	fputs(jmp, cgs->outfp);

	char label[32] = {0};
	sprintf(label, ".Lce_%x:\n", cgs->curlabel++);
	fputs(label, cgs->outfp);
}
