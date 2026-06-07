#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdarg.h>
#include <stddef.h>

typedef struct {
    char *data; // The actual null-terminated string
    size_t len; // Current length (excluding \0)
    size_t cap; // Total allocated memory capacity
} StringBuffer;

StringBuffer sb_create(void);
void sb_free(StringBuffer *sb);
void sb_append(StringBuffer *sb, const char *str);
void sb_append_char(StringBuffer *sb, char c);
void sb_append_shell_escaped(StringBuffer *sb, const char *str);
void sb_appendf(StringBuffer *sb, const char *fmt, ...);
void sb_slice(StringBuffer *sb, int start, int end);

#endif // STRING_BUILDER_H
