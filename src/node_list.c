#include "node_list.h"
#include "ast.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct NodeList {
    NodeListItem *root;
    NodeListItem *current;
};

NodeList *node_list_init() { return calloc(1, sizeof(NodeList)); }

void node_list_push_item(NodeList *l, NodeListItem *i) {
    if (!l->root) {
        l->root = i;
        l->current = i;
        return;
    }
    i->parent = l->current;
    l->current->next = i;
    l->current = i;
}

void node_list_push_flag(NodeList *l, Flag *f) {
    NodeListItem *i = calloc(1, sizeof(NodeListItem));
    Node *n = calloc(1, sizeof(Node));
    n->type = NODE_FLAG;
    n->as.flag = f;
    i->node = n;
    node_list_push_item(l, i);
}

NodeListItem *node_list_root(NodeList *l) {
    if (!l) {
        return NULL;
    }
    return l->root;
}

void node_list_debug_print(NodeList *l) {
    if (!l) {
        return;
    }
    NodeListItem *i = l->root;
    while (i) {
        printf("%s -> %i\n", i->node->as.flag->short_name,
               i->node->as.flag->global);
        i = i->next;
    }
}

void node_list_free(NodeList *l) {
    if (!l) {
        return;
    }
    while (l->current) {
        NodeListItem *current = l->current;
        l->current = current->parent;
        free(current->node);
        free(current);
    }
    free(l);
}
