#include "util.h"

#include <stdint.h>

int
fgetc_back(FILE *fp)
{
	if (ftell(fp) == 0)
		return EOF;

	uint8_t c;
	fseek(fp, -2, SEEK_CUR);
	fread(&c, 1, 1, fp);
	return c;
}
