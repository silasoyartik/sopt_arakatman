/*
 * GLFW reference environment for the OpenGL ES 3.1 test suite.
 *
 * This file is NOT part of the OpenGL ES requirements being
 * verified.
 *
 * It only establishes the execution environment required by
 * the test procedures.
 *
 * A target that cannot or does not wish to use GLFW may omit
 * this source file from the build and provide another source
 * file implementing:
 *
 *     GS_GLES31_prepare_environment()
 *     GS_GLES31_environment_is_current()
 *     GS_GLES31_cleanup_environment()
 *
 * The individual TP source files shall not need to change.
 */


#include "helpers_gles31.h"


/*
 * Prevent GLFW from selecting or including another OpenGL
 * header.  helpers_gles31.h already selected the appropriate
 * OpenGL ES / GLAD header.
 */
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


/* ============================================================
 * GLFW ERROR CALLBACK
 * ============================================================ */

static void GS_GLES31_glfw_error_callback(
    int error,
    const char *description)
{
    fprintf(
        stderr,
        "GLFW error %d: %s\n",
        error,
        description != NULL
            ? description
            : "No GLFW error description available"
    );
}


/* ============================================================
 * OPENGL ES ENTRY-POINT LOADING
 * ============================================================
 *
 * The OpenGL ES function loader is test infrastructure.
 *
 * It is not part of glDispatchCompute and is not a requirement
 * imposed on the target implementation.
 *
 * If the target exposes OpenGL ES entry points in another way,
 * it may replace this platform adaptation file.
 */

static int GS_GLES31_load_entry_points(void)
{

#if defined(GS_GLES31_USE_GLAD2)

    /*
     * GLAD 2 OpenGL ES loader.
     *
     * The GLAD package linked with this test binary must have
     * been generated for an OpenGL ES API containing the
     * functions required by OpenGL ES 3.1.
     */
    if (gladLoadGLES2(
            (GLADloadfunc)glfwGetProcAddress) == 0)
    {
        fprintf(
            stderr,
            "Failed to load OpenGL ES entry points with GLAD 2.\n"
        );

        return 0;
    }


#elif defined(GS_GLES31_USE_GLAD1)

    /*
     * GLAD 1 OpenGL ES loader.
     *
     * The exact GLAD API is determined when the GLAD source is
     * generated.  This reference path assumes a GLAD 1 package
     * exposing gladLoadGLES2Loader().
     *
     * If the generated loader uses another function name,
     * adapt only this platform file.  The TP files must remain
     * unchanged.
     */
    if (!gladLoadGLES2Loader(
            (GLADloadproc)glfwGetProcAddress))
    {
        fprintf(
            stderr,
            "Failed to load OpenGL ES entry points with GLAD 1.\n"
        );

        return 0;
    }


#else

    /*
     * No explicit loader has been selected.
     *
     * In this configuration the target build environment is
     * responsible for exposing the required OpenGL ES 3.1
     * entry points.
     *
     * Examples include implementations where the GLES library
     * exports the required core functions directly.
     *
     * If dynamic loading is necessary on the target, provide
     * an appropriate loader here or enable one of the GLAD
     * configurations above.
     */
    (void)glfwGetProcAddress;

#endif


    return 1;
}


/* ============================================================
 * PREPARE ENVIRONMENT
 * ============================================================ */

int GS_GLES31_prepare_environment(
    GS_GLES31_TestEnvironment *environment)
{
    GLFWwindow *window;

    int major;
    int minor;
    int client_api;

    const GLubyte *version_string;


    if (environment == NULL)
    {
        return 0;
    }


    /*
     * Do not overwrite an environment that already owns a
     * platform fixture.
     */
    if (environment->initialized)
    {
        return 0;
    }


    environment->platform_context = NULL;
    environment->initialized = 0;


    /*
     * GLFW is used only by this reference platform
     * implementation.
     */
    glfwSetErrorCallback(
        GS_GLES31_glfw_error_callback
    );


    if (glfwInit() != GLFW_TRUE)
    {
        fprintf(
            stderr,
            "Could not initialize the GLFW reference environment.\n"
        );

        return 0;
    }


    /*
     * Reset hints so that state left by another GLFW user in
     * the same process does not accidentally change the context
     * requested by this fixture.
     */
    glfwDefaultWindowHints();


    /*
     * Explicitly request OpenGL ES rather than desktop OpenGL.
     */
    glfwWindowHint(
        GLFW_CLIENT_API,
        GLFW_OPENGL_ES_API
    );


    /*
     * Request the API version required by this test module.
     *
     * OpenGL ES 3.1 is the minimum acceptable version.
     */
    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );


    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        1
    );


    /*
     * Compute tests do not require a visible application
     * window.
     *
     * GLFW does not create an OpenGL/OpenGL ES context without
     * an associated window object, so the reference
     * implementation creates a hidden window and uses only its
     * context.
     *
     * Another platform implementation does not need to follow
     * this design.  It only needs to establish an equivalent
     * current OpenGL ES 3.1 context.
     */
    glfwWindowHint(
        GLFW_VISIBLE,
        GLFW_FALSE
    );


    window = glfwCreateWindow(
        1,
        1,
        "GS OpenGL ES 3.1 Test Context",
        NULL,
        NULL
    );


    if (window == NULL)
    {
        fprintf(
            stderr,
            "GLFW could not create the OpenGL ES 3.1 reference context.\n"
        );

        glfwTerminate();

        return 0;
    }


    /*
     * OpenGL ES commands operate on the context current for the
     * calling thread.
     */
    glfwMakeContextCurrent(
        window
    );


    if (glfwGetCurrentContext() != window)
    {
        fprintf(
            stderr,
            "The GLFW OpenGL ES context could not be made current.\n"
        );

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return 0;
    }


    /*
     * Load OpenGL ES functions only after the context has been
     * made current.
     */
    if (!GS_GLES31_load_entry_points())
    {
        glfwMakeContextCurrent(
            NULL
        );

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return 0;
    }


    /*
     * Verify that GLFW created an OpenGL ES context, not a
     * desktop OpenGL context.
     */
    client_api = glfwGetWindowAttrib(
        window,
        GLFW_CLIENT_API
    );


    if (client_api != GLFW_OPENGL_ES_API)
    {
        fprintf(
            stderr,
            "The created context is not an OpenGL ES context.\n"
        );

        glfwMakeContextCurrent(
            NULL
        );

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return 0;
    }


    /*
     * Verify the actual version supplied by the target.
     *
     * Requesting version 3.1 is not by itself sufficient for a
     * conformance-style test.  The resulting context is checked
     * before the fixture is accepted.
     */
    major = glfwGetWindowAttrib(
        window,
        GLFW_CONTEXT_VERSION_MAJOR
    );


    minor = glfwGetWindowAttrib(
        window,
        GLFW_CONTEXT_VERSION_MINOR
    );


    if (major < 3 ||
        (major == 3 && minor < 1))
    {
        fprintf(
            stderr,
            "OpenGL ES 3.1 or later is required; "
            "the created context reports %d.%d.\n",
            major,
            minor
        );

        glfwMakeContextCurrent(
            NULL
        );

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return 0;
    }


    /*
     * A valid current context shall provide a version string.
     *
     * This is an additional fixture sanity check and is not a
     * glDispatchCompute test.
     */
    version_string = glGetString(
        GL_VERSION
    );


    if (version_string == NULL)
    {
        fprintf(
            stderr,
            "Could not query GL_VERSION from the current context.\n"
        );

        glfwMakeContextCurrent(
            NULL
        );

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return 0;
    }


    /*
     * The fixture is now valid and ownership is transferred to
     * GS_GLES31_TestEnvironment.
     */
    environment->platform_context =
        (void *)window;

    environment->initialized = 1;


    /*
     * Setup calls shall not leave stale errors for the test
     * procedure.
     *
     * Individual tests shall still clear the error state
     * immediately before an API call when the requirement being
     * verified concerns glGetError().
     */
    GS_GLES31_clear_errors();


    return 1;
}


/* ============================================================
 * CHECK CURRENT CONTEXT
 * ============================================================ */

int GS_GLES31_environment_is_current(
    const GS_GLES31_TestEnvironment *environment)
{
    GLFWwindow *window;


    if (environment == NULL ||
        !environment->initialized ||
        environment->platform_context == NULL)
    {
        return 0;
    }


    window = (GLFWwindow *)
        environment->platform_context;


    return glfwGetCurrentContext() == window;
}


/* ============================================================
 * CLEANUP ENVIRONMENT
 * ============================================================ */

void GS_GLES31_cleanup_environment(
    GS_GLES31_TestEnvironment *environment)
{
    GLFWwindow *window;


    if (environment == NULL)
    {
        return;
    }


    window = (GLFWwindow *)
        environment->platform_context;


    if (window != NULL)
    {
        /*
         * A platform implementation shall release any current
         * binding before destroying its context unless its
         * native API specifies another required sequence.
         */
        if (glfwGetCurrentContext() == window)
        {
            glfwMakeContextCurrent(
                NULL
            );
        }


        glfwDestroyWindow(
            window
        );
    }


    /*
     * This reference implementation assumes that the test
     * fixture owns GLFW initialization for the lifetime of this
     * environment.
     *
     * A larger application embedding the tests may need a
     * different ownership model.  In that case it should
     * provide its own platform adaptation implementation rather
     * than modifying individual TP files.
     */
    if (environment->initialized)
    {
        glfwTerminate();
    }


    environment->platform_context = NULL;
    environment->initialized = 0;
}