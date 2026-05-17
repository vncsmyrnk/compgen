#include "ast.h"
#include <stdlib.h>
#include <string.h>

static char *safe_strdup(const char *s) {
    if (!s)
        return NULL;

    size_t len = strlen(s) + 1;
    char *dup = malloc(len);

    if (dup) {
        memcpy(dup, s, len);
    }

    return dup;
}

Command *ast_create_command(const char *name) {
    Command *cmd = calloc(1, sizeof(Command));
    if (cmd && name) {
        cmd->name = safe_strdup(name);
    }
    return cmd;
}

Flag *ast_create_flag(void) { return calloc(1, sizeof(Flag)); }

Arg *ast_create_arg(const char *name) {
    Arg *arg = calloc(1, sizeof(Arg));
    if (arg) {
        arg->name = safe_strdup(name);
        arg->choice_cap = 4; // Start with space for 4 choices
        arg->choices = malloc(sizeof(char *) * arg->choice_cap);
    }
    return arg;
}

void ast_add_choice(Arg *arg, const char *choice) {
    if (!arg || !choice)
        return;

    if (arg->choice_count >= arg->choice_cap) {
        arg->choice_cap *= 2;
        arg->choices = realloc(arg->choices, sizeof(char *) * arg->choice_cap);
    }
    arg->choices[arg->choice_count++] = safe_strdup(choice);
}

// Recursively free the tree to avoid memory leaks
void ast_free_command(Command *cmd) {
    while (cmd != NULL) {
        Command *next_cmd = cmd->next;

        free(cmd->name);
        free(cmd->help);

        // Free subcommands
        ast_free_command(cmd->subcommands);

        // Free flags
        Flag *f = cmd->flags;
        while (f != NULL) {
            Flag *next_f = f->next;
            free(f->short_name);
            free(f->long_name);
            free(f->help);
            free(f->value_name);
            free(f);
            f = next_f;
        }

        // Free arguments
        Arg *a = cmd->args;
        while (a != NULL) {
            Arg *next_a = a->next;
            free(a->name);
            free(a->help);
            for (int i = 0; i < a->choice_count; i++) {
                free(a->choices[i]);
            }
            free(a->choices);
            free(a);
            a = next_a;
        }

        free(cmd);
        cmd = next_cmd;
    }
}
