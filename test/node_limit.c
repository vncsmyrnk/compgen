#define _POSIX_C_SOURCE 200112L
#include "../src/parser.h"
#include "test.h"
#include <stdlib.h>

TEST(test_kdl_node_limit_respected) {
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
