#ifndef NODE_STACK_H
#define NODE_STACK_H

#include "node.h"

void nstack_push_cmd(Command *c);
void nstack_push_arg(Arg *a);
void nstack_push_flag(Flag *a);
Node nstack_pop();

#endif // NODE_STACK_H
