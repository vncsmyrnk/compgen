#ifndef NODE_H
#define NODE_H

#include "string_builder.h"
#include <stdbool.h>

typedef struct {
    char **values; // Array of strings: ["start", "stop", "restart"]
    int count;     // How many choices exist
    int cap;       // Internal capacity for the array
} Choices;

// Represents a command-line flag
typedef struct Flag {
    char *short_name; // e.g., "-v"
    char *long_name;  // e.g., "--verbose"
    char *help;       // e.g., "Enable output"
    char *value_name; // e.g., "<PORT>" (NULL means boolean flag)
    char *run;        // Shell command to generate choices dynamically
    Choices *choices; // Makes any sense to this be a pointer?
    bool global;
    bool multiple;
    struct Flag *next; // Pointer to the next flag
} Flag;

// Represents the semantic behavior of the argument
typedef enum {
    ARG_TYPE_DEFAULT,    // Standard argument or static choices
    ARG_TYPE_FILE,       // Triggers file completion
    ARG_TYPE_DIR,        // Triggers directory completion
    ARG_TYPE_PRECOMMAND, // Triggers completions for the following command typed
    ARG_TYPE_INT         // Guards digits
} ArgType;

// Represents a positional argument with static choices
typedef struct Arg {
    char *name; // e.g., "<action>"
    char *help; // e.g., "The action to perform"
    ArgType type;
    Choices *choices;
    char *run; // Shell command to generate choices dynamically
    bool multiple;
    struct Arg *next; // Pointer to the next arg
} Arg;

// Represents a CLI command or subcommand
typedef struct Command {
    char *name; // e.g., "deploy"
    char *help; // e.g., "Deploy the system"
    char *alias;
    struct Flag *flags; // Linked list of flags
    struct Arg *args;   // Linked list of arguments
} Command;

typedef enum { NODE_CMD, NODE_FLAG, NODE_ARG } NodeType;

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
void node_flag_add_choice(Flag *flag, const char *choice);
char *node_flag_name_canonical(Flag *f);
Arg *node_create_arg(const char *name);
void node_arg_add_choice(Arg *arg, const char *choice);
char *node_arg_name_canonical(Arg *a);
void node_cmd_print(Command *cmd, int indent, StringBuffer *out);
void node_flag_free(Flag *f);
void node_arg_free(Arg *a);
void node_cmd_free(Command *cmd);
void node_choices_free(Choices *choices);

#endif // NODE_H
