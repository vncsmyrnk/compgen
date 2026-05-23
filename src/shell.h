#ifndef SHELL_H
#define SHELL_H

#include "ast.h"
#include "string_builder.h"

void generate(ASTCommand *root, StringBuffer *out);

#endif // SHELL_H
