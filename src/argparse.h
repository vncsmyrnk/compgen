#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdbool.h>

typedef struct ArgParser ArgParser;

ArgParser *argparse_new(const char *app_description);
void argparse_free(ArgParser *parser);

// Use '\0' for short_name if you only want a long version.
void argparse_add_bool(ArgParser *parser, char short_name,
                       const char *long_name, const char *help);
void argparse_add_str(ArgParser *parser, char short_name, const char *long_name,
                      const char *help, const char *default_val);

void argparse_add_str_choices(ArgParser *parser, char short_name,
                              const char *long_name, const char *help,
                              const char *default_val, const char **choices);

// Returns false if there was a syntax error in the provided arguments
bool argparse_parse(ArgParser *parser, int argc, char **argv);

// The Search/Query API
bool argparse_get_bool(ArgParser *parser, const char *long_name);
const char *argparse_get_str(ArgParser *parser, const char *long_name);

// Positional Arguments Retrieval
int argparse_positional_count(ArgParser *parser);
const char *argparse_positional_at(ArgParser *parser, int index);

// Utilities
void argparse_print_help(ArgParser *parser);

#endif // ARGPARSE_H
