#include "node_stack.h"
#include "ast.h"
#include <stddef.h>

#define MAX_DEPTH 128

Node stack[MAX_DEPTH];
int stack_ptr = 0;

void nstack_push_cmd(Command *c) {
    if (stack_ptr >= MAX_DEPTH)
        return;
    stack[stack_ptr].type = NODE_CMD;
    stack[stack_ptr].as.cmd = c;
    stack_ptr++;
}

void nstack_push_arg(Arg *a) {
    if (stack_ptr >= MAX_DEPTH)
        return;
    stack[stack_ptr].type = NODE_ARG;
    stack[stack_ptr].as.arg = a;
    stack_ptr++;
}

void nstack_push_flag(Flag *f) {
    if (stack_ptr >= MAX_DEPTH)
        return;
    stack[stack_ptr].type = NODE_FLAG;
    stack[stack_ptr].as.flag = f;
    stack_ptr++;
}

Node nstack_pop() {
    if (stack_ptr > 0) {
        stack_ptr--;
        return stack[stack_ptr];
    }
    // Return a safe default if underflow occurs
    Node empty = {NODE_CMD, .as.cmd = NULL};
    return empty;
}
