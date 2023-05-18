#ifndef CG_X86_64_H__
#define CG_X86_64_H__

#include <stdio.h>

#include "cgdef.h"

void cg_x86_64_prelude(FILE *out_fp, enum target_os os);
void cg_x86_64_postlude(FILE *out_fp, enum target_os os);
void cg_x86_64_ptr_right(FILE *out_fp);
void cg_x86_64_ptr_left(FILE *out_fp);
void cg_x86_64_inc(FILE *out_fp);
void cg_x86_64_dec(FILE *out_fp);
void cg_x86_64_output(FILE *out_fp, enum target_os os);
void cg_x86_64_input(FILE *out_fp, enum target_os os);
void cg_x86_64_cond_begin(FILE *in_fp, FILE *out_fp);
void cg_x86_64_cond_end(FILE *in_fp, FILE *out_fp);

#endif
