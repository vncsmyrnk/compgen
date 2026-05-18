#include "ast.h"
#include "node.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTCommand *root = NULL;
ASTCommand *current = NULL;

void ast_init(Command *cmd) {
    ASTCommand *c = calloc(1, sizeof(ASTCommand));
    if (!c) {
        return;
    }
    c->cmd = cmd;
    root = c;
    current = root;
}

void ast_add_cmd(Command *cmd) {
    ASTCommand *c = calloc(1, sizeof(ASTCommand));
    if (!c) {
        return;
    }
    c->cmd = cmd;
    if (!root) {
        root = c;
        current = root;
        return;
    }

    c->parent = current;

    if (!current->child) {
        current->child = c;
    } else {
        ASTCommand *sibling = current->child;
        while (sibling->sibling) {
            sibling = sibling->sibling;
        }
        sibling->sibling = c;
    }

    current = c;
}

void ast_rebase(void) {
    if (current && current->parent) {
        current = current->parent;
    }
}

ASTCommand *ast_root(void) { return root; }

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

void ast_debug_print(void) {
    StringBuffer s = sb_create();
    _ast_debug_print(root, 0, &s);
    printf("%s", s.data);
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

void ast_free(void) {
    _ast_free_recursive(root);
    root = NULL;
    current = NULL;
}
