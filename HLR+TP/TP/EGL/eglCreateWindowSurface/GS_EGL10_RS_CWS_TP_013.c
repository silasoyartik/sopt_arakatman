#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-013
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_013";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_013";

void GS_EGL10_RS_CWS_TP_013_init(void)
{
    EGLSurface surface;
    EGLint error;

    /* EGL_NO_DISPLAY cannot name a valid EGLDisplay. Because display
     * validation fails first, this test needs no native-window fixture and
     * can directly verify the required return value and EGL error.
     */
    (void)eglGetError();
    surface = eglCreateWindowSurface(EGL_NO_DISPLAY, (EGLConfig)0,
        (EGLNativeWindowType)0, NULL);
    error = eglGetError();

    if (surface != EGL_NO_SURFACE || error != EGL_BAD_DISPLAY)
    {
        TEST_LOG_FAIL(test_case, test_procedure,
            "Expected EGL_NO_SURFACE/EGL_BAD_DISPLAY, got %p/0x%x",
            (void*)surface, error);
        return;
    }

    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_RS_CWS_TP_013_draw(void) { }
void GS_EGL10_RS_CWS_TP_013_close(void) { }
