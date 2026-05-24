#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string_dup(const char *s) {
    if (!s)
        return NULL;

    size_t len = strlen(s) + 1;
    char *dup = malloc(len);

    if (dup) {
        memcpy(dup, s, len);
    }

    return dup;
}

char *string_ndup(const char *src, size_t n) {
    if (!src)
        return NULL;

    size_t len = 0;
    while (len < n && src[len] != '\0') {
        len++;
    }

    char *dest = malloc(len + 1);
    if (!dest)
        return NULL;

    memcpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

char *string_slice(char *str, int start, int end) {
    if (!str)
        return NULL;

    int len = strlen(str);

    if (end == 0)
        end = len;

    if (start < 0)
        start = len + start;
    if (end < 0)
        end = len + end;

    if (start < 0)
        start = 0;
    if (end > len)
        end = len;

    if (start >= end) {
        return string_dup("");
    }

    int slice_len = end - start;
    char *new_str = string_ndup(str + start, slice_len);

    return new_str;
}

char *string_slice_consume(char *str, int start, int end) {
    char *new_str = string_slice(str, start, end);
    free(str);
    return new_str;
}
