#include <EGL/egl.h>

/*
 * eglGetCurrentContext(void)
 *
 * Bu dosya eglGetCurrentContext icin temel senaryolari tek yerde toplar.
 * EGLDisplay, EGLSurface ve EGLContext'in daha once basarili sekilde
 * olusturuldugu varsayilir.
 */

/*
 * SENARYO A - Aktif context varken eglGetCurrentContext cagrisi
 *
 * Context current hale getirildikten sonra eglGetCurrentContext()
 * cagrisinin ayni context handle degerini dondurmesi beklenir.
 */
void scenario_a_current_context_is_returned(EGLDisplay display,
                                            EGLSurface surface,
                                            EGLContext context)
{
    EGLBoolean make_current_result;
    EGLContext current_context;

    make_current_result = eglMakeCurrent(display, surface, surface, context);
    if (make_current_result != EGL_TRUE) {
        return;
    }

    current_context = eglGetCurrentContext();

    if (current_context == context) {
    } else if (current_context == EGL_NO_CONTEXT) {
    } else {
    }
}

/*
 * SENARYO B - Aktif context yokken eglGetCurrentContext cagrisi
 *
 * Current context thread'den ayrildiktan sonra eglGetCurrentContext()
 * cagrisinin EGL_NO_CONTEXT dondurmesi beklenir.
 */
void scenario_b_no_current_context_returns_no_context(EGLDisplay display,
                                                      EGLSurface surface,
                                                      EGLContext context)
{
    EGLBoolean make_current_result;
    EGLBoolean detach_result;
    EGLContext current_context;

    make_current_result = eglMakeCurrent(display, surface, surface, context);
    if (make_current_result != EGL_TRUE) {
        return;
    }

    detach_result = eglMakeCurrent(display,
                                   EGL_NO_SURFACE,
                                   EGL_NO_SURFACE,
                                   EGL_NO_CONTEXT);
    if (detach_result != EGL_TRUE) {
        return;
    }

    current_context = eglGetCurrentContext();

    if (current_context == EGL_NO_CONTEXT) {
    } else {
    }
}
