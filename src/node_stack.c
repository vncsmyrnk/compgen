#include "node_stack.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct NodeStackItem {
    Node *node;
    struct NodeStackItem *next;
} NodeStackItem;

struct NodeStack {
    NodeStackItem *top;
};

NodeStack *node_stack_init() { return calloc(1, sizeof(NodeStack)); }

void node_stack_push_item(NodeStack *s, NodeStackItem *i) {
    if (!s->top) {
        s->top = i;
        return;
    }
    i->next = s->top;
    s->top = i;
}

void node_stack_push_cmd(NodeStack *s, Command *c) {
    NodeStackItem *i = calloc(1, sizeof(NodeStackItem));
    Node *n = calloc(1, sizeof(Node));
    n->type = NODE_CMD;
    n->as.cmd = c;
    i->node = n;
    node_stack_push_item(s, i);
}

void node_stack_push_arg(NodeStack *s, Arg *a) {
    NodeStackItem *i = calloc(1, sizeof(NodeStackItem));
    Node *n = calloc(1, sizeof(Node));
    n->type = NODE_ARG;
    n->as.arg = a;
    i->node = n;
    node_stack_push_item(s, i);
}

void node_stack_push_flag(NodeStack *s, Flag *f) {
    NodeStackItem *i = calloc(1, sizeof(NodeStackItem));
    Node *n = calloc(1, sizeof(Node));
    n->type = NODE_FLAG;
    n->as.flag = f;
    i->node = n;
    node_stack_push_item(s, i);
}

Node *node_stack_pop(NodeStack *s) {
    if (!s->top) {
        return NULL;
    }
    Node *top_node = s->top->node;
    NodeStackItem *current_top = s->top;
    s->top = s->top->next;
    free(current_top);
    return top_node;
}

Node *node_stack_peek(NodeStack *s) {
    if (!s->top) {
        return NULL;
    }
    return s->top->node;
}

Node *node_stack_free(NodeStack *s) {
    if (!s) {
        return NULL;
    }
    while (s->top) {
        Node *n = node_stack_pop(s);
        free(n);
    }
    free(s);
    return NULL;
}
