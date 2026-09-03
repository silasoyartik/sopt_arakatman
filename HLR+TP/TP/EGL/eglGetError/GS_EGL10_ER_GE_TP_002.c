#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-002
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_002";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_002";

void GS_EGL10_ER_GE_TP_002_init(void)
{
    EGLint error;

    /* Discard a possible error left by an earlier EGL call. Then execute a
     * valid query which does not require a current EGL context or surface.
     * eglGetError shall report success for this calling thread.
     */
    (void)eglGetError();
    (void)eglGetCurrentDisplay();
    error = eglGetError();

    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_SUCCESS, got 0x%x", error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_002_draw(void) { }
void GS_EGL10_ER_GE_TP_002_close(void) { }
