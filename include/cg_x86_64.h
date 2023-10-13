#ifndef CG_X86_64_H__
#define CG_X86_64_H__

#include "cgdef.h"

void cg_x86_64_prelude(struct cgstate *cgs);
void cg_x86_64_postlude(struct cgstate *cgs);
void cg_x86_64_right(struct cgstate *cgs);
void cg_x86_64_left(struct cgstate *cgs);
void cg_x86_64_inc(struct cgstate *cgs);
void cg_x86_64_dec(struct cgstate *cgs);
void cg_x86_64_output(struct cgstate *cgs);
void cg_x86_64_input(struct cgstate *cgs);
void cg_x86_64_condbegin(struct cgstate *cgs);
void cg_x86_64_condend(struct cgstate *cgs);

extern void (*cg_x86_64_fntab[10])(struct cgstate *);

#endif
