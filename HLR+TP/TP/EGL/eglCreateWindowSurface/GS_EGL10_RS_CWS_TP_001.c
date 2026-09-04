#include <EGL/egl.h>
#include "../../macros.h"

/* EGL10 - RenderingSurfaces - eglCreateWindowSurface
 * Covered requirement: GS-EGL10-RS-CWS-001
 */
static const char* test_case = "GS_EGL10_RS_CWS_TC_001";
static const char* test_procedure = "GS_EGL10_RS_CWS_TP_001";

void GS_EGL10_RS_CWS_TP_001_init(void)
{
    EGLSurface (EGLAPIENTRYP create_window_surface_function)(EGLDisplay,
        EGLConfig, EGLNativeWindowType, const EGLint*) =
        eglCreateWindowSurface;

    /* Assign the API symbol to a function pointer with the specified EGL
     * interface. Compilation and linking verify that the library exports the
     * function with compatible parameter and return types.
     */
    (void)create_window_surface_function;
    TEST_LOG_SUCCESS(test_case, test_procedure);
}

void GS_EGL10_RS_CWS_TP_001_draw(void) { }
void GS_EGL10_RS_CWS_TP_001_close(void) { }
