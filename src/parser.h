#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

typedef enum {
    PARSER_RESULT_OK,
    PARSER_RESULT_ERR_FILE_NOT_FOUND,
    PARSER_RESULT_ERR_FAILED_TO_CREATE_PARSER,
    PARSER_RESULT_ERR_PARSE_FAILED,
    PARSER_RESULT_ERR_TOO_MANY_NODES
} ParseResultStatus;

typedef struct {
    AST *ast;
    ParseResultStatus status;
} ParseResult;

ParseResult parse_file(const char *filepath);
void free_result(ParseResult *r);

#endif // PARSER_H
