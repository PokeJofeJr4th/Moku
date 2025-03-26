#include <string.h>

#include "helper.h"

int substr(const char *needle, const char *haystack)
{
    int n = strlen(needle);
    while (*haystack++)
        if (strnicmp(needle, haystack, n) == 0)
            return 1;
    return 0;
}