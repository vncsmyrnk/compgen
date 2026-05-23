#include "ast.h"
#include "shell.h"
#include <stdio.h>

static void indent(StringBuffer *out, int level) {
    for (int i = 0; i < level; i++) {
        sb_append(out, "    ");
    }
}

static void gen_single_flag(Flag *f, const char *flag_name, StringBuffer *out) {
    indent(out, 2);
    sb_append(out, "'(");
    if (f->short_name)
        sb_append(out, f->short_name);
    if (f->short_name && f->long_name)
        sb_append(out, " ");
    if (f->long_name)
        sb_append(out, f->long_name);
    sb_append(out, ")'");

    sb_append(out, flag_name);
    sb_append(out, "'");
    if (f->help)
        sb_appendf(out, "[%s]", f->help);
    if (f->value_name)
        sb_appendf(out, ":%s:", f->value_name);
    sb_append(out, "' \\\n");
}

// Recursive function to generate Zsh for a command and all its children
static void gen_cmd_function(ASTCommand *c, const char *func_name,
                             StringBuffer *out) {
    if (!c || !c->cmd)
        return;

    Command *cmd = c->cmd;

    // 1. Recursively generate child subcommands FIRST
    ASTCommand *sub = c->child;
    while (sub && sub->cmd) {
        char sub_func[256];
        snprintf(sub_func, sizeof(sub_func), "%s_%s", func_name,
                 sub->cmd->name);
        gen_cmd_function(sub, sub_func, out);
        sub = sub->sibling;
    }

    // 2. Generate the function for THIS command
    sb_appendf(out, "function _%s() {\n", func_name);

    indent(out, 1);
    sb_append(out, "local context state state_descr line\n");
    indent(out, 1);
    sb_append(out, "typeset -A opt_args\n");
    indent(out, 1);
    sb_append(out, "local ret=1\n\n");

    indent(out, 1);
    sb_append(out, "_arguments -C \\\n");

    if (!c->child) {
        Flag *f = cmd->flags;
        while (f) {
            if (f->short_name)
                gen_single_flag(f, f->short_name, out);
            if (f->long_name)
                gen_single_flag(f, f->long_name, out);
            f = f->next;
        }

        Arg *a = cmd->args;
        int arg_index = 1;
        while (a) {
            switch (a->type) {
            case ARG_TYPE_INT:
                indent(out, 2);
                sb_appendf(out, "'%d:%s:_guard \"[0-9]#\"' \\\n", arg_index,
                           a->help);
                break;
            case ARG_TYPE_FILE:
                indent(out, 2);
                sb_appendf(out, "'%d:%s:_files' \\\n", arg_index, a->help);
                break;
            case ARG_TYPE_DIR:
                indent(out, 2);
                sb_appendf(out, "'%d:%s:_files -/' \\\n", arg_index, a->help);
                break;
            default:
                indent(out, 2);
                sb_appendf(out, "'%d:%s:' \\\n", arg_index, a->help);
            }
            arg_index++;
            a = a->next;
        }
        indent(out, 2);
        sb_append(out, "'*:: :->args' && ret=0\n\n");

        indent(out, 1);
        sb_append(out, "return ret\n");
        sb_append(out, "}\n\n");
        return;
    }

    indent(out, 2);
    sb_append(out, "'1: :->action' \\\n");
    indent(out, 2);
    sb_append(out, "'*:: :->args' && ret=0\n\n");

    indent(out, 1);
    sb_append(out, "case $state in\n");

    indent(out, 2);
    sb_append(out, "action)\n");

    indent(out, 3);
    sb_append(out, "local -a choices\n");
    indent(out, 3);
    sb_append(out, "choices=(\n");

    sub = c->child;
    while (sub && sub->cmd) {
        indent(out, 4);
        if (sub->cmd->help) {
            sb_appendf(out, "'%s:%s'\n", sub->cmd->name, sub->cmd->help);
        } else {
            sb_appendf(out, "'%s'\n", sub->cmd->name);
        }
        sub = sub->sibling;
    }

    indent(out, 3);
    sb_append(out, ")\n");
    indent(out, 3);
    sb_appendf(out, "_describe 'commands' choices && ret=0\n");

    indent(out, 3);
    sb_append(out, ";;\n");

    indent(out, 2);
    sb_append(out, "args)\n");
    indent(out, 3);
    sb_append(out, "case $line[1] in\n");

    sub = c->child;
    while (sub && sub->cmd) {
        indent(out, 4);
        sb_appendf(out, "%s)\n", sub->cmd->name);
        indent(out, 5);
        sb_appendf(out, "_%s_%s \"$@\" && ret=0\n", func_name, sub->cmd->name);
        indent(out, 5);
        sb_append(out, ";;\n");
        sub = sub->sibling;
    }

    indent(out, 3);
    sb_append(out, "esac\n");
    indent(out, 3);
    sb_append(out, ";;\n");

    indent(out, 1);
    sb_append(out, "esac\n\n");
    indent(out, 1);
    sb_append(out, "return ret\n");
    sb_append(out, "}\n\n");
}

void generate(ASTCommand *root, StringBuffer *out) {
    if (!root || !root->cmd)
        return;

    char *name = root->cmd->name;

    sb_appendf(out, "#compdef %s\n\n", name);
    sb_append(out, "# This script was generated automatically\n\n");

    // Start the recursive generation
    gen_cmd_function(root, name, out);

    // The Execution Footer
    sb_appendf(out, "if [ \"$funcstack[1]\" = \"_%s\" ]; then\n", name);
    indent(out, 1);
    sb_appendf(out, "_%s \"$@\"\n", name);
    sb_append(out, "else\n");
    indent(out, 1);
    sb_appendf(out, "compdef _%s %s\n", name, name);
    sb_append(out, "fi\n");
}
