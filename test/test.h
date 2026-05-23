#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MAX_TESTS
#define MAX_TESTS 256
#endif // MAX_TESTS

#ifndef ASSERT_STR_EQ
#define ASSERT_STR_EQ(expected, actual)                                        \
    do {                                                                       \
        const char *exp = (expected);                                          \
        const char *act = (actual);                                            \
        if (strcmp(exp, act) != 0) {                                           \
            fprintf(                                                           \
                stderr,                                                        \
                "\n[FAIL] %s:%d\n  Expected: \"%s\"\n  Actual:   \"%s\"\n",    \
                __FILE__, __LINE__, exp, act);                                 \
            assert(0); /* Force the abort */                                   \
        }                                                                      \
    } while (0)
#endif // ASSERT_STR_EQ

#ifndef ASSERT_NOT_NULL
#define ASSERT_NOT_NULL(ptr)                                                   \
    do {                                                                       \
        if ((ptr) == NULL) {                                                   \
            fprintf(stderr,                                                    \
                    "\n[FAIL] %s:%d\n  Pointer '%s' was unexpectedly NULL.\n", \
                    __FILE__, __LINE__, #ptr);                                 \
            assert(0);                                                         \
        }                                                                      \
    } while (0)
#endif // ASSERT_NOT_NULL

#ifndef ASSERT_INT_EQ
#define ASSERT_INT_EQ(expected, actual) \
    do { \
        int exp = (int)(expected); \
        int act = (int)(actual); \
        if (exp != act) { \
            fprintf(stderr, "\n[FAIL] %s:%d\n  Expected: %d\n  Actual:   %d\n", \
                    __FILE__, __LINE__, exp, act); \
            exit(1); \
        } \
    } while(0)
#endif // ASSERT_INT_EQ

#ifndef ASSERT_SNAPSHOT_EQ
#define ASSERT_SNAPSHOT_EQ(expected_str, actual_str, out_fail_path) \
    do { \
        if (strcmp((expected_str), (actual_str)) != 0) { \
            FILE *f = fopen((out_fail_path), "wb"); \
            if (f) { \
                fwrite((actual_str), 1, strlen(actual_str), f); \
                fclose(f); \
            } \
            fprintf(stderr, "\n[SNAPSHOT FAIL] %s:%d\n", __FILE__, __LINE__); \
            fprintf(stderr, "  Output differed from golden file.\n"); \
            fprintf(stderr, "  Wrote actual output to: %s\n", out_fail_path); \
            fprintf(stderr, "  Run 'diff <expected_file> %s' to see changes.\n", out_fail_path); \
            exit(1); \
        } \
    } while(0)
#endif // ASSERT_SNAPSHOT_EQ

#ifndef TEST_H
#define TEST_H

typedef void (*TestFunc)(void);
extern TestFunc test_registry[MAX_TESTS];
extern const char* test_names[MAX_TESTS];
extern int test_count;

char* load_file_to_string(const char *filepath);

#ifndef TEST
#define TEST(name) \
    void name(void); \
    __attribute__((constructor)) void register_##name(void) { \
        if (test_count < MAX_TESTS) { \
            test_registry[test_count] = name; \
            test_names[test_count] = #name; \
            test_count++; \
        } \
    } \
    void name(void)
#endif // TEST

#endif // TEST_H
