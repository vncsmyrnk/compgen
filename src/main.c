#include "ast.h"
#include "gen_zsh.h"
#include "string_builder.h"
#include <kdl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Safe strdup for strict C11 ---
static char *safe_strdup(const char *s) {
    if (!s)
        return NULL;
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup)
        memcpy(dup, s, len);
    return dup;
}

// --- Debug Helper to Print the AST ---
void debug_print_ast(Command *cmd, int indent) {
    while (cmd) {
        printf("%*sCommand: %s (Help: %s)\n", indent, "",
               cmd->name ? cmd->name : "None", cmd->help ? cmd->help : "None");

        Flag *f = cmd->flags;
        while (f) {
            printf("%*s  Flag: %s | %s (Value: %s, Help: %s)\n", indent, "",
                   f->short_name ? f->short_name : "None",
                   f->long_name ? f->long_name : "None",
                   f->value_name ? f->value_name : "Bool",
                   f->help ? f->help : "None");
            f = f->next;
        }

        Arg *a = cmd->args;
        while (a) {
            printf("%*s  Arg: %s (Help: %s)\n", indent, "",
                   a->name ? a->name : "None", a->help ? a->help : "None");
            for (int i = 0; i < a->choice_count; i++) {
                printf("%*s    Choice: %s\n", indent, "", a->choices[i]);
            }
            a = a->next;
        }

        if (cmd->subcommands) {
            printf("%*s  Subcommands:\n", indent, "");
            debug_print_ast(cmd->subcommands, indent + 4);
        }
        cmd = cmd->next;
    }
}

// --- File Reading Helper ---
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

// --- Main Program ---
int main(void) {
    char *kdl_text = read_file("test.kdl");
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

    // AST State tracking
    Command *root_cmd = ast_create_command("root");
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
                Command *new_cmd = ast_create_command(NULL);
                new_cmd->next = current_cmd->subcommands;
                current_cmd->subcommands = new_cmd;
                current_cmd = new_cmd;
            } else if (strcmp(current_node_type, "flag") == 0) {
                current_flag = ast_create_flag();
                current_flag->next = current_cmd->flags;
                current_cmd->flags = current_flag;
            } else if (strcmp(current_node_type, "arg") == 0) {
                current_arg = ast_create_arg(NULL);
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
                    current_cmd->name = safe_strdup(val);
                } else if (strcmp(current_node_type, "cmd") == 0) {
                    current_cmd->name = safe_strdup(val);
                } else if (strcmp(current_node_type, "help") == 0) {
                    current_cmd->help = safe_strdup(val);
                } else if (strcmp(current_node_type, "flag") == 0 &&
                           current_flag) {
                    char *token = strtok(val, " \t");

                    while (token != NULL) {
                        if (strncmp(token, "--", 2) == 0) {
                            current_flag->long_name = safe_strdup(token);
                        } else if (token[0] == '-') {
                            current_flag->short_name = safe_strdup(token);
                        } else if (token[0] == '<' &&
                                   token[strlen(token) - 1] == '>') {
                            current_flag->value_name = safe_strdup(token);
                        }

                        token = strtok(NULL, " \t");
                    }
                } else if (strcmp(current_node_type, "arg") == 0 &&
                           current_arg) {
                    current_arg->name = safe_strdup(val);
                } else if (strcmp(current_node_type, "choices") == 0 &&
                           current_arg) {
                    ast_add_choice(current_arg, val);
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
                        current_flag->help = safe_strdup(prop_val);
                    } else if (strcmp(current_node_type, "arg") == 0 &&
                               current_arg) {
                        current_arg->help = safe_strdup(prop_val);
                    } else if (strcmp(current_node_type, "cmd") == 0) {
                        current_cmd->help = safe_strdup(prop_val);
                    }
                }
            }
            break;

        case KDL_EVENT_END_NODE:
            // If the node we are exiting is a "cmd", step back up to the root
            // command.
            if (event->name.len == 3 &&
                strncmp(event->name.data, "cmd", 3) == 0) {
                current_cmd = root_cmd;
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

    StringBuffer zsh_code = sb_create();
    generate_zsh(root_cmd, &zsh_code);

    printf("%s\n", zsh_code.data);

    sb_free(&zsh_code);

    ast_free_command(root_cmd);
    return EXIT_SUCCESS;
}
