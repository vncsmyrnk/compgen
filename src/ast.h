#ifndef AST_H
#define AST_H

#include "node.h"

typedef struct ASTCommand {
    struct ASTCommand *sibling;
    struct ASTCommand *child;
    struct ASTCommand *parent;
    Command *cmd;
} ASTCommand;

typedef struct AST AST;

AST *ast_init(Command *cmd_root);
bool ast_empty(AST *ast);
void ast_append(AST *ast, Command *cmd);
void ast_rebase(AST *ast);
ASTCommand *ast_root(AST *ast);
void ast_free(AST *ast);
void ast_debug_print(AST *ast);

#endif // AST_H
