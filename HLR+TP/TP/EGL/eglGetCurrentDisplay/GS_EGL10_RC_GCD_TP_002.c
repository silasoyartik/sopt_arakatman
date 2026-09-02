#include <stdio.h>
#include <EGL/egl.h>
#include "macros.h"

/*
EGL10 - RenderingContexts - GetCurrentDisplay

Covered requirements:
    - GS-EGL10-RC-GCD-002
    - GS-EGL10-RC-GCD-003

TC_002:
    eglGetCurrentDisplay shall return the EGLDisplay associated
    with the calling thread's current EGLContext.

TC_003:
    eglGetCurrentDisplay shall return EGL_NO_DISPLAY when no
    current EGLContext exists for the calling thread.
*/

static const char* test_case1 =
    "GS_EGL10_RC_GCD_TC_002";

static const char* test_case2 =
    "GS_EGL10_RC_GCD_TC_003";

static const char* test_procedure =
    "GS_EGL10_RC_GCD_TP_002";


static EGLBoolean test_success1 = EGL_TRUE;
static EGLBoolean test_success2 = EGL_TRUE;

static EGLDisplay display = EGL_NO_DISPLAY;
static EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;

static EGLBoolean initialized = EGL_FALSE;
static EGLBoolean context_current = EGL_FALSE;


/* Initialization */
void GS_EGL10_RC_GCD_TP_002_init(void)
{
    EGLBoolean result;
    EGLint error;

    EGLConfig config = (EGLConfig)0;
    EGLint num_config = 0;

    EGLDisplay current_display;


    test_success1 = EGL_TRUE;
    test_success2 = EGL_TRUE;


    /* TEST PRECONDITION
     * Obtain and initialize an EGLDisplay.
     */

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);


    if (display == EGL_NO_DISPLAY)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: eglGetDisplay returned "
            "EGL_NO_DISPLAY. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    result = eglInitialize(
        display,
        NULL,
        NULL
    );


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: eglInitialize returned "
            "EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    initialized = EGL_TRUE;


    /* Select an EGLConfig supporting a Pbuffer surface.
     * A Pbuffer is used because it does not require a native window system object.
     */

    const EGLint config_attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };


    result = eglChooseConfig(
        display,
        config_attribs,
        &config,
        1,
        &num_config
    );


    if (result != EGL_TRUE ||
        num_config != 1 ||
        config == (EGLConfig)0)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: unable to obtain "
            "a Pbuffer-capable EGLConfig. "
            "num_config: %d, eglGetError(): 0x%x",
            num_config,
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    /* Create a small Pbuffer surface. */
    const EGLint pbuffer_attribs[] =
    {
        EGL_WIDTH,  1,
        EGL_HEIGHT, 1,
        EGL_NONE
    };


    surface = eglCreatePbufferSurface(
        display,
        config,
        pbuffer_attribs
    );


    if (surface == EGL_NO_SURFACE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: "
            "eglCreatePbufferSurface returned "
            "EGL_NO_SURFACE. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    /* Create an EGLContext.
     * No rendering is required by this TP, so no shaders, buffers or drawing operations are needed. */
    context = eglCreateContext(
        display,
        config,
        EGL_NO_CONTEXT,
        NULL
    );


    if (context == EGL_NO_CONTEXT)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: eglCreateContext "
            "returned EGL_NO_CONTEXT. "
            "eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    /* Make the EGLContext current on the calling thread. */
    result = eglMakeCurrent(
        display,
        surface,
        surface,
        context
    );


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Test precondition failed: eglMakeCurrent "
            "returned EGL_FALSE. eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
        test_success2 = EGL_FALSE;

        return;
    }


    context_current = EGL_TRUE;


    // TEST CASE 002

    /* A context associated with 'display' is now current on
     * the calling thread.
     * Expected:
     *     eglGetCurrentDisplay() == display
     */
    (void)eglGetError();


    current_display = eglGetCurrentDisplay();


    error = eglGetError();


    if (current_display != display)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglGetCurrentDisplay did not return the "
            "EGLDisplay associated with the current context. "
            "Expected: %p, got: %p",
            (void*)display,
            (void*)current_display
        );

        test_success1 = EGL_FALSE;
    }


    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "eglGetCurrentDisplay generated an unexpected "
            "EGL error while a context was current. "
            "eglGetError(): 0x%x",
            error
        );

        test_success1 = EGL_FALSE;
    }


    if (test_success1)
    {
        TEST_LOG_INFO(
            "eglGetCurrentDisplay returned the EGLDisplay "
            "associated with the current EGLContext"
        );

        TEST_LOG_SUCCESS(
            test_case1,
            test_procedure
        );
    }


    /* Release the current context.
     * After this call, the calling thread shall no longer
     * have a current EGLContext.
     */

    result = eglMakeCurrent(
        display,
        EGL_NO_SURFACE,
        EGL_NO_SURFACE,
        EGL_NO_CONTEXT
    );


    if (result != EGL_TRUE)
    {
        error = eglGetError();

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Test precondition failed: unable to release "
            "the current EGLContext. eglGetError(): 0x%x",
            error
        );

        test_success2 = EGL_FALSE;

        return;
    }


    context_current = EGL_FALSE;


    // TEST CASE 003

    /* The current EGLContext has been released.
     * Expected:
     *     eglGetCurrentDisplay() == EGL_NO_DISPLAY
     */
    (void)eglGetError();


    current_display = eglGetCurrentDisplay();


    error = eglGetError();


    if (current_display != EGL_NO_DISPLAY)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Expected EGL_NO_DISPLAY after releasing "
            "the current EGLContext, got: %p",
            (void*)current_display
        );

        test_success2 = EGL_FALSE;
    }


    /* EGL_NO_DISPLAY is the normal state result when there
     * is no current context. It is not itself an EGL error.
     */
    if (error != EGL_SUCCESS)
    {
        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "eglGetCurrentDisplay generated an unexpected "
            "EGL error after the context was released. "
            "eglGetError(): 0x%x",
            error
        );

        test_success2 = EGL_FALSE;
    }


    if (test_success2)
    {
        TEST_LOG_INFO(
            "eglGetCurrentDisplay returned EGL_NO_DISPLAY "
            "after the current EGLContext was released"
        );

        TEST_LOG_SUCCESS(
            test_case2,
            test_procedure
        );
    }
}


void GS_EGL10_RC_GCD_TP_002_draw(void) {

}

void GS_EGL10_RC_GCD_TP_002_close(void)
{
    if (context_current == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(
            display,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT
        );
    }


    if (context != EGL_NO_CONTEXT &&
        display != EGL_NO_DISPLAY)
    {
        eglDestroyContext(
            display,
            context
        );
    }


    if (surface != EGL_NO_SURFACE &&
        display != EGL_NO_DISPLAY)
    {
        eglDestroySurface(
            display,
            surface
        );
    }


    if (initialized == EGL_TRUE &&
        display != EGL_NO_DISPLAY)
    {
        eglTerminate(display);
    }


    context_current = EGL_FALSE;
    initialized = EGL_FALSE;

    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    display = EGL_NO_DISPLAY;
}