#ifndef AST_H
#define AST_H

// Represents a command-line flag
typedef struct Flag {
    char *short_name;  // e.g., "-v"
    char *long_name;   // e.g., "--verbose"
    char *help;        // e.g., "Enable output"
    char *value_name;  // e.g., "<PORT>" (NULL means boolean flag)
    struct Flag *next; // Pointer to the next flag
} Flag;

// Represents a positional argument with static choices
typedef struct Arg {
    char *name;       // e.g., "<action>"
    char *help;       // e.g., "The action to perform"
    char **choices;   // Array of strings: ["start", "stop", "restart"]
    int choice_count; // How many choices exist
    int choice_cap;   // Internal capacity for the array
    struct Arg *next; // Pointer to the next argument
} Arg;

// Represents a CLI command or subcommand
typedef struct Command {
    char *name;                  // e.g., "deploy"
    char *help;                  // e.g., "Deploy the system"
    Flag *flags;                 // Linked list of flags
    Arg *args;                   // Linked list of arguments
    struct Command *subcommands; // Linked list of child commands
    struct Command *next;        // Pointer to sibling command
} Command;

// --- Builder Functions ---
Command *ast_create_command(const char *name);
Flag *ast_create_flag(void);
Arg *ast_create_arg(const char *name);

// Adds a choice string to an argument
void ast_add_choice(Arg *arg, const char *choice);

// Safely frees a command and all of its nested children recursively
void ast_free_command(Command *cmd);

#endif // AST_H
