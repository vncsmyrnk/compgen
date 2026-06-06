#include "ast.h"
#include "node.h"
#include "node_list.h"
#include "shell.h"
#include "string_builder.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void indent(StringBuffer *out, int level) {
    for (int i = 0; i < level; i++) {
        sb_append(out, "    ");
    }
}

static void gen_flag(Flag *f, StringBuffer *out) {
    if (!f || (!f->short_name && !f->long_name)) {
        return;
    }

    bool flag_expects_values = f->choices->count > 0 || f->value_name || f->run;
    bool multiple = f->multiple;

    if (multiple && !flag_expects_values) {
        multiple = false;
    }

    indent(out, 2);

    if (f->short_name && f->long_name) {
        if (multiple) {
            sb_appendf(out, "'*'{%s,%s}", f->short_name, f->long_name,
                       f->short_name, f->long_name);
        } else {
            sb_appendf(out, "'(%s %s)'{%s,%s}", f->short_name, f->long_name,
                       f->short_name, f->long_name);
        }
    } else {
        if (multiple) {
            sb_append(out, "'*'");
        }
        if (f->short_name) {
            sb_append(out, f->short_name);
        } else if (f->long_name) {
            sb_append(out, f->long_name);
        }
    }

    sb_append(out, "'");

    if (flag_expects_values) {
        sb_append(out, "=");
    }

    if (f->help) {
        sb_appendf(out, "[%s]", f->help);
    }

    char *flag_name_canonical = node_flag_name_canonical(f);
    if (!flag_name_canonical || !f->value_name) {
        free(flag_name_canonical);
        sb_append(out, "' \\\n");
        return;
    }

    sb_appendf(out, ":%s:", flag_name_canonical);
    if (f->run) {
        sb_appendf(out, "->action_%s", flag_name_canonical);
    } else if (flag_name_canonical) {
        if (strcmp(flag_name_canonical, "file") == 0) {
            sb_append(out, "_files");
        } else if (strcmp(flag_name_canonical, "dir") == 0) {
            sb_append(out, "_files -/");
        } else if (f->choices->count > 0) {
            sb_append(out, "(");
            for (int i = 0; i < f->choices->count; i++) {
                sb_appendf(out, "%s ", f->choices->values[i]);
            }
            sb_slice(out, 0, -1);
            sb_append(out, ")");
        }
    }

    free(flag_name_canonical);
    sb_append(out, "' \\\n");
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

        int arg_index = 1;
        bool any_multiple_arg_defined = false;

        Arg *a = cmd->args;
        while (a) {
            if (a->type == ARG_TYPE_PRECOMMAND) {
                // No other argument is expected after
                // this type as it generates suggestions
                // for the following command
                break;
            }

            indent(out, 2);
            sb_append(out, "'");
            if (a->multiple) {
                sb_append(out, "*");
                any_multiple_arg_defined = true;
            } else {
                sb_appendf(out, "%d", arg_index);
            }

            switch (a->type) {
            case ARG_TYPE_INT:
                sb_appendf(out, ":%s:_guard \"[0-9]#\"", a->help);
                break;
            case ARG_TYPE_FILE:
                sb_appendf(out, ":%s:_files", a->help);
                break;
            case ARG_TYPE_DIR:
                sb_appendf(out, ":%s:_files -/", a->help);
                break;
            default:
                char *arg_name_canonical = node_arg_name_canonical(a);
                sb_appendf(out, ":%s:", arg_name_canonical);
                if (a->run || a->choices->count > 0) {
                    sb_appendf(out, "->action_%s", arg_name_canonical);
                }
                free(arg_name_canonical);
            }

            if (a->multiple) {
                // No other argument is expected after
                // this type as it defines variadic completions
                sb_append(out, "' && ret=0\n\n");
                break;
            }
            sb_append(out, "' \\\n");

            arg_index++;
            a = a->next;
        }

        if (!any_multiple_arg_defined) {
            indent(out, 2);
            sb_append(out, "'*:: :->args' && ret=0\n\n");
        }

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

                char *arg_name_canonical = node_arg_name_canonical(a);
                indent(out, 2);
                sb_appendf(out, "action_%s)\n", arg_name_canonical);

                indent(out, 3);
                sb_append(out, "local -a choices\n");
                indent(out, 3);
                sb_appendf(out, "choices=(${(f)\"$(_call_program %s %s)\"})\n",
                           arg_name_canonical, a->run);
                free(arg_name_canonical);

                indent(out, 3);
                sb_appendf(out, "compadd -a choices && ret=0\n");

                indent(out, 3);
                sb_append(out, ";;\n");
            } else if (a->choices->count > 0) {
                if (!case_state_run_statement_added) {
                    indent(out, 1);
                    sb_append(out, "case $state in\n");
                    case_state_run_statement_added = true;
                }

                char *arg_name_canonical = node_arg_name_canonical(a);
                indent(out, 2);
                sb_appendf(out, "action_%s)\n", arg_name_canonical);
                free(arg_name_canonical);

                indent(out, 3);
                sb_append(out, "local -a choices\n");

                indent(out, 3);
                sb_append(out, "choices=(");
                for (int i = 0; i < a->choices->count; i++) {
                    sb_appendf(out, "\"%s\" ", a->choices->values[i]);
                }
                sb_slice(out, 0, -1);
                sb_append(out, ")\n");

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

                char *flag_name_canonical = node_flag_name_canonical(f);

                indent(out, 2);
                sb_appendf(out, "action_%s)\n", flag_name_canonical);

                indent(out, 3);
                sb_append(out, "local -a choices\n");
                indent(out, 3);
                sb_appendf(out, "choices=(${(f)\"$(_call_program %s %s)\"})\n",
                           flag_name_canonical, f->run);
                free(flag_name_canonical);

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
