#include "test.h"
#include <stdio.h>

TestFunc test_registry[MAX_TESTS];
const char *test_names[MAX_TESTS];
int test_count = 0;

char *load_file_to_string(const char *filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, f);
        buffer[length] = '\0';
    }
    fclose(f);
    return buffer;
}

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
