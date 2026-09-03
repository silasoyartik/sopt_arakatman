#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-010
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_010";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_010";

void GS_EGL10_ER_GE_TP_010_init(void)
{
    EGLBoolean result;
    EGLint error;

    /* EGL_NO_DISPLAY is never a valid EGLDisplay argument for eglInitialize.
     * The failed call deterministically creates EGL_BAD_DISPLAY without any
     * EGL fixture or object supplied by main.
     */
    (void)eglGetError();
    result = eglInitialize(EGL_NO_DISPLAY, NULL, NULL);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_BAD_DISPLAY)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_BAD_DISPLAY, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_010_draw(void) { }
void GS_EGL10_ER_GE_TP_010_close(void) { }
