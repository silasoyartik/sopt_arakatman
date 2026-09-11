#ifdef GS_GLES31_API_HEADER
#include GS_GLES31_API_HEADER
#else
#include <GLES3/gl31.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirement: GS-GLES31-CS-BBB-001
 * Compile/link the exact API signature; do not substitute a wrapper.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GLES31_CS_BBB_TC_001";
static const char *test_procedure = "GS_GLES31_CS_BBB_TP_001";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GLES31_CS_BBB_TP_001_init(void)
{
    /* Compile/link check: exact standard API signature, no GL objects needed.
     * A dynamic loader, if used, must already have resolved the entry point.
     */
    PFNGLBINDBUFFERBASEPROC volatile entry = glBindBufferBase;
    test_result = 0;
    if (entry == NULL)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "glBindBufferBase entry point unavailable");
        return;
    }
    test_result = 1;
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_GLES31_CS_BBB_TP_001_draw(void) {}
void GS_GLES31_CS_BBB_TP_001_close(void) {}
int GS_GLES31_CS_BBB_TP_001_result(void) { return test_result; }
