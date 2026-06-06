#define _POSIX_C_SOURCE 200112L
#include "../src/parser.h"
#include "test.h"
#include <stdio.h>

void write_test_kdl(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    fprintf(f, "%s", content);
    fclose(f);
}

TEST(parser_validation_invalid_node) {
    const char *path = "test_invalid_node.kdl";
    write_test_kdl(path, "name \"tool\"\nunknown_node \"test\"\n");

    ParseResult res = parse_file(path);
    ASSERT_INT_EQ(PARSER_RESULT_ERR_VALIDATION_FAILED, res.status);
    ASSERT_NOT_NULL(res.error_message);
    ASSERT_STR_EQ("Error at line 2: Invalid node type 'unknown_node'.",
                  res.error_message);

    free_result(&res);
    remove(path);
}

TEST(parser_node_limit_respected) {
    const char *test_kdl = "test_limit.kdl";
    FILE *f = fopen(test_kdl, "w");
    fprintf(f, "cmd \"test\" {\n");
    for (int i = 0; i < 10; i++) {
        fprintf(f, "  flag \"--f%d\"\n", i);
    }
    fprintf(f, "}\n");
    fclose(f);

    // Test with limit higher than node count
    setenv("COMPGEN_MAX_NODES", "100", 1);
    ParseResult res1 = parse_file(test_kdl);
    ASSERT_INT_EQ(PARSER_RESULT_OK, res1.status);
    free_result(&res1);

    // Total should be 11 nodes.
    setenv("COMPGEN_MAX_NODES", "5", 1);
    ParseResult res2 = parse_file(test_kdl);
    ASSERT_INT_EQ(PARSER_RESULT_ERR_TOO_MANY_NODES, res2.status);
    free_result(&res2);

    // Test with default limit (200)
    unsetenv("COMPGEN_MAX_NODES");
    ParseResult res3 = parse_file(test_kdl);
    ASSERT_INT_EQ(PARSER_RESULT_OK, res3.status);
    free_result(&res3);

    remove(test_kdl);
}
