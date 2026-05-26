#include "string_builder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Start small; it will automatically double when needed
#define SB_INIT_CAPACITY 128

StringBuffer sb_create(void) {
    StringBuffer sb;
    sb.cap = SB_INIT_CAPACITY;
    sb.len = 0;
    sb.data = malloc(sb.cap);
    if (sb.data != NULL) {
        sb.data[0] = '\0';
    } else {
        fprintf(stderr,
                "Fatal error: StringBuffer initial allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    return sb;
}

void sb_free(StringBuffer *sb) {
    if (sb && sb->data) {
        free(sb->data);
        sb->data = NULL;
        sb->len = 0;
        sb->cap = 0;
    }
}

// Internal helper to ensure we have enough space before copying
static void sb_ensure_capacity(StringBuffer *sb, size_t needed) {
    // +1 ensures we always have room for the null terminator
    if (sb->len + needed + 1 > sb->cap) {
        size_t new_cap = sb->cap * 2;
        // Keep doubling until it's large enough (in case of massive appends)
        while (sb->len + needed + 1 > new_cap) {
            new_cap *= 2;
        }

        char *new_data = realloc(sb->data, new_cap);
        if (!new_data) {
            fprintf(
                stderr,
                "Fatal error: StringBuffer out of memory during realloc.\n");
            exit(EXIT_FAILURE);
        }
        sb->data = new_data;
        sb->cap = new_cap;
    }
}

void sb_append(StringBuffer *sb, const char *str) {
    if (!str)
        return;
    size_t str_len = strlen(str);
    sb_ensure_capacity(sb, str_len);

    // Copy the string and the null terminator
    memcpy(sb->data + sb->len, str, str_len + 1);
    sb->len += str_len;
}

void sb_append_char(StringBuffer *sb, char c) {
    sb_ensure_capacity(sb, 1);
    sb->data[sb->len] = c;
    sb->len++;
    sb->data[sb->len] = '\0';
}

void sb_appendf(StringBuffer *sb, const char *fmt, ...) {
    va_list args;

    // First pass: Determine how many bytes the formatted string will take
    va_start(args, fmt);
    int needed = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (needed < 0)
        return; // Encoding error in formatting

    // Ensure we have enough capacity for the new string
    sb_ensure_capacity(sb, (size_t)needed);

    // Second pass: Actually format and write the string into our buffer
    va_start(args, fmt);
    vsnprintf(sb->data + sb->len, needed + 1, fmt, args);
    va_end(args);

    sb->len += needed;
}

void sb_slice(StringBuffer *sb, int start, int end) {
    if (!sb || !sb->data || sb->len == 0)
        return;

    if (start < 0)
        start = sb->len + start;
    if (end < 0)
        end = sb->len + end;

    if (end == 0)
        end = sb->len;

    if (start < 0)
        start = 0;
    if (end > (int)sb->len)
        end = sb->len;

    if (start >= end) {
        sb->len = 0;
        sb->data[0] = '\0';
        return;
    }

    size_t new_len = end - start;

    if (start > 0) {
        memmove(sb->data, sb->data + start, new_len);
    }

    sb->len = new_len;
    sb->data[sb->len] = '\0';
}
