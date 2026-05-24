#include "argparse.h"
#include "ast.h"
#include "kdl_parser.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s <input.kdl>\n", prog_name);
}

int main(int argc, char **argv) {
    ArgParser *parser = argparse_new("Shell completions generator.");
    argparse_add_bool(
        parser, 'd', "debug",
        "Prints information useful to debug the completion generation");

    if (!argparse_parse(parser, argc, argv)) {
        argparse_free(parser);
        return 1;
    }

    if (argparse_positional_count(parser) == 0) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_path = argparse_positional_at(parser, 0);
    ParseResult r = kdl_parse_file(file_path);
    if (r.status != KDL_RESULT_OK) {
        switch (r.status) {
        case KDL_RESULT_ERR_FILE_NOT_FOUND:
            fprintf(stderr, "Error: Could not read test.kdl\n");
            break;
        case KDL_RESULT_ERR_FAILED_TO_CREATE_PARSER:
            fprintf(stderr, "Error: Failed to create KDL parser.\n");
            break;
        case KDL_RESULT_ERR_PARSE_FAILED:
            fprintf(stderr, "Error: unable to parse KDL file.\n");
            break;
        default:
            fprintf(stderr, "Error: unexpected failure.\n");
        }
        kdl_free_result(&r);
        return EXIT_FAILURE;
    }

    if (argparse_get_bool(parser, "debug")) {
        ast_debug_print(r.ast);
        return EXIT_SUCCESS;
    }

    StringBuffer zsh_code = sb_create();
    generate(r.ast, &zsh_code);
    printf("%s\n", zsh_code.data);

    sb_free(&zsh_code);
    kdl_free_result(&r);
    return EXIT_SUCCESS;
}
