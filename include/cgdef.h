#ifndef CGDEF_H__
#define CGDEF_H__

#include <stdio.h>

enum target_arch {
	TARGET_ARCH_X86_64,
};

enum target_os {
	TARGET_OS_LINUX,
};

enum fntab_ent {
	FNTAB_ENT_PRELUDE = 0,
	FNTAB_ENT_POSTLUDE,
	FNTAB_ENT_RIGHT,
	FNTAB_ENT_LEFT,
	FNTAB_ENT_INC,
	FNTAB_ENT_DEC,
	FNTAB_ENT_OUTPUT,
	FNTAB_ENT_INPUT,
	FNTAB_ENT_COND_BEGIN,
	FNTAB_ENT_COND_END,
	FNTAB_ENT_LAST__,
};

struct cg_state {
	FILE *in_fp, *out_fp;
	unsigned cur_label;
	enum target_os os;
};

#endif
