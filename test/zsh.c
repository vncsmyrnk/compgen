#include "../src/parser.h"
#include "../src/shell.h"
#include "../src/string_builder.h"
#include "test.h"

TEST(zsh_subcommands) {
    ParseResult r = parse_file("./test/fixtures/subcommands/input.kdl");
    ASSERT_NOT_NULL(r.ast);
    ASSERT_INT_EQ(PARSER_RESULT_OK, r.status);

    char *expected_zsh =
        load_file_to_string("./test/fixtures/subcommands/expected.zsh");
    ASSERT_NOT_NULL(expected_zsh);

    StringBuffer out = sb_create();
    generate(r.ast, &out);

    ASSERT_SNAPSHOT_EQ(expected_zsh, out.data,
                       "./test/fixtures/subcommands.actual.zsh");

    free(expected_zsh);
    sb_free(&out);
    free_result(&r);
}

TEST(zsh_arg_types) {
    ParseResult r = parse_file("./test/fixtures/arg-types/input.kdl");
    ASSERT_NOT_NULL(r.ast);
    ASSERT_INT_EQ(PARSER_RESULT_OK, r.status);

    char *expected_zsh =
        load_file_to_string("./test/fixtures/arg-types/expected.zsh");
    ASSERT_NOT_NULL(expected_zsh);

    StringBuffer out = sb_create();
    generate(r.ast, &out);

    ASSERT_SNAPSHOT_EQ(expected_zsh, out.data,
                       "./test/fixtures/arg-types.actual.zsh");

    free(expected_zsh);
    sb_free(&out);
    free_result(&r);
}

TEST(zsh_flag_types) {
    ParseResult r = parse_file("./test/fixtures/flag-types/input.kdl");
    ASSERT_NOT_NULL(r.ast);
    ASSERT_INT_EQ(PARSER_RESULT_OK, r.status);

    char *expected_zsh =
        load_file_to_string("./test/fixtures/flag-types/expected.zsh");
    ASSERT_NOT_NULL(expected_zsh);

    StringBuffer out = sb_create();
    generate(r.ast, &out);

    ASSERT_SNAPSHOT_EQ(expected_zsh, out.data,
                       "./test/fixtures/flag-types.actual.zsh");

    free(expected_zsh);
    sb_free(&out);
    free_result(&r);
}
