#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/ast.h"
#include "../src/node.h"

// A macro to compare strings and print the actual values on failure
#define ASSERT_STR_EQ(expected, actual)                                        \
    do {                                                                       \
        const char *exp = (expected);                                          \
        const char *act = (actual);                                            \
        if (strcmp(exp, act) != 0) {                                           \
            fprintf(                                                           \
                stderr,                                                        \
                "\n[FAIL] %s:%d\n  Expected: \"%s\"\n  Actual:   \"%s\"\n",    \
                __FILE__, __LINE__, exp, act);                                 \
            assert(0); /* Force the abort */                                   \
        }                                                                      \
    } while (0)

// A macro to check pointers
#define ASSERT_NOT_NULL(ptr)                                                   \
    do {                                                                       \
        if ((ptr) == NULL) {                                                   \
            fprintf(stderr,                                                    \
                    "\n[FAIL] %s:%d\n  Pointer '%s' was unexpectedly NULL.\n", \
                    __FILE__, __LINE__, #ptr);                                 \
            assert(0);                                                         \
        }                                                                      \
    } while (0)

void test_ast_add_command() {
    printf("Running test_ast_add_command... ");

    Command *cmd = node_create_cmd("root");
    ast_add_cmd(cmd);
    ASTCommand *root = ast_root();
    ASSERT_NOT_NULL(root);

    ASSERT_STR_EQ("root", cmd->name);

    ast_free();
    printf("OK\n");
}

void test_ast_manipulation() {
    printf("Running test_ast_manipulation ");

    Command *cmd_root = node_create_cmd("root");
    ast_add_cmd(cmd_root);

    Command *cmd1 = node_create_cmd("subcommand depth 1");
    ast_add_cmd(cmd1);

    Command *cmd11 = node_create_cmd("sibling subcommand depth 1");
    ast_rebase();
    ast_add_cmd(cmd11);

    Command *cmd2 = node_create_cmd("subcommand depth 2");
    ast_add_cmd(cmd2);

    Command *cmd3 = node_create_cmd("subcommand depth 3");
    ast_add_cmd(cmd3);

    ASTCommand *root_node = ast_root();
    ASSERT_STR_EQ(cmd_root->name, root_node->cmd->name);

    ASTCommand *ast_cmd = root_node;

    ast_cmd = ast_cmd->child;
    ASSERT_NOT_NULL(ast_cmd);
    ASSERT_STR_EQ(cmd1->name, ast_cmd->cmd->name);

    ast_cmd = ast_cmd->sibling;
    ASSERT_NOT_NULL(ast_cmd);
    ASSERT_STR_EQ(cmd11->name, ast_cmd->cmd->name);

    ast_cmd = ast_cmd->child;
    ASSERT_NOT_NULL(ast_cmd);
    ASSERT_STR_EQ(cmd2->name, ast_cmd->cmd->name);

    ast_cmd = ast_cmd->child;
    ASSERT_NOT_NULL(ast_cmd);
    ASSERT_STR_EQ(cmd3->name, ast_cmd->cmd->name);

    ast_free();
    printf("OK\n");
}

void test_ast_track_parent() {
    printf("Running test_ast_manipulation... ");

    Command *cmd_root = node_create_cmd("root");
    ast_add_cmd(cmd_root);

    Command *cmd1 = node_create_cmd("subcommand depth 1");
    ast_add_cmd(cmd1);

    Command *cmd11 = node_create_cmd("sibling subcommand depth 1");
    ast_rebase();
    ast_add_cmd(cmd11);

    Command *cmd2 = node_create_cmd("subcommand depth 2");
    ast_add_cmd(cmd2);

    Command *cmd3 = node_create_cmd("subcommand depth 3");
    ast_add_cmd(cmd3);

    Command *cmd31 = node_create_cmd("sibling subcommand depth 3");
    ast_rebase();
    ast_add_cmd(cmd31);

    ASTCommand *root_node = ast_root();

    ASTCommand *ast_cmd = root_node;
    ASTCommand *ast_cmd1 = ast_cmd->child;
    ASTCommand *ast_cmd11 = ast_cmd1->sibling;
    ASTCommand *ast_cmd2 = ast_cmd11->child;
    ASTCommand *ast_cmd3 = ast_cmd2->child;
    ASTCommand *ast_cmd31 = ast_cmd3->sibling;

    ASSERT_STR_EQ(cmd_root->name, ast_cmd1->parent->cmd->name);
    ASSERT_STR_EQ(cmd_root->name, ast_cmd11->parent->cmd->name);
    ASSERT_STR_EQ(ast_cmd11->cmd->name, ast_cmd2->parent->cmd->name);
    ASSERT_STR_EQ(ast_cmd2->cmd->name, ast_cmd3->parent->cmd->name);
    ASSERT_STR_EQ(ast_cmd2->cmd->name, ast_cmd31->parent->cmd->name);

    ast_free();
    printf("OK\n");
}

int main() {
    printf("=== Starting AST Unit Tests ===\n");

    test_ast_add_command();
    test_ast_manipulation();
    test_ast_track_parent();

    printf("=== All tests passed successfully! ===\n");
    return 0;
}
