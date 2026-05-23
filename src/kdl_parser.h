#ifndef KDL_PARSER_H
#define KDL_PARSER_H

#include "ast.h"

typedef enum {
    KDL_RESULT_OK,
    KDL_RESULT_ERR_FILE_NOT_FOUND,
    KDL_RESULT_ERR_FAILED_TO_CREATE_PARSER,
    KDL_RESULT_ERR_PARSE_FAILED
} ParseResultStatus;

typedef struct {
    AST *ast;
    ParseResultStatus status;
} ParseResult;

ParseResult kdl_parse_file(const char *filepath);
void kdl_free_result(ParseResult *r);

#endif // KDL_PARSER_H
