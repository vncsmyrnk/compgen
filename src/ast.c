#include "ast.h"
#include "node.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct AST {
    ASTCommand *root;
    ASTCommand *current;
};

AST *ast_init(Command *cmd_root) {
    AST *ast = calloc(1, sizeof(AST));
    if (!ast) {
        return NULL;
    }

    if (!cmd_root) {
        return ast;
    }

    ASTCommand *c = calloc(1, sizeof(ASTCommand));
    if (!c) {
        free(ast);
        return NULL;
    }
    c->cmd = cmd_root;

    ast->root = c;
    ast->current = ast->root;
    return ast;
}

bool ast_empty(AST *ast) { return ast == NULL || ast->root == NULL; }

void ast_append(AST *ast, Command *cmd) {
    if (!ast) {
        return;
    }

    ASTCommand *c = calloc(1, sizeof(ASTCommand));
    if (!c) {
        return;
    }
    c->cmd = cmd;

    if (!ast->root) {
        ast->root = c;
        ast->current = ast->root;
        return;
    }
    c->parent = ast->current;

    if (!ast->current->child) {
        ast->current->child = c;
    } else {
        ASTCommand *sibling = ast->current->child;
        while (sibling->sibling) {
            sibling = sibling->sibling;
        }
        sibling->sibling = c;
    }

    ast->current = c;
}

void ast_rebase(AST *ast) {
    if (ast->current && ast->current->parent) {
        ast->current = ast->current->parent;
    }
}

ASTCommand *ast_root(AST *ast) {
    if (ast) {
        return ast->root;
    }
    return NULL;
}

void _ast_debug_print(ASTCommand *c, int indent, StringBuffer *out) {
    while (c) {
        node_cmd_print(c->cmd, indent, out);
        if (c->child) {
            sb_appendf(out, "%*s  Subcommands:\n", indent, "");
            _ast_debug_print(c->child, indent + 4, out);
        }
        c = c->sibling;
    }
}

void ast_debug_print(AST *ast) {
    StringBuffer s = sb_create();
    _ast_debug_print(ast->root, 0, &s);
    fprintf(stderr, "%s", s.data);
    sb_free(&s);
}

static void _ast_free_recursive(ASTCommand *c) {
    if (!c) {
        return;
    }

    _ast_free_recursive(c->child);
    _ast_free_recursive(c->sibling);

    if (c->cmd) {
        node_cmd_free(c->cmd);
    }
    free(c);
}

void ast_free(AST *ast) {
    if (!ast)
        return;
    _ast_free_recursive(ast->root);
    free(ast);
}
