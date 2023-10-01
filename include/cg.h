#ifndef CG_H__
#define CG_H__

#include <stdio.h>

#include "cgdef.h"

void compile(FILE *in_fp, FILE *out_fp, enum target_arch arch, enum target_os os);

#endif
