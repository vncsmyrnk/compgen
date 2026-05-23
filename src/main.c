#include "kdl_parser.h"
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ParseResult r = kdl_parse_file("./docs/example.kdl");
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

    StringBuffer zsh_code = sb_create();
    generate(r.ast, &zsh_code);
    printf("%s\n", zsh_code.data);

    sb_free(&zsh_code);
    kdl_free_result(&r);
    return EXIT_SUCCESS;
}
