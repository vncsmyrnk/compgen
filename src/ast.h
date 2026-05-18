#ifndef AST_H
#define AST_H

#include "node.h"

typedef struct ASTCommand {
    struct ASTCommand *sibling;
    struct ASTCommand *child;
    struct ASTCommand *parent;
    Command *cmd;
} ASTCommand;

void ast_init(Command *cmd);
void ast_add_cmd(Command *cmd);
void ast_rebase();
ASTCommand *ast_root();
void ast_free();
void ast_debug_print();

#endif // AST_H
