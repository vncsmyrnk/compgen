#ifndef NODE_STACK_H
#define NODE_STACK_H

#include "node.h"

typedef struct NodeStack NodeStack;

NodeStack *node_stack_init();
void node_stack_push_cmd(NodeStack *s, Command *c);
void node_stack_push_arg(NodeStack *s, Arg *a);
void node_stack_push_flag(NodeStack *s, Flag *f);
Node *node_stack_pop(NodeStack *s);
Node *node_stack_peek(NodeStack *s);
Node *node_stack_free(NodeStack *s);

#endif // NODE_STACK_H
