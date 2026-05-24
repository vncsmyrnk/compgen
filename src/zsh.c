#include "ast.h"
#include "node.h"
#include "node_list.h"
#include "shell.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

    if (f->run) {
        char *flag_value_name_canonical = node_flag_value_name_canonical(f);
        sb_appendf(out, "->action_%s", flag_value_name_canonical);
        free(flag_value_name_canonical);
    }

    sb_append(out, "' \\\n");
}

static void gen_flag(Flag *f, StringBuffer *out) {
    if (f->short_name)
        gen_single_flag(f, f->short_name, out);
    if (f->long_name)
        gen_single_flag(f, f->long_name, out);
}

static void gen_cmd_function(ASTCommand *c, const char *func_name,
                             NodeList *global_nodes_list, StringBuffer *out) {
    if (!c || !c->cmd) {
        return;
    }

    Command *cmd = c->cmd;
    Flag *f = cmd->flags;
    while (f) {
        if (f->global) {
            node_list_push_flag(global_nodes_list, f);
        }
        f = f->next;
    }

    ASTCommand *sub = c->child;
    while (sub && sub->cmd) {
        char sub_func[256];
        snprintf(sub_func, sizeof(sub_func), "%s_%s", func_name,
                 sub->cmd->name);
        gen_cmd_function(sub, sub_func, global_nodes_list, out);
        sub = sub->sibling;
    }

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
            gen_flag(f, out);
            f = f->next;
        }

        NodeListItem *global_item = node_list_root(global_nodes_list);
        while (global_item && global_item->node->type == NODE_FLAG) {
            f = global_item->node->as.flag;
            gen_flag(f, out);
            global_item = global_item->next;
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
            case ARG_TYPE_PRECOMMAND: // No other argument is expected after
                                      // this type as it generates suggestions
                                      // for the following command
                break;
            default:
                indent(out, 2);
                sb_appendf(out, "'%d:%s:", arg_index, a->name);
                if (a->run) {
                    sb_appendf(out, "->action_%s", a->name);
                }
                sb_appendf(out, "' \\\n", a->name);
            }
            arg_index++;
            a = a->next;
        }

        indent(out, 2);
        sb_append(out, "'*:: :->args' && ret=0\n\n");

        bool case_state_run_statement_added = false;
        a = cmd->args;
        while (a) {
            if (a->type == ARG_TYPE_PRECOMMAND) {
                if (!case_state_run_statement_added) {
                    indent(out, 1);
                    sb_append(out, "case $state in\n");
                    case_state_run_statement_added = true;
                }

                indent(out, 2);
                sb_append(out, "args)\n");
                indent(out, 3);
                sb_appendf(out, "_normal\n");

                indent(out, 3);
                sb_append(out, ";;\n");
            } else if (a->run) {
                if (!case_state_run_statement_added) {
                    indent(out, 1);
                    sb_append(out, "case $state in\n");
                    case_state_run_statement_added = true;
                }

                indent(out, 2);
                sb_appendf(out, "action_%s)\n", a->name);

                indent(out, 3);
                sb_append(out, "local -a choices\n");
                indent(out, 3);
                sb_appendf(out, "choices=(${(f)\"$(_call_program %s %s)\"})\n",
                           a->name, a->run);

                indent(out, 3);
                sb_appendf(out, "compadd -a choices && ret=0\n");

                indent(out, 3);
                sb_append(out, ";;\n");
            }
            a = a->next;
        }

        f = cmd->flags;
        while (f) {
            if (f->run) {
                if (!case_state_run_statement_added) {
                    indent(out, 1);
                    sb_append(out, "case $state in\n");
                    case_state_run_statement_added = true;
                }

                char *flag_value_name_canonical =
                    node_flag_value_name_canonical(f);

                indent(out, 2);
                sb_appendf(out, "action_%s)\n", flag_value_name_canonical);

                indent(out, 3);
                sb_append(out, "local -a choices\n");
                indent(out, 3);
                sb_appendf(out, "choices=(${(f)\"$(_call_program %s %s)\"})\n",
                           flag_value_name_canonical, f->run);
                free(flag_value_name_canonical);

                indent(out, 3);
                sb_appendf(out, "compadd -a choices && ret=0\n");

                indent(out, 3);
                sb_append(out, ";;\n");
            }
            f = f->next;
        }

        if (case_state_run_statement_added) {
            indent(out, 1);
            sb_append(out, "esac\n\n");
        }

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

void generate(AST *ast, StringBuffer *out) {
    if (ast_empty(ast)) {
        return;
    }

    ASTCommand *root_ast_cmd = ast_root(ast);
    char *name = root_ast_cmd->cmd->name;
    NodeList *global_flags_list = node_list_init();

    sb_appendf(out, "#compdef %s\n\n", name);
    sb_append(out, "# This script was generated automatically\n\n");

    // Start the recursive generation
    gen_cmd_function(root_ast_cmd, name, global_flags_list, out);
    node_list_free(global_flags_list);

    // The Execution Footer
    sb_appendf(out, "if [ \"$funcstack[1]\" = \"_%s\" ]; then\n", name);
    indent(out, 1);
    sb_appendf(out, "_%s \"$@\"\n", name);
    sb_append(out, "else\n");
    indent(out, 1);
    sb_appendf(out, "compdef _%s %s\n", name, name);
    sb_append(out, "fi\n");
}
