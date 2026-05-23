#ifndef NODE_LIST_H
#define NODE_LIST_H

#include "node.h"

typedef struct NodeList NodeList;

typedef struct NodeListItem {
    Node *node;
    struct NodeListItem *next;
    struct NodeListItem *parent;
} NodeListItem;

NodeList *node_list_init();
void node_list_push_cmd(NodeList *s, Command *c);
void node_list_push_arg(NodeList *s, Arg *a);
void node_list_push_flag(NodeList *s, Flag *f);
NodeListItem *node_list_root(NodeList *s);
void node_list_free(NodeList *s);
void node_list_debug_print(NodeList *l);

#endif // NODE_LIST_H
