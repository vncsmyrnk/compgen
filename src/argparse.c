#include "argparse.h"
#include "string.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { ARG_BOOL, ARG_STR } ArgType;

typedef struct {
    ArgType type;
    char short_name;
    char *long_name;
    char *help;
    char *default_val;
    bool is_present;
    char *str_val;
} ArgDef;

struct ArgParser {
    char *description;
    ArgDef *args;
    int arg_count;
    int arg_capacity;

    char **positionals;
    int pos_count;
    int pos_capacity;
};

ArgParser *argparse_new(const char *app_description) {
    ArgParser *parser = calloc(1, sizeof(ArgParser));
    if (!parser)
        return NULL;

    if (app_description) {
        parser->description = string_dup(app_description);
    }

    parser->arg_capacity = 8;
    parser->args = malloc(sizeof(ArgDef) * parser->arg_capacity);

    parser->pos_capacity = 8;
    parser->positionals = malloc(sizeof(char *) * parser->pos_capacity);

    return parser;
}

static void add_arg(ArgParser *parser, ArgDef def) {
    if (parser->arg_count >= parser->arg_capacity) {
        parser->arg_capacity *= 2;
        parser->args =
            realloc(parser->args, sizeof(ArgDef) * parser->arg_capacity);
    }
    parser->args[parser->arg_count++] = def;
}

void argparse_add_bool(ArgParser *parser, char short_name,
                       const char *long_name, const char *help) {
    ArgDef def = {.type = ARG_BOOL,
                  .short_name = short_name,
                  .long_name = string_dup(long_name),
                  .help = help ? string_dup(help) : NULL,
                  .default_val = NULL,
                  .is_present = false,
                  .str_val = NULL};
    add_arg(parser, def);
}

void argparse_add_str(ArgParser *parser, char short_name, const char *long_name,
                      const char *help, const char *default_val) {
    ArgDef def = {.type = ARG_STR,
                  .short_name = short_name,
                  .long_name = string_dup(long_name),
                  .help = help ? string_dup(help) : NULL,
                  .default_val = default_val ? string_dup(default_val) : NULL,
                  .is_present = false,
                  .str_val = NULL};
    add_arg(parser, def);
}

bool argparse_parse(ArgParser *parser, int argc, char **argv) {
    struct option *long_opts =
        calloc(parser->arg_count + 2, sizeof(struct option));
    char *short_opts = calloc(parser->arg_count * 3 + 3, sizeof(char));

    int short_idx = 0;

    // Auto-inject --help / -h
    long_opts[0].name = "help";
    long_opts[0].has_arg = no_argument;
    long_opts[0].flag = 0;
    long_opts[0].val = 'h';
    short_opts[short_idx++] = 'h';

    for (int i = 0; i < parser->arg_count; i++) {
        long_opts[i + 1].name = parser->args[i].long_name;
        long_opts[i + 1].has_arg =
            (parser->args[i].type == ARG_STR) ? required_argument : no_argument;
        long_opts[i + 1].flag = 0;
        long_opts[i + 1].val =
            256 + i; // Offset to avoid ASCII short flag collisions

        if (parser->args[i].short_name != '\0') {
            short_opts[short_idx++] = parser->args[i].short_name;
            if (parser->args[i].type == ARG_STR) {
                short_opts[short_idx++] =
                    ':'; // GNU syntax for requiring a value
            }
        }
    }

    // Null terminate
    memset(&long_opts[parser->arg_count + 1], 0, sizeof(struct option));
    short_opts[short_idx] = '\0';

    // 2. Parse using getopt_long
    optind = 1; // Reset global getopt state
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, short_opts, long_opts,
                              &option_index)) != -1) {
        if (opt == 'h') {
            argparse_print_help(parser);
            exit(0);
        } else if (opt == '?') {
            // getopt_long automatically prints the error
            free(long_opts);
            free(short_opts);
            return false;
        } else if (opt >= 256) {
            // It was a long option
            int idx = opt - 256;
            parser->args[idx].is_present = true;
            if (parser->args[idx].type == ARG_STR)
                parser->args[idx].str_val = optarg;
        } else {
            // It was a short option, map it back to the arg
            for (int i = 0; i < parser->arg_count; i++) {
                if (parser->args[i].short_name == opt) {
                    parser->args[i].is_present = true;
                    if (parser->args[i].type == ARG_STR)
                        parser->args[i].str_val = optarg;
                    break;
                }
            }
        }
    }

    free(long_opts);
    free(short_opts);

    // 3. Collect positional arguments
    while (optind < argc) {
        if (parser->pos_count >= parser->pos_capacity) {
            parser->pos_capacity *= 2;
            parser->positionals = realloc(
                parser->positionals, sizeof(char *) * parser->pos_capacity);
        }
        parser->positionals[parser->pos_count++] = argv[optind++];
    }

    return true;
}

bool argparse_get_bool(ArgParser *parser, const char *long_name) {
    for (int i = 0; i < parser->arg_count; i++) {
        if (strcmp(parser->args[i].long_name, long_name) == 0) {
            return parser->args[i].is_present;
        }
    }
    return false;
}

const char *argparse_get_str(ArgParser *parser, const char *long_name) {
    for (int i = 0; i < parser->arg_count; i++) {
        if (strcmp(parser->args[i].long_name, long_name) == 0) {
            if (parser->args[i].is_present && parser->args[i].str_val) {
                return parser->args[i].str_val;
            }
            return parser->args[i].default_val;
        }
    }
    return NULL;
}

int argparse_positional_count(ArgParser *parser) { return parser->pos_count; }

const char *argparse_positional_at(ArgParser *parser, int index) {
    if (index < 0 || index >= parser->pos_count)
        return NULL;
    return parser->positionals[index];
}

void argparse_print_help(ArgParser *parser) {
    if (parser->description) {
        printf("%s\n\n", parser->description);
    }
    printf("Options:\n");

    // Calculate the maximum width of the left column
    int max_left_width = 15;

    for (int i = 0; i < parser->arg_count; i++) {
        ArgDef a = parser->args[i];
        int current_width = 0;

        if (a.short_name != '\0') {
            current_width += 6;
        } else {
            current_width += 6;
        }
        current_width += strlen(a.long_name);

        if (a.type == ARG_STR) {
            current_width += 6;
        }

        if (current_width > max_left_width) {
            max_left_width = current_width;
        }
    }

    // Add a strict 4-space gutter between columns
    int padding = max_left_width + 4;

    // Print the formatted columns
    printf("  %-*s%s\n", padding, "-h, --help",
           "Show this help message and exit");

    for (int i = 0; i < parser->arg_count; i++) {
        ArgDef a = parser->args[i];

        char left_col[256];
        int written = 0;

        if (a.short_name != '\0') {
            written = snprintf(left_col, sizeof(left_col), "-%c, --%s",
                               a.short_name, a.long_name);
        } else {
            written =
                snprintf(left_col, sizeof(left_col), "    --%s", a.long_name);
        }

        if (a.type == ARG_STR && written < (int)sizeof(left_col)) {
            snprintf(left_col + written, sizeof(left_col) - written, " <val>");
        }

        printf("  %-*s", padding, left_col);

        if (a.help) {
            printf("%s", a.help);
        }

        if (a.default_val) {
            printf(" (Default: %s)", a.default_val);
        }

        printf("\n");
    }
}

void argparse_free(ArgParser *parser) {
    if (!parser)
        return;
    for (int i = 0; i < parser->arg_count; i++) {
        free(parser->args[i].long_name);
        if (parser->args[i].help)
            free(parser->args[i].help);
        if (parser->args[i].default_val)
            free(parser->args[i].default_val);
    }
    free(parser->args);
    free(parser->positionals);
    if (parser->description)
        free(parser->description);
    free(parser);
}
