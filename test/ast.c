#include "../src/ast.h"
#include "../src/node.h"
#include "test.h"

TEST(ast_add_command) {
    Command *cmd = node_create_cmd("root");
    AST *ast = ast_init(cmd);
    ASTCommand *root = ast_root(ast);
    ASSERT_NOT_NULL(root);

    ASSERT_STR_EQ("root", root->cmd->name);

    ast_free(ast);
}

TEST(ast_manipulation) {
    Command *cmd_root = node_create_cmd("root");
    AST *ast = ast_init(cmd_root);

    Command *cmd1 = node_create_cmd("subcommand depth 1");
    ast_append(ast, cmd1);

    Command *cmd11 = node_create_cmd("sibling subcommand depth 1");
    ast_rebase(ast);
    ast_append(ast, cmd11);

    Command *cmd2 = node_create_cmd("subcommand depth 2");
    ast_append(ast, cmd2);

    Command *cmd3 = node_create_cmd("subcommand depth 3");
    ast_append(ast, cmd3);

    ASTCommand *root_node = ast_root(ast);
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

    ast_free(ast);
}

TEST(ast_track_parent) {
    Command *cmd_root = node_create_cmd("root");
    AST *ast = ast_init(cmd_root);

    Command *cmd1 = node_create_cmd("subcommand depth 1");
    ast_append(ast, cmd1);

    Command *cmd11 = node_create_cmd("sibling subcommand depth 1");
    ast_rebase(ast);
    ast_append(ast, cmd11);

    Command *cmd2 = node_create_cmd("subcommand depth 2");
    ast_append(ast, cmd2);

    Command *cmd3 = node_create_cmd("subcommand depth 3");
    ast_append(ast, cmd3);

    Command *cmd31 = node_create_cmd("sibling subcommand depth 3");
    ast_rebase(ast);
    ast_append(ast, cmd31);

    ASTCommand *root_node = ast_root(ast);

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

    ast_free(ast);
}
