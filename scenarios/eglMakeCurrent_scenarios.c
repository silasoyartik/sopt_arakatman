#include <EGL/egl.h>
#include <stdint.h>
#include <stdio.h>

/*
 * All-in-one scenario file for eglMakeCurrent(dpy, draw, read, ctx).
 * Surfaces and contexts are created separately; the functions receive the
 * required fixture objects as parameters. Preconditions for thread/resource
 * errors are described in the relevant scenario comments.
 */

static const char *egl_error_name(EGLint error)
{
    switch (error) {
    case EGL_SUCCESS:            return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED:    return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:         return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:          return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:      return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONFIG:         return "EGL_BAD_CONFIG";
    case EGL_BAD_CONTEXT:        return "EGL_BAD_CONTEXT";
    case EGL_BAD_CURRENT_SURFACE:return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_DISPLAY:        return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:          return "EGL_BAD_MATCH";
    case EGL_BAD_NATIVE_PIXMAP:  return "EGL_BAD_NATIVE_PIXMAP";
    case EGL_BAD_NATIVE_WINDOW:  return "EGL_BAD_NATIVE_WINDOW";
    case EGL_BAD_PARAMETER:      return "EGL_BAD_PARAMETER";
    case EGL_BAD_SURFACE:        return "EGL_BAD_SURFACE";
    default:                     return "UNKNOWN_EGL_ERROR";
    }
}

static void run_make_current(const char *name,
                             EGLDisplay dpy,
                             EGLSurface draw,
                             EGLSurface read,
                             EGLContext ctx,
                             EGLint expected_error)
{
    EGLBoolean result;
    EGLBoolean passed;
    EGLint error;

    (void)eglGetError();
    result = eglMakeCurrent(dpy, draw, read, ctx);
    error = eglGetError();

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

/* SCENARIO 1 - Bind the same surface normally for both draw and read. */
void scenario_make_current_same_draw_read(EGLDisplay dpy,
                                          EGLSurface surface,
                                          EGLContext ctx)
{
    run_make_current("Scenario 1 - same draw/read", dpy, surface, surface,
                     ctx, EGL_SUCCESS);
}

/* SCENARIO 2 - Two distinct draw/read surfaces compatible with the context. */
void scenario_make_current_separate_draw_read(EGLDisplay dpy,
                                              EGLSurface draw_surface,
                                              EGLSurface read_surface,
                                              EGLContext ctx)
{
    run_make_current("Scenario 2 - separate draw/read", dpy, draw_surface,
                     read_surface, ctx, EGL_SUCCESS);
}

/* SCENARIO 3 - Release the current context and both surfaces correctly. */
void scenario_make_current_release(EGLDisplay dpy)
{
    run_make_current("Scenario 3 - release", dpy, EGL_NO_SURFACE,
                     EGL_NO_SURFACE, EGL_NO_CONTEXT, EGL_SUCCESS);
}

/* SCENARIO 4 - dpy == EGL_NO_DISPLAY. */
void scenario_make_current_no_display(EGLSurface surface, EGLContext ctx)
{
    run_make_current("Scenario 4 - EGL_NO_DISPLAY", EGL_NO_DISPLAY,
                     surface, surface, ctx, EGL_BAD_DISPLAY);
}

/* SCENARIO 5 - An invalid display handle not created by EGL. */
void scenario_make_current_invalid_display(EGLSurface surface, EGLContext ctx)
{
    EGLDisplay invalid_dpy = (EGLDisplay)(uintptr_t)1;

    run_make_current("Scenario 5 - invalid display", invalid_dpy,
                     surface, surface, ctx, EGL_BAD_DISPLAY);
}

/* SCENARIO 6 - A display obtained with eglGetDisplay but not initialized. */
void scenario_make_current_uninitialized_display(EGLDisplay uninitialized_dpy,
                                                 EGLSurface surface,
                                                 EGLContext ctx)
{
    run_make_current("Scenario 6 - uninitialized display", uninitialized_dpy,
                     surface, surface, ctx, EGL_NOT_INITIALIZED);
}

/* SCENARIO 7 - Using EGL_NO_CONTEXT with actual surfaces is invalid. */
void scenario_make_current_no_context_with_surfaces(EGLDisplay dpy,
                                                    EGLSurface surface)
{
    run_make_current("Scenario 7 - no context with surfaces", dpy, surface,
                     surface, EGL_NO_CONTEXT, EGL_BAD_MATCH);
}

/* SCENARIO 8 - draw == EGL_NO_SURFACE while a context is supplied. */
void scenario_make_current_missing_draw(EGLDisplay dpy,
                                        EGLSurface read_surface,
                                        EGLContext ctx)
{
    run_make_current("Scenario 8 - missing draw", dpy, EGL_NO_SURFACE,
                     read_surface, ctx, EGL_BAD_MATCH);
}

/* SCENARIO 9 - read == EGL_NO_SURFACE while a context is supplied. */
void scenario_make_current_missing_read(EGLDisplay dpy,
                                        EGLSurface draw_surface,
                                        EGLContext ctx)
{
    run_make_current("Scenario 9 - missing read", dpy, draw_surface,
                     EGL_NO_SURFACE, ctx, EGL_BAD_MATCH);
}

/* SCENARIO 10 - Both draw and read are EGL_NO_SURFACE with a context. */
void scenario_make_current_context_without_surfaces(EGLDisplay dpy,
                                                    EGLContext ctx)
{
    run_make_current("Scenario 10 - context without surfaces", dpy,
                     EGL_NO_SURFACE, EGL_NO_SURFACE, ctx, EGL_BAD_MATCH);
}

/* SCENARIO 11 - A draw surface handle not created by EGL. */
void scenario_make_current_invalid_draw(EGLDisplay dpy,
                                        EGLSurface valid_read,
                                        EGLContext ctx)
{
    EGLSurface invalid_draw = (EGLSurface)(uintptr_t)1;

    run_make_current("Scenario 11 - invalid draw", dpy, invalid_draw,
                     valid_read, ctx, EGL_BAD_SURFACE);
}

/* SCENARIO 12 - A read surface handle not created by EGL. */
void scenario_make_current_invalid_read(EGLDisplay dpy,
                                        EGLSurface valid_draw,
                                        EGLContext ctx)
{
    EGLSurface invalid_read = (EGLSurface)(uintptr_t)1;

    run_make_current("Scenario 12 - invalid read", dpy, valid_draw,
                     invalid_read, ctx, EGL_BAD_SURFACE);
}

/* SCENARIO 13 - A non-NULL context handle not created by EGL. */
void scenario_make_current_invalid_context(EGLDisplay dpy,
                                           EGLSurface surface)
{
    EGLContext invalid_ctx = (EGLContext)(uintptr_t)1;

    run_make_current("Scenario 13 - invalid context", dpy, surface, surface,
                     invalid_ctx, EGL_BAD_CONTEXT);
}

/*
 * SCENARIO 14 - The draw surface's color/ancillary buffer layout is incompatible
 * with ctx. incompatible_draw must be created from a different, incompatible
 * config on the same display.
 */
void scenario_make_current_incompatible_draw(EGLDisplay dpy,
                                             EGLSurface incompatible_draw,
                                             EGLSurface compatible_read,
                                             EGLContext ctx)
{
    run_make_current("Scenario 14 - incompatible draw", dpy,
                     incompatible_draw, compatible_read, ctx, EGL_BAD_MATCH);
}

/*
 * SCENARIO 15 - The read surface's color/ancillary buffer layout is incompatible
 * with ctx.
 */
void scenario_make_current_incompatible_read(EGLDisplay dpy,
                                             EGLSurface compatible_draw,
                                             EGLSurface incompatible_read,
                                             EGLContext ctx)
{
    run_make_current("Scenario 15 - incompatible read", dpy, compatible_draw,
                     incompatible_read, ctx, EGL_BAD_MATCH);
}

/*
 * SCENARIO 16 - The objects do not belong to the same display.
 * foreign_surface must have been created with another EGLDisplay.
 */
void scenario_make_current_surface_from_another_display(
    EGLDisplay dpy,
    EGLSurface foreign_surface,
    EGLContext ctx)
{
    run_make_current("Scenario 16 - surface from another display", dpy,
                     foreign_surface, foreign_surface, ctx, EGL_BAD_MATCH);
}

/*
 * SCENARIO 17 - The context does not belong to this display.
 * foreign_ctx must have been created with another EGLDisplay.
 */
void scenario_make_current_context_from_another_display(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext foreign_ctx)
{
    run_make_current("Scenario 17 - context from another display", dpy,
                     surface, surface, foreign_ctx, EGL_BAD_MATCH);
}

/*
 * SCENARIO 18 - The context is current on another thread.
 * The fixture must call this function on another thread while keeping ctx
 * current on a worker thread. EGL_BAD_ACCESS is expected before worker release.
 */
void scenario_make_current_context_current_on_another_thread(
    EGLDisplay dpy,
    EGLSurface surface,
    EGLContext ctx)
{
    run_make_current("Scenario 18 - context owned by another thread", dpy,
                     surface, surface, ctx, EGL_BAD_ACCESS);
}

/*
 * SCENARIO 19 - The draw surface is bound to a context on another thread.
 * Here, ctx must be a second, unused context compatible with the surface.
 */
void scenario_make_current_draw_owned_by_another_thread(
    EGLDisplay dpy,
    EGLSurface busy_draw,
    EGLSurface free_read,
    EGLContext ctx)
{
    run_make_current("Scenario 19 - draw owned by another thread", dpy,
                     busy_draw, free_read, ctx, EGL_BAD_ACCESS);
}

/* SCENARIO 20 - The read surface is bound to a context on another thread. */
void scenario_make_current_read_owned_by_another_thread(
    EGLDisplay dpy,
    EGLSurface free_draw,
    EGLSurface busy_read,
    EGLContext ctx)
{
    run_make_current("Scenario 20 - read owned by another thread", dpy,
                     free_draw, busy_read, ctx, EGL_BAD_ACCESS);
}

/*
 * SCENARIO 21 - A window surface whose underlying native window was destroyed.
 * EGL 1.0 does not guarantee detection of an invalid native object on every platform.
 */
void scenario_make_current_destroyed_native_window(EGLDisplay dpy,
                                                   EGLSurface affected_surface,
                                                   EGLContext ctx)
{
    run_make_current("Scenario 21 - destroyed native window", dpy,
                     affected_surface, affected_surface, ctx,
                     EGL_BAD_NATIVE_WINDOW);
}

/*
 * SCENARIO 22 - Flush the old context while the previous current surface is invalid.
 * The fixture must first destroy the current surface's native object and leave
 * unflushed GL commands. This condition is driver/platform dependent.
 */
void scenario_make_current_bad_previous_current_surface(
    EGLDisplay dpy,
    EGLSurface new_surface,
    EGLContext new_ctx)
{
    run_make_current("Scenario 22 - bad previous current surface", dpy,
                     new_surface, new_surface, new_ctx,
                     EGL_BAD_CURRENT_SURFACE);
}

/*
 * SCENARIO 23 - Ancillary buffer allocation failure.
 * The fixture/driver must force draw/read buffer allocation to fail in a
 * controlled way (for example, through fault injection). Normal code cannot
 * reliably produce this condition.
 */
void scenario_make_current_ancillary_allocation_failure(
    EGLDisplay dpy,
    EGLSurface draw,
    EGLSurface read,
    EGLContext ctx)
{
    run_make_current("Scenario 23 - ancillary allocation failure", dpy,
                     draw, read, ctx, EGL_BAD_ALLOC);
}

/*
 * SCENARIO 24 - Draw/read cannot be supported together or fit in memory.
 * Driver fault injection or a fixture that genuinely creates this condition is required.
 */
void scenario_make_current_draw_read_resource_mismatch(
    EGLDisplay dpy,
    EGLSurface draw,
    EGLSurface read,
    EGLContext ctx)
{
    run_make_current("Scenario 24 - draw/read resource mismatch", dpy,
                     draw, read, ctx, EGL_BAD_MATCH);
}

/*
 * SCENARIO 25 - Switch the current binding to another compatible context/surface.
 * A successful call flushes the old context and binds the new tuple to the thread.
 */
void scenario_make_current_switch_binding(EGLDisplay dpy,
                                          EGLSurface old_surface,
                                          EGLContext old_ctx,
                                          EGLSurface new_surface,
                                          EGLContext new_ctx)
{
    run_make_current("Scenario 25a - establish old binding", dpy, old_surface,
                     old_surface, old_ctx, EGL_SUCCESS);
    run_make_current("Scenario 25b - switch binding", dpy, new_surface,
                     new_surface, new_ctx, EGL_SUCCESS);
}

/*
 * SCENARIO 26 - A previously destroyed draw surface that is no longer current.
 * destroyed_draw's lifetime must have ended after eglDestroySurface.
 */
void scenario_make_current_destroyed_draw(EGLDisplay dpy,
                                          EGLSurface destroyed_draw,
                                          EGLSurface valid_read,
                                          EGLContext ctx)
{
    run_make_current("Scenario 26 - destroyed draw", dpy, destroyed_draw,
                     valid_read, ctx, EGL_BAD_SURFACE);
}

/* SCENARIO 27 - A previously destroyed read surface that is no longer current. */
void scenario_make_current_destroyed_read(EGLDisplay dpy,
                                          EGLSurface valid_draw,
                                          EGLSurface destroyed_read,
                                          EGLContext ctx)
{
    run_make_current("Scenario 27 - destroyed read", dpy, valid_draw,
                     destroyed_read, ctx, EGL_BAD_SURFACE);
}
