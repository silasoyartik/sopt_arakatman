#ifndef GS_TEST_MACROS_H
#define GS_TEST_MACROS_H

#include <stdio.h>

/* The format string is part of __VA_ARGS__, so a message with no format
 * arguments is valid C99/C11 without the GNU comma-swallowing extension.
 */
#define TEST_LOG_INFO(...) do { \
    printf("[INFO   ] "); \
    printf(__VA_ARGS__); \
    printf("\n\r"); \
} while (0)

#define TEST_LOG_SUCCESS(test_case, test_procedure) \
    printf("[SUCCESS][ %s ][ %s ] Test successful.\n\r", \
        (test_case), (test_procedure))

#define TEST_LOG_FAIL(test_case, test_procedure, ...) do { \
    printf("[FAIL   ][ %s ][ %s : line %d ] Test failed: ", \
        (test_case), (test_procedure), __LINE__); \
    printf(__VA_ARGS__); \
    printf("\n\r"); \
} while (0)

#endif
