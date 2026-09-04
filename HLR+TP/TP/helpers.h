#ifndef GS_EGL10_TEST_HELPERS_H
#define GS_EGL10_TEST_HELPERS_H

#include <EGL/egl.h>
#include "macros.h"

typedef struct
{
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    EGLBoolean initialized;
} GS_EGL10_TestEnvironment;

#define GS_EGL10_ENV_INITIALIZER \
    { EGL_NO_DISPLAY, (EGLConfig)0, EGL_NO_CONTEXT, EGL_NO_SURFACE, EGL_FALSE }

/* CHECK_ERROR expects test_case and test_success in the test source file. */
#define CHECK_ERROR(test_procedure)                                         \
    do                                                                      \
    {                                                                       \
        EGLint gs_egl_check_error = eglGetError();                          \
        if (gs_egl_check_error != EGL_SUCCESS)                              \
        {                                                                   \
            TEST_LOG_FAIL(test_case, test_procedure,                        \
                "Unexpected EGL error: 0x%x", gs_egl_check_error);          \
            test_success = EGL_FALSE;                                       \
        }                                                                   \
    } while (0)

static EGLBoolean GS_EGL10_initialize_display(
    GS_EGL10_TestEnvironment *environment)
{
    environment->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (environment->display == EGL_NO_DISPLAY)
    {
        return EGL_FALSE;
    }

    if (eglInitialize(environment->display, NULL, NULL) != EGL_TRUE)
    {
        return EGL_FALSE;
    }

    environment->initialized = EGL_TRUE;
    return EGL_TRUE;
}

static EGLBoolean GS_EGL10_choose_config(
    GS_EGL10_TestEnvironment *environment,
    EGLint surface_type)
{
    const EGLint attributes[] = {
        EGL_SURFACE_TYPE, surface_type,
        EGL_NONE
    };
    EGLint count = 0;

    return eglChooseConfig(environment->display, attributes,
        &environment->config, 1, &count) == EGL_TRUE && count > 0;
}

/* Creates the objects but deliberately leaves the context non-current. */
static EGLBoolean GS_EGL10_prepare_pbuffer_environment(
    GS_EGL10_TestEnvironment *environment,
    EGLint width,
    EGLint height)
{
    const EGLint pbuffer_attributes[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_NONE
    };

    if (!GS_EGL10_initialize_display(environment) ||
        !GS_EGL10_choose_config(environment, EGL_PBUFFER_BIT))
    {
        return EGL_FALSE;
    }

    environment->context = eglCreateContext(environment->display,
        environment->config, EGL_NO_CONTEXT, NULL);
    if (environment->context == EGL_NO_CONTEXT)
    {
        return EGL_FALSE;
    }

    environment->surface = eglCreatePbufferSurface(environment->display,
        environment->config, pbuffer_attributes);
    return environment->surface != EGL_NO_SURFACE;
}

static EGLBoolean GS_EGL10_make_environment_current(
    GS_EGL10_TestEnvironment *environment)
{
    return eglMakeCurrent(environment->display, environment->surface,
        environment->surface, environment->context);
}

static void GS_EGL10_cleanup_environment(
    GS_EGL10_TestEnvironment *environment)
{
    if (environment->display != EGL_NO_DISPLAY && environment->initialized)
    {
        (void)eglMakeCurrent(environment->display, EGL_NO_SURFACE,
            EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (environment->surface != EGL_NO_SURFACE)
        {
            (void)eglDestroySurface(environment->display,
                environment->surface);
        }

        if (environment->context != EGL_NO_CONTEXT)
        {
            (void)eglDestroyContext(environment->display,
                environment->context);
        }

        (void)eglTerminate(environment->display);
    }

    environment->display = EGL_NO_DISPLAY;
    environment->config = (EGLConfig)0;
    environment->context = EGL_NO_CONTEXT;
    environment->surface = EGL_NO_SURFACE;
    environment->initialized = EGL_FALSE;
}

/*
 * Additional target-adaptation hook interface
 * ===========================================
 *
 * These hooks predate GS_EGL_PLATFORM_TEST_HOOKS and are controlled by the
 * feature switches documented with each declaration.  Like the platform test
 * hooks below, declarations are centralized here while definitions belong in a
 * target-specific .c file.  Getter/release pairs own one fixture at a time.
 */

/*
 * Return a real native display identifier for which eglGetDisplay is expected
 * to return a matching EGLDisplay.  Open any native connection required by the
 * target and retain it until release_valid_native_display.  Release must close
 * only resources owned by this getter and must be safe when eglGetDisplay
 * returned EGL_NO_DISPLAY.  A Wayland implementation is already supplied in
 * GS_EGL10_IN_GD_TP_002 when GS_EGL_USE_WAYLAND is defined; other targets
 * provide the external pair below.
 */
#ifndef GS_EGL_USE_WAYLAND
EGLNativeDisplayType GS_EGL10_get_valid_native_display(void);
void GS_EGL10_release_valid_native_display(
    EGLNativeDisplayType native_display);
#endif

/*
 * Return a platform-defined native display identifier for which no matching
 * EGLDisplay is available.  It must be safe to pass the value to eglGetDisplay
 * and the target must specify that the expected result is EGL_NO_DISPLAY with
 * EGL_SUCCESS; never use a fabricated pointer or arbitrary integer.  Release
 * must dispose of any native fixture state without calling EGL APIs that alter
 * the error result observed by the test.
 */
EGLNativeDisplayType GS_EGL10_get_unmatched_native_display(void);
void GS_EGL10_release_unmatched_native_display(
    EGLNativeDisplayType native_display);

/*
 * Return a genuine EGLDisplay handle which is not EGL_NO_DISPLAY, but arrange
 * through a controlled target condition or fault injector that the following
 * eglInitialize call fails with EGL_NOT_INITIALIZED.  The getter must not call
 * eglInitialize itself.  Release must disable injection, terminate the display
 * if the target call unexpectedly initialized it, and free any native display
 * state.  Do not return an invented EGLDisplay handle: that would test
 * EGL_BAD_DISPLAY instead of the required condition.
 */
EGLDisplay GS_EGL10_get_noninitializable_display(void);
void GS_EGL10_release_noninitializable_display(EGLDisplay display);

/*
 * Comparator for the implementation-defined EGL_NATIVE_VISUAL_TYPE sorting
 * priority used by eglChooseConfig.  When
 * GS_EGL10_NATIVE_VISUAL_COMPARE_AVAILABLE is defined, return -1 when lhs sorts
 * before rhs, 0 when their priorities are equal, and 1 when lhs sorts after
 * rhs.  The result must reflect the target EGL implementation's documented
 * ordering, be deterministic and antisymmetric, and return 0 for equal inputs.
 */
#ifdef GS_EGL10_NATIVE_VISUAL_COMPARE_AVAILABLE
int GS_EGL10_compare_native_visual_type(EGLint lhs, EGLint rhs);
#endif

typedef void (*GS_EGL10_ThreadFunction)(void *argument);

/*
 * Non-pthread targets implement this synchronous thread adapter.  Create a new
 * thread in the same process, invoke function(argument) exactly once in that
 * thread, wait for it to finish, publish its memory writes to the caller, and
 * return EGL_TRUE.  Validate function before creating the thread.  On any
 * failure return EGL_FALSE only after guaranteeing that no worker can still
 * access argument, which may point to caller-owned stack/static test state.
 * GS_EGL_USE_PTHREAD selects the implementation embedded in TP_006 instead.
 */
#ifndef GS_EGL_USE_PTHREAD
EGLBoolean GS_EGL10_create_and_join_thread(
    GS_EGL10_ThreadFunction function, void *argument);
#endif

/*
 * Platform test hook interface
 * ============================
 *
 * The declarations below are intentionally enabled only when the build defines
 * GS_EGL_PLATFORM_TEST_HOOKS.  They are contracts, not generic implementations:
 * a target integration must provide exactly one externally linked definition
 * for every declared function in a platform-specific .c file.
 *
 * Do not define GS_EGL_PLATFORM_TEST_HOOKS merely to make the guarded tests
 * compile.  Define it only when the linked implementation can create and
 * observe all of the documented conditions reliably.  A platform which cannot
 * implement a condition must leave the macro disabled for the affected test
 * binary and report that procedure as not applicable.
 *
 * General ownership rules for every prepare/cleanup pair:
 *
 *  - A prepare function returns EGL_TRUE only after every output is valid and
 *    the complete postcondition in its comment has been established.
 *  - On EGL_FALSE, prepare must either release partial resources immediately or
 *    retain enough state for its cleanup function to release them safely.
 *  - Output pointers must be checked before use.  Invalid pointers cause
 *    prepare to return EGL_FALSE without leaking or changing an active fixture.
 *  - The implementation owns the native display/window/pixmap, any EGLContext
 *    and EGLSurface not returned for destruction by the test, instrumentation,
 *    snapshots and fault-injection state that it creates.
 *  - Cleanup must unbind current objects before destroying them, destroy EGL
 *    objects before their native backing objects, terminate a display only if
 *    the fixture initialized it, and reset all private state.
 *  - Cleanup must also be safe after a partially failed setup and after the test
 *    invalidated a native object or terminated the EGLDisplay.
 *  - Hooks run in the test thread unless their comment explicitly permits an
 *    implementation worker.  EGL error state is thread-local; do not generate
 *    or consume an error in another thread when the test expects eglGetError()
 *    to observe it in the calling thread.
 *
 * See README.md, "Platform test hook'ları", for the integration procedure and
 * the requirement-to-hook mapping.
 */
#ifdef GS_EGL_PLATFORM_TEST_HOOKS

/*
 * Create the normal native-window fixture used by eglCreateWindowSurface tests.
 *
 * Required work:
 *  1. Open/create the target's native display connection and native window.
 *  2. Obtain and initialize the matching EGLDisplay.
 *  3. Select a valid EGLConfig containing EGL_WINDOW_BIT whose native visual or
 *     pixel format is compatible with the native window.
 *  4. Return the initialized display, compatible config and available window.
 *
 * Do not create an EGLSurface for the window.  The test owns any EGLSurface or
 * EGLContext that it subsequently creates.  The same window must remain usable
 * after an EGLSurface is destroyed because TP_004 creates two surfaces in
 * sequence.  The window must not already be associated with another live
 * EGLSurface.  cleanup_native_window owns the native objects and EGLDisplay and
 * must tolerate TP_012 having already called eglTerminate(display).
 */
EGLBoolean GS_EGL10_prepare_native_window(EGLDisplay *display,
    EGLConfig *config, EGLNativeWindowType *window);
void GS_EGL10_cleanup_native_window(void);

/*
 * Create an initialized display, a valid available native window, and a valid
 * window-capable EGLConfig deliberately incompatible with that window's native
 * visual/pixel format.  The only invalid relationship passed to
 * eglCreateWindowSurface must be window-versus-config compatibility so that the
 * target call deterministically produces EGL_BAD_MATCH.
 */
EGLBoolean GS_EGL10_prepare_incompatible_native_window(
    EGLDisplay *display, EGLConfig *config, EGLNativeWindowType *window);
void GS_EGL10_cleanup_incompatible_native_window(void);

/*
 * Create an initialized display and valid native window, then return a valid
 * EGLConfig from that display whose EGL_SURFACE_TYPE does not contain
 * EGL_WINDOW_BIT.  Return EGL_FALSE, without leaking resources, when the target
 * exposes no such config; the caller reports that case as not applicable.
 */
EGLBoolean GS_EGL10_prepare_non_window_config(EGLDisplay *display,
    EGLConfig *config, EGLNativeWindowType *window);
void GS_EGL10_cleanup_non_window_config(void);

/*
 * Return an initialized display, a compatible window-capable config, and a
 * native window handle which the target EGL implementation can positively
 * identify as invalid.  A robust implementation normally creates a real native
 * window, saves its handle, destroys/invalidates it through the native API, and
 * returns the stale handle.  Do not guess that a zero or fabricated bit pattern
 * is invalid unless the target platform explicitly guarantees that behavior.
 * The following eglCreateWindowSurface call must be the operation that produces
 * EGL_BAD_NATIVE_WINDOW.  Cleanup must not destroy the native object twice.
 */
EGLBoolean GS_EGL10_prepare_invalid_native_window(EGLDisplay *display,
    EGLConfig *config, EGLNativeWindowType *window);
void GS_EGL10_cleanup_invalid_native_window(void);

/*
 * Return an initialized display, a compatible pixmap-capable config, and a
 * native pixmap handle which the target EGL implementation can positively
 * identify as invalid.  Prefer creating and then invalidating a real pixmap to
 * inventing a handle.  The following eglCreatePixmapSurface call must produce
 * EGL_BAD_NATIVE_PIXMAP in the calling thread.  Cleanup must tolerate the
 * pixmap already being invalid.
 */
EGLBoolean GS_EGL10_prepare_invalid_native_pixmap(EGLDisplay *display,
    EGLConfig *config, EGLNativePixmapType *pixmap);
void GS_EGL10_cleanup_invalid_native_pixmap(void);

/*
 * Prepare otherwise valid eglCreateWindowSurface inputs and arm deterministic
 * allocation-failure injection for the next surface creation.  Use a supported
 * driver/platform fault-injection facility; do not try to exhaust all process
 * or system memory.  The target call must return EGL_NO_SURFACE and set
 * EGL_BAD_ALLOC for this reason alone.  Cleanup must always disarm injection,
 * even when the target unexpectedly succeeds or setup only partially completes.
 */
EGLBoolean GS_EGL10_prepare_window_surface_allocation_failure(
    EGLDisplay *display, EGLConfig *config, EGLNativeWindowType *window);
void GS_EGL10_cleanup_window_surface_allocation_failure(void);

/*
 * Prepare arguments and private fixture state so the next eglMakeCurrent call
 * made by the test fails with exactly expected_error.  Supported requests are:
 *
 *  EGL_BAD_NATIVE_WINDOW:
 *    Create a window-backed draw or read surface, then invalidate its native
 *    window without destroying the EGLSurface.
 *
 *  EGL_BAD_MATCH:
 *    Return individually valid/compatible draw, read and context objects, but
 *    use controlled graphics-memory pressure or fault injection so draw and
 *    read cannot fit simultaneously.  Do not introduce a simpler mismatch.
 *
 *  EGL_BAD_CURRENT_SURFACE:
 *    Establish a previous current context/surface in the calling thread, queue
 *    unflushed rendering commands, invalidate that previous native surface,
 *    and return valid objects for the attempted replacement binding.
 *
 *  EGL_BAD_ALLOC:
 *    Return otherwise valid compatible objects and inject failure of ancillary
 *    buffer allocation performed by the next eglMakeCurrent call.
 *
 * Return EGL_FALSE for unsupported expected_error values.  All returned handles
 * must remain alive until cleanup.  Do not call the target eglMakeCurrent on the
 * test's behalf.  cleanup_make_current_error must be safe even when prepare
 * returned EGL_FALSE because some callers invoke cleanup unconditionally; it
 * must release current bindings, EGL/native resources, queued observation state
 * and all fault injection associated with that expected_error.
 */
EGLBoolean GS_EGL10_prepare_make_current_error(EGLint expected_error,
    EGLDisplay *display, EGLSurface *draw, EGLSurface *read,
    EGLContext *context);
void GS_EGL10_cleanup_make_current_error(EGLint expected_error);

/*
 * Build the shared current-window fixture used by eglSwapBuffers integration
 * tests.  Create a native window; initialize EGL; choose a compatible
 * EGL_WINDOW_BIT config; create a window EGLSurface and compatible EGLContext;
 * bind the context and surface to the calling thread; and return display and
 * surface.  For posting verification, render a deterministic pattern and make
 * the pre-swap native content distinguishable from that pattern.  Keep all
 * native handles, context and other owned state privately for the operations
 * below.  Only one such fixture needs to be active at a time.
 *
 * Cleanup must unbind and destroy the context/surface, terminate the owned EGL
 * display connection, destroy the native window, remove instrumentation, and
 * tolerate invalidate_native_window having already destroyed the window.
 */
EGLBoolean GS_EGL10_prepare_current_window_surface(EGLDisplay *display,
    EGLSurface *surface);
void GS_EGL10_cleanup_current_window_surface(void);

/*
 * With a current-window fixture active and after the test calls eglSwapBuffers,
 * observe the native/front-buffer result using the target window-system API.
 * Synchronize or wait for asynchronous presentation with a bounded timeout and
 * return EGL_TRUE only when the deterministic rendered content was actually
 * posted.  Do not call eglSwapBuffers again and do not treat EGL_TRUE alone as
 * evidence that pixels reached the native window.
 */
EGLBoolean GS_EGL10_verify_window_content_posted(void);

/*
 * With a current-window fixture active, resize its native window to a supported,
 * non-zero size different from the current EGLSurface size.  Wait until the
 * native system accepts the resize, return that accepted size through both
 * output pointers, and do not call eglSwapBuffers.  Return EGL_FALSE if the
 * platform cannot distinguish this resize step from transparent EGL resizing.
 */
EGLBoolean GS_EGL10_resize_native_window(EGLint *expected_width,
    EGLint *expected_height);

/*
 * Reset/arm target instrumentation immediately before eglSwapBuffers so only
 * the target call's implicit client-API flush is observed.  The query returns
 * EGL_TRUE only if that call caused the equivalent of glFlush for the current
 * context.  Driver instrumentation, a validated interposer, or an equivalent
 * synchronization oracle is required; posting success alone is insufficient.
 * Neither function may itself flush the current context.
 */
void GS_EGL10_begin_flush_observation(void);
EGLBoolean GS_EGL10_implicit_flush_observed(void);

/*
 * With a current-window fixture active, invalidate/destroy the backing native
 * window without destroying its EGLSurface or releasing the current binding.
 * Preserve the stale association long enough for the next eglSwapBuffers call
 * to detect it and produce EGL_BAD_NATIVE_WINDOW.  Wait for native invalidation
 * to become observable when the platform is asynchronous.
 */
EGLBoolean GS_EGL10_invalidate_native_window(void);

/*
 * Build a native-pixmap fixture: create a native pixmap with a deterministic
 * initial value, initialize EGL, choose a compatible EGL_PIXMAP_BIT config,
 * create its EGLSurface and a compatible EGLContext, and bind them in the
 * calling thread.  Complete and synchronize all setup writes, then establish/
 * snapshot the native pixmap state immediately before returning so the
 * no-posting check below has a trustworthy baseline.  Return the EGLDisplay and
 * current EGLSurface; retain the native pixmap, context and snapshot privately.
 *
 * Cleanup must unbind, destroy the context and surface, terminate owned EGL
 * state, destroy the native pixmap and discard the snapshot.
 */
EGLBoolean GS_EGL10_prepare_current_pixmap_surface(EGLDisplay *display,
    EGLSurface *surface);
void GS_EGL10_cleanup_current_pixmap_surface(void);

/*
 * After the test calls eglSwapBuffers on the active pixmap surface, use the
 * native platform API to compare the pixmap against the saved baseline and
 * return EGL_TRUE only when the swap had no posting effect.  Perform any native
 * synchronization required for a reliable read, but do not issue another swap
 * or otherwise modify the pixmap before comparison.
 */
EGLBoolean GS_EGL10_verify_pixmap_unchanged(void);

#endif /* GS_EGL_PLATFORM_TEST_HOOKS */

#endif
