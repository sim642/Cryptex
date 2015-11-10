#include "util.h"
#include <string.h>

bool streq(const char* str1, const char* str2)
{
	return strcmp(str1, str2) == 0;
}

bool strpref(const char *str, const char *pre)
{
	return strncmp(str, pre, strlen(pre)) == 0;
}
