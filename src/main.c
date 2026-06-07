#include "argparse.h"
#include "ast.h"
#include "parser.h"
#include "shell.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <file.kdl> [FLAGS]\n", prog_name);
}

int main(int argc, char **argv) {
    ArgParser *parser = argparse_new("Shell completions generator.");
    argparse_add_str_choices(parser, 's', "shell", "Target shell", "zsh",
                             (const char *[]){"zsh", NULL});
    argparse_add_bool(
        parser, 'd', "debug",
        "Prints information useful to debug the completion generation");
    argparse_add_bool(parser, 'v', "version", "Prints version");

    if (!argparse_parse(parser, argc, argv)) {
        argparse_free(parser);
        return 1;
    }

    if (argparse_get_bool(parser, "version")) {
        printf("%s\n", VERSION);
        return EXIT_SUCCESS;
    }

    if (argparse_positional_count(parser) == 0) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_path = argparse_positional_at(parser, 0);
    ParseResult r = parse_file(file_path);
    if (r.status != PARSER_RESULT_OK) {
        switch (r.status) {
        case PARSER_RESULT_ERR_FILE_NOT_FOUND:
            fprintf(stderr, "Error: Could not read KDL file.\n");
            break;
        case PARSER_RESULT_ERR_FILE_TOO_LARGE:
            fprintf(stderr, "Error: KDL file is too large (max 1MB).\n");
            break;
        case PARSER_RESULT_ERR_FAILED_TO_CREATE_PARSER:
            fprintf(stderr, "Error: Failed to create KDL parser.\n");
            break;
        case PARSER_RESULT_ERR_PARSE_FAILED:
            fprintf(stderr, "Error: unable to parse KDL file.\n");
            break;
        case PARSER_RESULT_ERR_TOO_MANY_NODES:
            fprintf(
                stderr,
                "Error: Node limit exceeded. You can configure the limit by "
                "setting the COMPGEN_MAX_NODES environment variable.\n");
            break;
        case PARSER_RESULT_ERR_VALIDATION_FAILED:
            fprintf(stderr, "%s\n", r.error_message);
            break;
        default:
            fprintf(stderr, "Error: unexpected failure.\n");
        }
        free_result(&r);
        return EXIT_FAILURE;
    }

    if (argparse_get_bool(parser, "debug")) {
        argparse_free(parser);
        ast_debug_print(r.ast);
        free_result(&r);
        return EXIT_SUCCESS;
    }

    StringBuffer generated_completion = sb_create();
    if (strcmp(argparse_get_str(parser, "shell"), "zsh") == 0) {
        generate(r.ast, &generated_completion);
    }
    argparse_free(parser);

    if (strcmp(generated_completion.data, "") == 0) {
        sb_free(&generated_completion);
        free_result(&r);
        return EXIT_FAILURE;
    }

    printf("%s\n", generated_completion.data);
    sb_free(&generated_completion);
    free_result(&r);
    return EXIT_SUCCESS;
}
