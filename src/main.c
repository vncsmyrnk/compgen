#include "ast.h"
#include "gen_zsh.h"
#include "node_stack.h"
#include "string.h"
#include "string_builder.h"
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

int main(void) {
    char *kdl_text = read_file("./docs/example.kdl");
    if (!kdl_text) {
        fprintf(stderr, "Error: Could not read test.kdl\n");
        return EXIT_FAILURE;
    }

    kdl_parser *parser =
        kdl_create_string_parser(kdl_str_from_cstr(kdl_text), 0);
    if (!parser) {
        fprintf(stderr, "Error: Failed to create KDL parser.\n");
        free(kdl_text);
        return EXIT_FAILURE;
    }

    kdl_event_data *event;

    Command *root_cmd = node_create_command("root");
    ast_init(root_cmd);
    Command *current_cmd = root_cmd;

    Flag *current_flag = NULL;
    Arg *current_arg = NULL;
    char current_node_type[32] = {0};

    while ((event = kdl_parser_next_event(parser)) != NULL) {
        if (event->event == KDL_EVENT_EOF)
            break;
        if (event->event == KDL_EVENT_PARSE_ERROR) {
            fprintf(stderr, "KDL Parse Error!\n");
            break;
        }

        switch (event->event) {
        case KDL_EVENT_START_NODE:
            snprintf(current_node_type, sizeof(current_node_type), "%.*s",
                     (int)event->name.len, event->name.data);
            if (strcmp(current_node_type, "cmd") == 0) {
                Command *new_cmd = node_create_command(NULL);
                nstack_push_cmd(new_cmd);
                ast_add_cmd(new_cmd);
                current_cmd = new_cmd;
            } else if (strcmp(current_node_type, "flag") == 0) {
                current_flag = node_create_flag();
                nstack_push_flag(current_flag);
                current_flag->next = current_cmd->flags;
                current_cmd->flags = current_flag;
            } else if (strcmp(current_node_type, "arg") == 0) {
                current_arg = node_create_arg(NULL);
                nstack_push_arg(current_arg);
                current_arg->next = current_cmd->args;
                current_cmd->args = current_arg;
            }

            break;

        case KDL_EVENT_ARGUMENT:
            if (event->value.type == KDL_TYPE_STRING) {
                char val[256];
                snprintf(val, sizeof(val), "%.*s", (int)event->value.string.len,
                         event->value.string.data);

                if (strcmp(current_node_type, "name") == 0 &&
                    current_cmd == root_cmd) {
                    safe_free_and_assign(&current_cmd->name, val);
                } else if (strcmp(current_node_type, "cmd") == 0) {
                    safe_free_and_assign(&current_cmd->name, val);
                } else if (strcmp(current_node_type, "help") == 0) {
                    safe_free_and_assign(&current_cmd->help, val);
                } else if (strcmp(current_node_type, "flag") == 0 &&
                           current_flag) {
                    char *token = strtok(val, " \t");

                    while (token != NULL) {
                        if (strncmp(token, "--", 2) == 0) {
                            safe_free_and_assign(&current_flag->long_name,
                                                 token);
                        } else if (token[0] == '-') {
                            safe_free_and_assign(&current_flag->short_name,
                                                 token);
                        } else if (token[0] == '<' &&
                                   token[strlen(token) - 1] == '>') {
                            safe_free_and_assign(&current_flag->value_name,
                                                 token);
                        }

                        token = strtok(NULL, " \t");
                    }
                } else if (strcmp(current_node_type, "arg") == 0 &&
                           current_arg) {
                    safe_free_and_assign(&current_arg->name, val);
                } else if (strcmp(current_node_type, "choices") == 0 &&
                           current_arg) {
                    node_flag_add_choice(current_arg, val);
                }
            }
            break;

        case KDL_EVENT_PROPERTY:
            if (event->value.type == KDL_TYPE_STRING) {
                char prop_name[256];
                char prop_val[256];
                snprintf(prop_name, sizeof(prop_name), "%.*s",
                         (int)event->name.len, event->name.data);
                snprintf(prop_val, sizeof(prop_val), "%.*s",
                         (int)event->value.string.len,
                         event->value.string.data);

                if (strcmp(prop_name, "help") == 0) {
                    if (strcmp(current_node_type, "flag") == 0 &&
                        current_flag) {
                        safe_free_and_assign(&current_flag->help, prop_val);
                    } else if (strcmp(current_node_type, "arg") == 0 &&
                               current_arg) {
                        safe_free_and_assign(&current_arg->help, prop_val);
                    } else if (strcmp(current_node_type, "cmd") == 0) {
                        safe_free_and_assign(&current_cmd->help, prop_val);
                    }
                }
            }
            break;

        case KDL_EVENT_END_NODE:
            Node last_closed_node = nstack_pop();
            if (last_closed_node.type == NODE_CMD && last_closed_node.as.cmd) {
                ast_rebase();
            }
            break;

        case KDL_EVENT_PARSE_ERROR:
            fprintf(stderr, "KDL Parse Error!\n");
            break;

        default:
            break;
        }
    }

    kdl_destroy_parser(parser);
    free(kdl_text);

    ast_debug_print();

    StringBuffer zsh_code = sb_create();
    generate_zsh(ast_root(), &zsh_code);

    printf("%s\n", zsh_code.data);

    sb_free(&zsh_code);

    ast_free();
    return EXIT_SUCCESS;
}
