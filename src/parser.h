#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

typedef enum {
    PARSER_RESULT_OK,
    PARSER_RESULT_ERR_FILE_NOT_FOUND,
    PARSER_RESULT_ERR_FAILED_TO_CREATE_PARSER,
    PARSER_RESULT_ERR_PARSE_FAILED,
    PARSER_RESULT_ERR_TOO_MANY_NODES,
    PARSER_RESULT_ERR_VALIDATION_FAILED
} ParseResultStatus;

typedef struct {
    AST *ast;
    ParseResultStatus status;
    char *error_message;
} ParseResult;

ParseResult parse_file(const char *filepath);
void free_result(ParseResult *r);

#endif // PARSER_H
