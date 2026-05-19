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
void ast_rebase(void);
ASTCommand *ast_root(void);
void ast_free(void);
void ast_debug_print(void);

#endif // AST_H
