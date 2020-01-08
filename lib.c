#include "proto.h"

size_t
strlen(const char *s)
{
	size_t len = 0;

	while (*s++ != '\0') {
		len++;
	}
	return len;
}

int
strcmp(const char *s1, const char *s2)
{
	for (;;) {
		char c1 = *s1++;
		char c2 = *s2++;

		if (c1 < c2) {
			return -1;
		}
		if (c1 > c2) {
			return -1;
		}
		if (c1 == 0) {
			return 0;
		}
	}
}