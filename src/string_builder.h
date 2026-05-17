#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdarg.h>
#include <stddef.h>

typedef struct {
    char *data; // The actual null-terminated string
    size_t len; // Current length (excluding \0)
    size_t cap; // Total allocated memory capacity
} StringBuffer;

// Initialize a new, empty string buffer
StringBuffer sb_create(void);

// Free the allocated memory
void sb_free(StringBuffer *sb);

// Append a standard string
void sb_append(StringBuffer *sb, const char *str);

// Append a single character
void sb_append_char(StringBuffer *sb, char c);

// Append a formatted string (like printf)
void sb_appendf(StringBuffer *sb, const char *fmt, ...);

#endif // STRING_BUILDER_H
