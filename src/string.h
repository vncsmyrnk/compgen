#ifndef STRING_H
#define STRING_H

#include <stdlib.h>

char *string_dup(const char *s);
char *string_ndup(const char *src, size_t n);
char *string_slice(char *str, int start, int end);
char *string_slice_consume(char *str, int start, int end);

#endif // STRING_H
