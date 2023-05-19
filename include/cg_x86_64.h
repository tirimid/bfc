#ifndef CG_X86_64_H__
#define CG_X86_64_H__

#include "cgdef.h"

void cg_x86_64_prelude(struct cg_state *cgs);
void cg_x86_64_postlude(struct cg_state *cgs);
void cg_x86_64_ptr_right(struct cg_state *cgs);
void cg_x86_64_ptr_left(struct cg_state *cgs);
void cg_x86_64_inc(struct cg_state *cgs);
void cg_x86_64_dec(struct cg_state *cgs);
void cg_x86_64_output(struct cg_state *cgs);
void cg_x86_64_input(struct cg_state *cgs);
void cg_x86_64_cond_begin(struct cg_state *cgs);
void cg_x86_64_cond_end(struct cg_state *cgs);

#endif
