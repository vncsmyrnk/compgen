#ifndef GEN_ZSH_H
#define GEN_ZSH_H

#include "ast.h"
#include "string_builder.h"

// The main entry point for the Zsh generator.
// Takes the root AST node and writes the shell script into the buffer.
void generate_zsh(ASTCommand *root, StringBuffer *out);

#endif // GEN_ZSH_H
