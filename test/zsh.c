#include "../src/ast.h"
#include "../src/kdl_parser.h"
#include "../src/shell.h"
#include "../src/string_builder.h"
#include "test.h"

TEST(zsh_use_case_default) {
    ParseResult r = kdl_parse_file("./test/fixtures/basic/input.kdl");
    ASSERT_NOT_NULL(r.ast);
    ASSERT_INT_EQ(KDL_RESULT_OK, r.status);

    char *expected_zsh =
        load_file_to_string("./test/fixtures/basic/expected.zsh");
    ASSERT_NOT_NULL(expected_zsh);

    StringBuffer out = sb_create();
    generate(r.ast, &out);

    ASSERT_SNAPSHOT_EQ(expected_zsh, out.data,
                       "./test/fixtures/basic.actual.zsh");

    free(expected_zsh);
    sb_free(&out);
    kdl_free_result(&r);
}

TEST(zsh_global_flags) {
    ParseResult r = kdl_parse_file("./test/fixtures/global-flags/input.kdl");
    ASSERT_NOT_NULL(r.ast);
    ASSERT_INT_EQ(KDL_RESULT_OK, r.status);

    char *expected_zsh =
        load_file_to_string("./test/fixtures/global-flags/expected.zsh");
    ASSERT_NOT_NULL(expected_zsh);

    StringBuffer out = sb_create();
    generate(r.ast, &out);

    ASSERT_SNAPSHOT_EQ(expected_zsh, out.data,
                       "./test/fixtures/global-flags.actual.zsh");

    free(expected_zsh);
    sb_free(&out);
    kdl_free_result(&r);
}
