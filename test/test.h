#include <assert.h>
#include <string.h>
#include <stdio.h>

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

typedef void (*TestFunc)(void);

#ifndef TEST_H
#define TEST_H

#define MAX_TESTS 256
extern TestFunc test_registry[MAX_TESTS];
extern const char* test_names[MAX_TESTS];
extern int test_count;

// --- The Meta-Programming Constructor Macro ---
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

#endif // TEST_H
