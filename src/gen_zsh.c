#include "gen_zsh.h"
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
static void gen_cmd_function(Command *cmd, const char *func_name,
                             StringBuffer *out) {
    if (!cmd)
        return;

    // 1. Recursively generate child subcommands FIRST
    Command *sub = cmd->subcommands;
    while (sub) {
        char sub_func[256];
        snprintf(sub_func, sizeof(sub_func), "%s_%s", func_name, sub->name);
        gen_cmd_function(sub, sub_func, out);
        sub = sub->next;
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

    Flag *f = cmd->flags;
    while (f) {
        if (f->short_name)
            gen_single_flag(f, f->short_name, out);
        if (f->long_name)
            gen_single_flag(f, f->long_name, out);
        f = f->next;
    }

    indent(out, 2);
    sb_append(out, "'1: :->action' \\\n");
    indent(out, 2);
    sb_append(out, "'*:: :->args' && ret=0\n\n");

    indent(out, 1);
    sb_append(out, "case $state in\n");

    // --- STATE: action (Suggest static choices AND subcommand names) ---
    indent(out, 2);
    sb_append(out, "action)\n");
    if ((cmd->args && cmd->args->choice_count > 0) || cmd->subcommands) {
        indent(out, 3);
        sb_append(out, "local -a choices\n");
        indent(out, 3);
        sb_append(out, "choices=(\n");

        // Add static choices (e.g., start, stop, restart)
        if (cmd->args) {
            for (int i = 0; i < cmd->args->choice_count; i++) {
                indent(out, 4);
                sb_appendf(out, "'%s'\n", cmd->args->choices[i]);
            }
        }

        // Add Subcommands (e.g., deploy:Deploy the system)
        sub = cmd->subcommands;
        while (sub) {
            indent(out, 4);
            if (sub->help) {
                sb_appendf(out, "'%s:%s'\n", sub->name, sub->help);
            } else {
                sb_appendf(out, "'%s'\n", sub->name);
            }
            sub = sub->next;
        }

        indent(out, 3);
        sb_append(out, ")\n");
        indent(out, 3);
        sb_appendf(out, "_describe 'commands' choices && ret=0\n");
    }
    indent(out, 3);
    sb_append(out, ";;\n");

    // --- STATE: args (Dispatch to subcommand functions) ---
    if (cmd->subcommands) {
        indent(out, 2);
        sb_append(out, "args)\n");
        indent(out, 3);
        sb_append(out, "case $line[1] in\n");

        sub = cmd->subcommands;
        while (sub) {
            indent(out, 4);
            sb_appendf(out, "%s)\n", sub->name);
            indent(out, 5);
            sb_appendf(out, "_%s_%s \"$@\" && ret=0\n", func_name, sub->name);
            indent(out, 5);
            sb_append(out, ";;\n");
            sub = sub->next;
        }

        indent(out, 3);
        sb_append(out, "esac\n");
        indent(out, 3);
        sb_append(out, ";;\n");
    }

    indent(out, 1);
    sb_append(out, "esac\n\n");
    indent(out, 1);
    sb_append(out, "return ret\n");
    sb_append(out, "}\n\n");
}

void generate_zsh(Command *root, StringBuffer *out) {
    if (!root || !root->name)
        return;

    sb_appendf(out, "#compdef %s\n\n", root->name);
    sb_append(out, "# This script was generated automatically\n\n");

    // Start the recursive generation
    gen_cmd_function(root, root->name, out);

    // The Execution Footer
    sb_appendf(out, "if [ \"$funcstack[1]\" = \"_%s\" ]; then\n", root->name);
    indent(out, 1);
    sb_appendf(out, "_%s \"$@\"\n", root->name);
    sb_append(out, "else\n");
    indent(out, 1);
    sb_appendf(out, "compdef _%s %s\n", root->name, root->name);
    sb_append(out, "fi\n");
}
