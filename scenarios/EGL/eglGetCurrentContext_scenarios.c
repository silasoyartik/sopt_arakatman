#include <EGL/egl.h>

/*
 * eglGetCurrentContext(void)
 *
 * This file collects the basic eglGetCurrentContext scenarios in one place.
 * EGLDisplay, EGLSurface, and EGLContext are assumed to have been created
 * successfully beforehand.
 */

/*
 * SCENARIO A - Call eglGetCurrentContext with an active context
 *
 * After the context is made current, eglGetCurrentContext() is expected to
 * return the same context handle.
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
 * SCENARIO B - Call eglGetCurrentContext with no active context
 *
 * After the current context is detached from the thread,
 * eglGetCurrentContext() is expected to return EGL_NO_CONTEXT.
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
