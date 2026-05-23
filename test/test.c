#include "test.h"

TestFunc test_registry[MAX_TESTS];
const char* test_names[MAX_TESTS];
int test_count = 0;

int main(void) {
    printf("=== Starting %d Unit Tests ===\n", test_count);

    for (int i = 0; i < test_count; i++) {
        printf("RUNNING: %s... ", test_names[i]);
        test_registry[i]();
        printf("OK\n");
    }

    printf("=== All tests passed successfully! ===\n");
    return 0;
}
