#include "../src/ast.h"
#include "../src/kdl_parser.h"
#include "../src/shell.h"
#include "../src/string_builder.h"
#include "test.h"

TEST(zsh_use_case_one) {
    ParseResult r = kdl_parse_file("./test/fixtures/zsh/basic/input.kdl");
    ASSERT_NOT_NULL(r.ast);
    ASSERT_INT_EQ(KDL_RESULT_OK, r.status);

    char *expected_zsh =
        load_file_to_string("./test/fixtures/zsh/basic/expected.zsh");
    ASSERT_NOT_NULL(expected_zsh);

    StringBuffer out = sb_create();
    generate(ast_root(r.ast), &out);

    ASSERT_SNAPSHOT_EQ(expected_zsh, out.data,
                       "test/fixtures/basic.actual.zsh");

    sb_free(&out);
    kdl_free_result(&r);
}
