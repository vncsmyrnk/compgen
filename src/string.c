#include <stdlib.h>
#include <string.h>

char *safe_strdup(const char *s) {
    if (!s)
        return NULL;

    size_t len = strlen(s) + 1;
    char *dup = malloc(len);

    if (dup) {
        memcpy(dup, s, len);
    }

    return dup;
}

void safe_free_and_assign(char **target, const char *new_val) {
    if (target) {
        if (*target) {
            free(*target);
        }
        *target = safe_strdup(new_val);
    }
}
