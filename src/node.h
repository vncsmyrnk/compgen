#ifndef NODE_H
#define NODE_H

#include "string_builder.h"
#include <stdbool.h>

// Represents a command-line flag
typedef struct Flag {
    char *short_name;  // e.g., "-v"
    char *long_name;   // e.g., "--verbose"
    char *help;        // e.g., "Enable output"
    char *value_name;  // e.g., "<PORT>" (NULL means boolean flag)
    bool global;
    struct Flag *next; // Pointer to the next flag
} Flag;

// Represents the semantic behavior of the argument
typedef enum {
    ARG_TYPE_DEFAULT, // Standard argument or static choices
    ARG_TYPE_FILE,    // Triggers file completion
    ARG_TYPE_DIR,     // Triggers directory completion
    ARG_TYPE_INT      // Guards digits
} ArgType;

// Represents a positional argument with static choices
typedef struct Arg {
    char *name;         // e.g., "<action>"
    char *help;         // e.g., "The action to perform"
    ArgType type;
    char **choices;     // Array of strings: ["start", "stop", "restart"]
    int choice_count;   // How many choices exist
    int choice_cap;     // Internal capacity for the array
    struct Arg *next;  // Pointer to the next flag
} Arg;

// Represents a CLI command or subcommand
typedef struct Command {
    char *name;                  // e.g., "deploy"
    char *help;                  // e.g., "Deploy the system"
    struct Flag *flags;          // Linked list of flags
    struct Arg *args;            // Linked list of arguments
} Command;

typedef enum {
    NODE_CMD,
    NODE_FLAG,
    NODE_ARG
} NodeType;

typedef struct {
    NodeType type;
    union {
        Command *cmd;
        Flag *flag;
        Arg *arg;
    } as;
} Node;

Command *node_create_cmd(const char *name);
Flag *node_create_flag(void);
Arg *node_create_arg(const char *name);
void node_flag_add_choice(Arg *arg, const char *choice);
void node_cmd_print(Command *cmd, int indent, StringBuffer *out);
void node_flag_free(Flag *f);
void node_arg_free(Arg *a);
void node_cmd_free(Command *cmd);

#endif // NODE_H
