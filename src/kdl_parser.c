#include "ast.h"
#include "node.h"
#include "node_stack.h"
#include "parser.h"
#include "string.h"
#include <kdl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, f);
        buffer[length] = '\0';
    }
    fclose(f);
    return buffer;
}

ParseResult parse_file(const char *filepath) {
    AST *ast = ast_init(NULL);
    ParseResult res = {.ast = ast, .status = PARSER_RESULT_OK};

    char *kdl_text = read_file(filepath);
    if (!kdl_text) {
        res.status = PARSER_RESULT_ERR_FILE_NOT_FOUND;
        return res;
    }

    kdl_parser *parser =
        kdl_create_string_parser(kdl_str_from_cstr(kdl_text), 0);
    if (!parser) {
        free(kdl_text);
        res.status = PARSER_RESULT_ERR_PARSE_FAILED;
        return res;
    }

    kdl_event_data *event;

    Command *root_cmd = node_create_cmd(NULL);
    ast_append(ast, root_cmd);
    Command *current_cmd = root_cmd;

    Flag *current_flag = NULL;
    Arg *current_arg = NULL;
    char current_node_type[32] = {0};

    NodeStack *node_stack = node_stack_init();

    int node_count = 0;
    int max_nodes = 200;
    char *max_nodes_env = getenv("COMPGEN_MAX_NODES");
    if (max_nodes_env) {
        max_nodes = atoi(max_nodes_env);
    }

    while ((event = kdl_parser_next_event(parser)) != NULL) {
        if (event->event == KDL_EVENT_EOF)
            break;
        if (event->event == KDL_EVENT_PARSE_ERROR) {
            fprintf(stderr, "KDL Parse Error!\n");
            break;
        }

        switch (event->event) {
        case KDL_EVENT_START_NODE:
            node_count++;
            if (node_count > max_nodes) {
                res.status = PARSER_RESULT_ERR_TOO_MANY_NODES;
                goto cleanup;
            }

            snprintf(current_node_type, sizeof(current_node_type), "%.*s",
                     (int)event->name.len, event->name.data);
            if (strcmp(current_node_type, "cmd") == 0) {
                Command *new_cmd = node_create_cmd(NULL);
                node_stack_push_cmd(node_stack, new_cmd);
                ast_append(ast, new_cmd);
                current_cmd = new_cmd;
            } else if (strcmp(current_node_type, "flag") == 0) {
                current_flag = node_create_flag();
                node_stack_push_flag(node_stack, current_flag);
                current_flag->next = current_cmd->flags;
                current_cmd->flags = current_flag;
            } else if (strcmp(current_node_type, "arg") == 0) {
                current_arg = node_create_arg(NULL);
                node_stack_push_arg(node_stack, current_arg);
                Arg *a = current_cmd->args;
                if (!a) {
                    current_cmd->args = current_arg;
                } else {
                    while (a->next) {
                        a = a->next;
                    }
                    a->next = current_arg;
                }
            }

            break;

        case KDL_EVENT_ARGUMENT:
            if (event->value.type == KDL_TYPE_STRING) {
                char val[256];
                snprintf(val, sizeof(val), "%.*s", (int)event->value.string.len,
                         event->value.string.data);

                if (strcmp(current_node_type, "name") == 0 &&
                    current_cmd == root_cmd) {
                    current_cmd->name = string_dup(val);
                } else if (strcmp(current_node_type, "cmd") == 0) {
                    current_cmd->name = string_dup(val);
                } else if (strcmp(current_node_type, "help") == 0) {
                    current_cmd->help = string_dup(val);
                } else if (strcmp(current_node_type, "flag") == 0 &&
                           current_flag) {
                    char *token = strtok(val, " \t");

                    while (token != NULL) {
                        if (strncmp(token, "--", 2) == 0) {
                            current_flag->long_name = string_dup(token);
                        } else if (token[0] == '-') {
                            current_flag->short_name = string_dup(token);
                        } else if (token[0] == '<' &&
                                   token[strlen(token) - 1] == '>') {
                            current_flag->value_name = string_dup(token);
                        }

                        token = strtok(NULL, " \t");
                    }
                } else if (strcmp(current_node_type, "arg") == 0 &&
                           current_arg) {
                    current_arg->name = string_dup(val);
                    current_arg->type = ARG_TYPE_DEFAULT;
                    if (strcmp(current_arg->name, "<file>") == 0) {
                        current_arg->type = ARG_TYPE_FILE;
                    } else if (strcmp(current_arg->name, "<dir>") == 0) {
                        current_arg->type = ARG_TYPE_DIR;
                    } else if (strcmp(current_arg->name, "<int>") == 0) {
                        current_arg->type = ARG_TYPE_INT;
                    } else if (strcmp(current_arg->name, "<precommand>") == 0) {
                        current_arg->type = ARG_TYPE_PRECOMMAND;
                    }
                } else if (strcmp(current_node_type, "choices") == 0) {
                    Node *current_node = node_stack_peek(node_stack);
                    if (current_node == NULL) {
                        break;
                    }
                    switch (current_node->type) {
                    case NODE_ARG:
                        if (current_arg) {
                            node_arg_add_choice(current_arg, val);
                        }
                        break;
                    case NODE_FLAG:
                        if (current_flag) {
                            node_flag_add_choice(current_flag, val);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            break;

        case KDL_EVENT_PROPERTY:
            char prop_name[256];
            snprintf(prop_name, sizeof(prop_name), "%.*s", (int)event->name.len,
                     event->name.data);

            if (event->value.type == KDL_TYPE_STRING) {
                char prop_val[256];
                snprintf(prop_val, sizeof(prop_val), "%.*s",
                         (int)event->value.string.len,
                         event->value.string.data);

                if (strcmp(prop_name, "help") == 0) {
                    if (strcmp(current_node_type, "flag") == 0 &&
                        current_flag) {
                        current_flag->help = string_dup(prop_val);
                    } else if (strcmp(current_node_type, "arg") == 0 &&
                               current_arg) {
                        current_arg->help = string_dup(prop_val);
                    } else if (strcmp(current_node_type, "cmd") == 0) {
                        current_cmd->help = string_dup(prop_val);
                    }
                } else if (strcmp(prop_name, "run") == 0) {
                    if (strcmp(current_node_type, "arg") == 0 && current_arg) {
                        current_arg->run = string_dup(prop_val);
                    } else if (strcmp(current_node_type, "flag") == 0 &&
                               current_flag) {
                        current_flag->run = string_dup(prop_val);
                    }
                }
            } else if (event->value.type == KDL_TYPE_BOOLEAN) {
                if (strcmp(prop_name, "global") == 0 &&
                    strcmp(current_node_type, "flag") == 0 && current_flag) {
                    current_flag->global = event->value.boolean;
                } else if (strcmp(prop_name, "multiple") == 0) {
                    if (strcmp(current_node_type, "flag") == 0 &&
                        current_flag) {
                        current_flag->multiple = event->value.boolean;
                    } else if (strcmp(current_node_type, "arg") == 0 &&
                               current_arg) {
                        current_arg->multiple = event->value.boolean;
                    }
                }
            }
            break;

        case KDL_EVENT_END_NODE:
            Node *last_closed_node = node_stack_pop(node_stack);
            if (last_closed_node && last_closed_node->type == NODE_CMD) {
                ast_rebase(ast);
            }
            free(last_closed_node);
            break;

        case KDL_EVENT_PARSE_ERROR:
            fprintf(stderr, "KDL Parse Error!\n");
            break;

        default:
            break;
        }
    }

cleanup:
    kdl_destroy_parser(parser);
    free(kdl_text);
    node_stack_free(node_stack);

    return res;
}

void free_result(ParseResult *r) { ast_free(r->ast); }
