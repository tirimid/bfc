#ifndef UTIL_H__
#define UTIL_H__

#include <stdio.h>
#include <stddef.h>

int fgetc_back(FILE *fp);
void file_pos(FILE *fp, size_t *out_line, size_t *out_col);

#endif