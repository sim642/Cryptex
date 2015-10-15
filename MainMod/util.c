#include "util.h"

bool strpref(const char *str, const char *pre)
{
	return strncmp(str, pre, strlen(pre)) == 0;
}
