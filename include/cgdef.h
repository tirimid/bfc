#ifndef CGDEF_H__
#define CGDEF_H__

#include <stdio.h>

enum target_arch {
	TARGET_ARCH_X86_64,
};

enum target_os {
	TARGET_OS_LINUX,
};

struct cg_state {
	FILE *in_fp, *out_fp;
	unsigned cur_label;
	enum target_os os;
};

#endif
