#include "node.h"
#include "string.h"
#include <stdlib.h>

Command *node_create_cmd(const char *name) {
    Command *cmd = calloc(1, sizeof(Command));
    if (cmd && name) {
        cmd->name = string_dup(name);
    }
    return cmd;
}

Flag *node_create_flag(void) { return calloc(1, sizeof(Flag)); }

char *node_flag_value_name_canonical(Flag *f) {
    if (f && f->value_name) {
        return string_slice(f->value_name, 1, -1);
    }
    return NULL;
}

Arg *node_create_arg(const char *name) {
    Arg *arg = calloc(1, sizeof(Arg));
    if (arg) {
        arg->name = string_dup(name);
        arg->choice_cap = 4; // Start with space for 4 choices
        arg->choices = malloc(sizeof(char *) * arg->choice_cap);
    }
    return arg;
}

void node_flag_add_choice(Arg *arg, const char *choice) {
    if (!arg || !choice)
        return;

    if (arg->choice_count >= arg->choice_cap) {
        arg->choice_cap *= 2;
        arg->choices = realloc(arg->choices, sizeof(char *) * arg->choice_cap);
    }
    arg->choices[arg->choice_count++] = string_dup(choice);
}

void node_cmd_print(Command *cmd, int indent, StringBuffer *out) {
    if (!cmd) {
        return;
    }
    sb_appendf(out, "%*sCommand: %s (Help: %s)\n", indent, "",
               cmd->name ? cmd->name : "None", cmd->help ? cmd->help : "None");

    Flag *f = cmd->flags;
    while (f) {
        sb_appendf(out,
                   "%*s  Flag: %s | %s (Value: %s, Help: %s, Global: %s)\n",
                   indent, "", f->short_name ? f->short_name : "None",
                   f->long_name ? f->long_name : "None",
                   f->value_name ? f->value_name : "Bool",
                   f->help ? f->help : "None", f->global ? "True" : "False");
        f = f->next;
    }

    Arg *a = cmd->args;
    while (a) {
        sb_appendf(out, "%*s  Arg: %s (Help: %s)\n", indent, "",
                   a->name ? a->name : "None", a->help ? a->help : "None");
        for (int i = 0; i < a->choice_count; i++) {
            sb_appendf(out, "%*s    Choice: %s\n", indent, "", a->choices[i]);
        }
        a = a->next;
    }
}

void node_flag_free(Flag *f) {
    if (f) {
        free(f->short_name);
        free(f->long_name);
        free(f->help);
        free(f->value_name);
        free(f->run);
        free(f);
    }
}

void node_arg_free(Arg *a) {
    if (a) {
        free(a->name);
        free(a->help);
        for (int i = 0; i < a->choice_count; i++) {
            free(a->choices[i]);
        }
        free(a->choices);
        free(a->run);
        free(a);
    }
}

void node_cmd_free(Command *cmd) {
    if (cmd) {
        free(cmd->name);
        free(cmd->help);

        Flag *f = cmd->flags;
        while (f) {
            Flag *next_f = f->next;
            node_flag_free(f);
            f = next_f;
        }

        Arg *a = cmd->args;
        while (a) {
            Arg *next_a = a->next;
            node_arg_free(a);
            a = next_a;
        }
        free(cmd);
    }
}
