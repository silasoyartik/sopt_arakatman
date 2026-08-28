#define TEST_LOG_INFO(fmt, ...)                             printf("[INFO   ] " fmt "\n\r", ##__VA_ARGS__)
#define TEST_LOG_SUCCESS(test_case, test_procedure)         printf("[SUCCESS][ %s ][ %s ] Test successful.\n\r", (char*)test_case, (char*)test_procedure)
#define TEST_LOG_FAIL(test_case, test_procedure, fmt, ...)  printf("[FAIL   ][ %s ][ %s : line %d ] Test failed: " fmt "\n\r", \
																												 (char*)test_case, (char*)test_procedure, __LINE__, ##__VA_ARGS__)