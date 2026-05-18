#ifndef STRING_H
#define STRING_H

char *safe_strdup(const char *s);
void safe_free_and_assign(char **target, const char *new_val);

#endif // STRING_H
