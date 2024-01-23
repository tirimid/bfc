#include "util.h"

#include <stdint.h>

int
fgetc_back(FILE *fp)
{
	if (ftell(fp) <= 1)
		return EOF;

	uint8_t c;
	fseek(fp, -2, SEEK_CUR);
	fread(&c, 1, 1, fp);
	return c;
}

void
filepos(FILE *fp, size_t *out_line, size_t *out_col)
{
	*out_line = *out_col = 1;

	size_t reset_pt = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	for (size_t i = 1; i < reset_pt; ++i) {
		if (fgetc(fp) == '\n') {
			*out_col = 1;
			++*out_line;
		} else
			++*out_col;
	}

	fseek(fp, reset_pt, SEEK_SET);
}

int
cpfile(char const *srcpath, char const *dstpath)
{
	FILE *src = fopen(srcpath, "rb");
	if (!src)
		return 1;
	
	FILE *dst = fopen(dstpath, "wb");
	if (!dst) {
		fclose(src);
		return 1;
	}
	
	int c;
	while ((c = fgetc(src)) != EOF)
		fputc(c, dst);
	
	fclose(dst);
	fclose(src);
	
	return 0;
}
