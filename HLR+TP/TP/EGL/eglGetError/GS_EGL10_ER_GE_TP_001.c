#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-001
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_001";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_001";

void GS_EGL10_ER_GE_TP_001_init(void)
{
    EGLint (*get_error_function)(void) = eglGetError;
    EGLBoolean result;
    EGLint error;

    /* Generate a known error immediately before calling eglGetError.
     * The typed function pointer also verifies the required API interface.
     * No display or other fixture is supplied by main.
     */
    (void)eglGetError();
    result = eglInitialize(EGL_NO_DISPLAY, NULL, NULL);
    error = get_error_function();

    if (result != EGL_FALSE || error != EGL_BAD_DISPLAY)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_DISPLAY, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_001_draw(void) { }
void GS_EGL10_ER_GE_TP_001_close(void) { }
