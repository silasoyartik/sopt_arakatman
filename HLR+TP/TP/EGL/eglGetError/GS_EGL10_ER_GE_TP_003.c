#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - Errors - eglGetError
 * Covered requirement: GS-EGL10-ER-GE-003
 */
static const char* test_case = "GS_EGL10_ER_GE_TC_003";
static const char* test_procedure = "GS_EGL10_ER_GE_TP_003";

void GS_EGL10_ER_GE_TP_003_init(void)
{
    EGLDisplay display;
    EGLBoolean result;
    EGLint config_count = 0;
    EGLint error;

    /* Initialize and terminate the display during setup so its state is
     * known. The operation under test is then made on that valid but
     * deliberately uninitialized EGLDisplay.
     */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY ||
        eglInitialize(display, NULL, NULL) != EGL_TRUE ||
        eglTerminate(display) != EGL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Could not prepare an uninitialized EGLDisplay, error: 0x%x",
            eglGetError());
        return;
    }

    (void)eglGetError();
    result = eglGetConfigs(display, NULL, 0, &config_count);
    error = eglGetError();

    if (result != EGL_FALSE || error != EGL_NOT_INITIALIZED)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_FALSE/EGL_NOT_INITIALIZED, got %u/0x%x",
            (unsigned int)result, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_ER_GE_TP_003_draw(void) { }
void GS_EGL10_ER_GE_TP_003_close(void) { }
