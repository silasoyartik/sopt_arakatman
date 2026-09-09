#include <EGL/egl.h>
#include <stdint.h>
#include <stdio.h>

/*
 * All-in-one scenario file for eglSwapBuffers(dpy, surface).
 *
 * Because window/pixmap creation is platform dependent, the fixture passes
 * these objects as parameters. Each success scenario expects an initialized
 * display, a compatible context, and a surface of the specified type.
 */

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:           return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED:   return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:        return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:         return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:     return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONFIG:        return "EGL_BAD_CONFIG";
    case EGL_BAD_CONTEXT:       return "EGL_BAD_CONTEXT";
    case EGL_BAD_CURRENT_SURFACE:return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_DISPLAY:       return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:         return "EGL_BAD_MATCH";
    case EGL_BAD_NATIVE_PIXMAP: return "EGL_BAD_NATIVE_PIXMAP";
    case EGL_BAD_NATIVE_WINDOW: return "EGL_BAD_NATIVE_WINDOW";
    case EGL_BAD_PARAMETER:     return "EGL_BAD_PARAMETER";
    case EGL_BAD_SURFACE:       return "EGL_BAD_SURFACE";
    default:                    return "UNKNOWN_EGL_ERROR";
    }
}

static void report_swap(const char *name, EGLBoolean result,
                        EGLint expected_error)
{
    EGLint error = eglGetError();
    EGLBoolean passed;

    if (expected_error == EGL_SUCCESS) {
        passed = (result == EGL_TRUE && error == EGL_SUCCESS);
    } else {
        passed = (result == EGL_FALSE && error == expected_error);
    }

    printf("%s: %s (result=%s, error=%s)\n",
           name,
           passed ? "PASS" : "FAIL",
           result == EGL_TRUE ? "EGL_TRUE" : "EGL_FALSE",
           egl_error_name(error));
}

static EGLBoolean make_surface_current(EGLDisplay dpy, EGLSurface surface,
                                       EGLContext ctx, const char *name)
{
    (void)eglGetError();
    if (eglMakeCurrent(dpy, surface, surface, ctx) == EGL_FALSE) {
        printf("%s setup failed: eglMakeCurrent error=%s\n",
               name, egl_error_name(eglGetError()));
        return EGL_FALSE;
    }
    return EGL_TRUE;
}

/* SCENARIO 1 - Current window surface: post the color buffer to the native window. */
void scenario_swap_current_window_surface(EGLDisplay dpy,
                                          EGLSurface window_surface,
                                          EGLContext ctx)
{
    if (!make_surface_current(dpy, window_surface, ctx, "Scenario 1"))
        return;

    (void)eglGetError();
    report_swap("Scenario 1 - current window surface",
                eglSwapBuffers(dpy, window_surface), EGL_SUCCESS);
}

/*
 * SCENARIO 2 - Current pbuffer surface.
 * According to EGL 1.0, the call succeeds but performs no visible post operation.
 */
void scenario_swap_current_pbuffer_no_effect(EGLDisplay dpy,
                                             EGLSurface pbuffer_surface,
                                             EGLContext ctx)
{
    if (!make_surface_current(dpy, pbuffer_surface, ctx, "Scenario 2"))
        return;

    (void)eglGetError();
    report_swap("Scenario 2 - current pbuffer (no effect)",
                eglSwapBuffers(dpy, pbuffer_surface), EGL_SUCCESS);
}

/*
 * SCENARIO 3 - Current pixmap surface.
 * According to EGL 1.0, the call succeeds but does not copy to the pixmap.
 */
void scenario_swap_current_pixmap_no_effect(EGLDisplay dpy,
                                            EGLSurface pixmap_surface,
                                            EGLContext ctx)
{
    if (!make_surface_current(dpy, pixmap_surface, ctx, "Scenario 3"))
        return;

    (void)eglGetError();
    report_swap("Scenario 3 - current pixmap (no effect)",
                eglSwapBuffers(dpy, pixmap_surface), EGL_SUCCESS);
}

/*
 * SCENARIO 4 - A valid but non-current surface.
 * current_surface and non_current_surface must be distinct, compatible objects.
 */
void scenario_swap_valid_but_non_current_surface(
    EGLDisplay dpy,
    EGLSurface current_surface,
    EGLSurface non_current_surface,
    EGLContext ctx)
{
    if (!make_surface_current(dpy, current_surface, ctx, "Scenario 4"))
        return;

    (void)eglGetError();
    report_swap("Scenario 4 - non-current surface",
                eglSwapBuffers(dpy, non_current_surface), EGL_BAD_SURFACE);
}

/* SCENARIO 5 - surface == EGL_NO_SURFACE. */
void scenario_swap_no_surface(EGLDisplay dpy)
{
    (void)eglGetError();
    report_swap("Scenario 5 - EGL_NO_SURFACE",
                eglSwapBuffers(dpy, EGL_NO_SURFACE), EGL_BAD_SURFACE);
}

/*
 * SCENARIO 6 - An invalid, non-NULL handle not created by EGL.
 * A fabricated handle must only be used in a controlled negative API test.
 */
void scenario_swap_invalid_surface(EGLDisplay dpy)
{
    EGLSurface invalid_surface = (EGLSurface)(uintptr_t)1;

    (void)eglGetError();
    report_swap("Scenario 6 - invalid surface",
                eglSwapBuffers(dpy, invalid_surface), EGL_BAD_SURFACE);
}

/*
 * SCENARIO 7 - Mark a current surface for deletion with eglDestroySurface.
 * The surface remains bound, so it is not destroyed immediately and the swap
 * should still succeed.
 */
void scenario_swap_destroyed_but_still_current(EGLDisplay dpy,
                                               EGLSurface surface,
                                               EGLContext ctx)
{
    if (!make_surface_current(dpy, surface, ctx, "Scenario 7"))
        return;

    if (eglDestroySurface(dpy, surface) == EGL_FALSE) {
        printf("Scenario 7 setup failed: eglDestroySurface error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    (void)eglGetError();
    report_swap("Scenario 7 - destroyed but still current",
                eglSwapBuffers(dpy, surface), EGL_SUCCESS);

    (void)eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

/*
 * SCENARIO 8 - Use a deferred-destruction surface after releasing it.
 * Release completes the actual destruction; the stale handle should now
 * produce EGL_BAD_SURFACE.
 */
void scenario_swap_destroyed_and_released_surface(EGLDisplay dpy,
                                                  EGLSurface surface,
                                                  EGLContext ctx)
{
    if (!make_surface_current(dpy, surface, ctx, "Scenario 8"))
        return;
    if (eglDestroySurface(dpy, surface) == EGL_FALSE) {
        printf("Scenario 8 setup failed: eglDestroySurface error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }
    if (eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE,
                       EGL_NO_CONTEXT) == EGL_FALSE) {
        printf("Scenario 8 setup failed: release error=%s\n",
               egl_error_name(eglGetError()));
        return;
    }

    (void)eglGetError();
    report_swap("Scenario 8 - destroyed and released surface",
                eglSwapBuffers(dpy, surface), EGL_BAD_SURFACE);
}

/* SCENARIO 9 - dpy == EGL_NO_DISPLAY. */
void scenario_swap_no_display(EGLSurface surface)
{
    (void)eglGetError();
    report_swap("Scenario 9 - EGL_NO_DISPLAY",
                eglSwapBuffers(EGL_NO_DISPLAY, surface), EGL_BAD_DISPLAY);
}

/* SCENARIO 10 - An invalid display handle not created by EGL. */
void scenario_swap_invalid_display(EGLSurface surface)
{
    EGLDisplay invalid_dpy = (EGLDisplay)(uintptr_t)1;

    (void)eglGetError();
    report_swap("Scenario 10 - invalid display",
                eglSwapBuffers(invalid_dpy, surface), EGL_BAD_DISPLAY);
}

/*
 * SCENARIO 11 - A valid but uninitialized display.
 * uninitialized_dpy must have been obtained with eglGetDisplay but not initialized.
 */
void scenario_swap_uninitialized_display(EGLDisplay uninitialized_dpy,
                                         EGLSurface surface)
{
    (void)eglGetError();
    report_swap("Scenario 11 - uninitialized display",
                eglSwapBuffers(uninitialized_dpy, surface),
                EGL_NOT_INITIALIZED);
}

/*
 * SCENARIO 12 - A current window surface whose native window is now invalid.
 * The fixture must first make the surface current, then destroy the underlying
 * native window without releasing the EGL surface. If detectable, the expected
 * error is EGL_BAD_NATIVE_WINDOW; EGL 1.0 does not guarantee detection everywhere.
 */
void scenario_swap_destroyed_native_window(EGLDisplay dpy,
                                           EGLSurface affected_surface)
{
    (void)eglGetError();
    report_swap("Scenario 12 - destroyed native window",
                eglSwapBuffers(dpy, affected_surface),
                EGL_BAD_NATIVE_WINDOW);
}

/*
 * SCENARIO 13 - Swap after resizing the native window.
 * The fixture must resize the native window before the call. EGL adjusts the
 * surface to the new size before posting; the application must also update the
 * GL viewport.
 */
void scenario_swap_after_native_window_resize(EGLDisplay dpy,
                                              EGLSurface resized_surface)
{
    (void)eglGetError();
    report_swap("Scenario 13 - resized native window",
                eglSwapBuffers(dpy, resized_surface), EGL_SUCCESS);
}

/*
 * SCENARIO 14 - The surface belongs to another display.
 * Because foreign_surface was not created with dpy, it is not a valid
 * EGLSurface for this display, and EGL_BAD_SURFACE is expected.
 */
void scenario_swap_surface_from_another_display(EGLDisplay dpy,
                                                EGLSurface foreign_surface)
{
    (void)eglGetError();
    report_swap("Scenario 14 - surface from another display",
                eglSwapBuffers(dpy, foreign_surface), EGL_BAD_SURFACE);
}
