/* strings.h shim for xc32 bare-metal */
#ifndef STRINGS_H_SHIM
#define STRINGS_H_SHIM

#include <stddef.h>
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);

#endif