#include <EGL/egl.h>
#include <GLES3/gl31.h>
#include <stdio.h>
#include <stdint.h>
#include "../../macros.h"

/*
 * GLES31 - ComputeShaders - glDispatchCompute
 *
 * Covered requirements:
 *   GS-GLES31-CS-DC-001
 *   GS-GLES31-CS-DC-002
 *   GS-GLES31-CS-DC-013
 *
 * Purpose:
 *
 *   Verify that glDispatchCompute:
 *
 *   1. Is available and accepts a valid dispatch command.
 *
 *   2. Uses the active compute shader executable to execute
 *      the requested compute work groups.
 *
 *   3. Generates no error when valid work-group counts are
 *      supplied and a valid compute shader executable is active.
 *
 * Test method:
 *
 *   A GLSL ES 3.10 compute shader with a local work-group size
 *   of (1, 1, 1) is executed with:
 *
 *       glDispatchCompute(4, 1, 1)
 *
 *   The shader writes:
 *
 *       data[0] = 1
 *       data[1] = 2
 *       data[2] = 3
 *       data[3] = 4
 *
 *   to a shader storage buffer object (SSBO).
 *
 *   Successful execution is verified by mapping the buffer and
 *   comparing the resulting values with the expected values.
 */


static const char *test_case1 = "GS_GLES31_CS_DC_TC_001";
static const char *test_case2 = "GS_GLES31_CS_DC_TC_002";
static const char *test_case3 = "GS_GLES31_CS_DC_TC_013";

static const char *test_procedure =
    "GS_GLES31_CS_DC_TP_001";


static GLboolean test_success1 = GL_TRUE;
static GLboolean test_success2 = GL_TRUE;
static GLboolean test_success3 = GL_TRUE;


/*
 * EGL objects used to create the OpenGL ES 3.1 test environment.
 */
static EGLDisplay display = EGL_NO_DISPLAY;
static EGLConfig config = (EGLConfig)0;
static EGLContext context = EGL_NO_CONTEXT;
static EGLSurface surface = EGL_NO_SURFACE;


/*
 * OpenGL ES objects used by the test.
 */
static GLuint compute_program = 0;
static GLuint output_buffer = 0;


/* ============================================================
 * COMPUTE SHADER
 * ============================================================ */

static const char *compute_shader_source =
    "#version 310 es\n"
    "\n"
    "layout(\n"
    "    local_size_x = 1,\n"
    "    local_size_y = 1,\n"
    "    local_size_z = 1\n"
    ") in;\n"
    "\n"
    "layout(std430, binding = 0) buffer OutputBuffer\n"
    "{\n"
    "    uint data[];\n"
    "};\n"
    "\n"
    "void main()\n"
    "{\n"
    "    uint index = gl_GlobalInvocationID.x;\n"
    "\n"
    "    data[index] = index + 1u;\n"
    "}\n";


/* ============================================================
 * CLEAR PREVIOUS GL ERRORS
 * ============================================================ */

static void clear_gl_errors(void)
{
    while (glGetError() != GL_NO_ERROR)
    {
        /* Clear previous OpenGL ES errors. */
    }
}


/* ============================================================
 * MARK ALL TEST CASES AS FAILED
 * ============================================================ */

static void fail_all_test_cases(void)
{
    test_success1 = GL_FALSE;
    test_success2 = GL_FALSE;
    test_success3 = GL_FALSE;
}


/* ============================================================
 * PREPARE OPENGL ES 3.1 ENVIRONMENT
 * ============================================================ */

static GLboolean prepare_gles31_environment(void)
{
    EGLint number_of_configs = 0;

    EGLint major = 0;
    EGLint minor = 0;

    GLint gl_major = 0;
    GLint gl_minor = 0;


    const EGLint config_attributes[] =
    {
        EGL_SURFACE_TYPE,
        EGL_PBUFFER_BIT,

        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES3_BIT,

        EGL_RED_SIZE,
        8,

        EGL_GREEN_SIZE,
        8,

        EGL_BLUE_SIZE,
        8,

        EGL_NONE
    };


    const EGLint pbuffer_attributes[] =
    {
        EGL_WIDTH,
        1,

        EGL_HEIGHT,
        1,

        EGL_NONE
    };


    const EGLint context_attributes[] =
    {
        EGL_CONTEXT_MAJOR_VERSION,
        3,

        EGL_CONTEXT_MINOR_VERSION,
        1,

        EGL_NONE
    };


    /*
     * Obtain the default EGL display.
     */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (display == EGL_NO_DISPLAY)
    {
        return GL_FALSE;
    }


    /*
     * Initialize EGL.
     */
    if (eglInitialize(
            display,
            &major,
            &minor) != EGL_TRUE)
    {
        return GL_FALSE;
    }


    /*
     * Select the OpenGL ES client API.
     */
    if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
    {
        return GL_FALSE;
    }


    /*
     * Find a configuration capable of supporting
     * an OpenGL ES 3.x context and a pbuffer surface.
     */
    if (eglChooseConfig(
            display,
            config_attributes,
            &config,
            1,
            &number_of_configs) != EGL_TRUE)
    {
        return GL_FALSE;
    }


    if (number_of_configs < 1 ||
        config == (EGLConfig)0)
    {
        return GL_FALSE;
    }


    /*
     * Create a small off-screen pbuffer surface.
     *
     * No visible window is required for this compute test.
     */
    surface = eglCreatePbufferSurface(
        display,
        config,
        pbuffer_attributes
    );


    if (surface == EGL_NO_SURFACE)
    {
        return GL_FALSE;
    }


    /*
     * Explicitly request an OpenGL ES 3.1 context.
     */
    context = eglCreateContext(
        display,
        config,
        EGL_NO_CONTEXT,
        context_attributes
    );


    if (context == EGL_NO_CONTEXT)
    {
        return GL_FALSE;
    }


    /*
     * Make the OpenGL ES context current.
     */
    if (eglMakeCurrent(
            display,
            surface,
            surface,
            context) != EGL_TRUE)
    {
        return GL_FALSE;
    }


    /*
     * Verify that the created context is at least
     * OpenGL ES 3.1.
     */
    glGetIntegerv(
        GL_MAJOR_VERSION,
        &gl_major
    );

    glGetIntegerv(
        GL_MINOR_VERSION,
        &gl_minor
    );


    if (gl_major < 3 ||
        (gl_major == 3 && gl_minor < 1))
    {
        return GL_FALSE;
    }


    return GL_TRUE;
}


/* ============================================================
 * CREATE COMPUTE PROGRAM
 * ============================================================ */

static GLboolean create_compute_program(void)
{
    GLuint shader;

    GLint compile_status = GL_FALSE;
    GLint link_status = GL_FALSE;

    GLchar info_log[1024];


    /*
     * Create the compute shader object.
     */
    shader = glCreateShader(GL_COMPUTE_SHADER);

    if (shader == 0)
    {
        return GL_FALSE;
    }


    /*
     * Supply GLSL ES 3.10 source.
     */
    glShaderSource(
        shader,
        1,
        &compute_shader_source,
        NULL
    );


    /*
     * Compile the compute shader.
     */
    glCompileShader(shader);


    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &compile_status
    );


    if (compile_status != GL_TRUE)
    {
        GLsizei length = 0;

        glGetShaderInfoLog(
            shader,
            sizeof(info_log),
            &length,
            info_log
        );

        printf(
            "Compute shader compilation failed:\n%s\n",
            info_log
        );

        glDeleteShader(shader);

        return GL_FALSE;
    }


    /*
     * Create the program object.
     */
    compute_program = glCreateProgram();

    if (compute_program == 0)
    {
        glDeleteShader(shader);

        return GL_FALSE;
    }


    /*
     * Attach and link the compute shader.
     */
    glAttachShader(
        compute_program,
        shader
    );


    glLinkProgram(compute_program);


    glGetProgramiv(
        compute_program,
        GL_LINK_STATUS,
        &link_status
    );


    /*
     * The shader object is no longer required after linking.
     */
    glDetachShader(
        compute_program,
        shader
    );

    glDeleteShader(shader);


    if (link_status != GL_TRUE)
    {
        GLsizei length = 0;

        glGetProgramInfoLog(
            compute_program,
            sizeof(info_log),
            &length,
            info_log
        );

        printf(
            "Compute program linking failed:\n%s\n",
            info_log
        );

        glDeleteProgram(compute_program);

        compute_program = 0;

        return GL_FALSE;
    }


    return GL_TRUE;
}


/* ============================================================
 * CREATE OUTPUT SSBO
 * ============================================================ */

static GLboolean create_output_buffer(void)
{
    /*
     * Sentinel values make it possible to verify that the
     * compute shader really modified the buffer.
     */
    const GLuint initial_data[4] =
    {
        0xFFFFFFFFu,
        0xFFFFFFFFu,
        0xFFFFFFFFu,
        0xFFFFFFFFu
    };


    glGenBuffers(
        1,
        &output_buffer
    );


    if (output_buffer == 0)
    {
        return GL_FALSE;
    }


    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        output_buffer
    );


    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        sizeof(initial_data),
        initial_data,
        GL_DYNAMIC_READ
    );


    /*
     * Bind the SSBO to binding point 0.
     *
     * This corresponds to:
     *
     *     layout(std430, binding = 0)
     *
     * in the compute shader.
     */
    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        0,
        output_buffer
    );


    if (glGetError() != GL_NO_ERROR)
    {
        return GL_FALSE;
    }


    return GL_TRUE;
}


/* ============================================================
 * TP INITIALIZATION AND EXECUTION
 * ============================================================ */

void GS_GLES31_CS_DC_TP_001_init(void)
{
    /*
     * Using a function pointer keeps the same style used by
     * the previous EGL test procedures and explicitly refers
     * to the API entry point under test.
     */
    void (*dispatch_compute)(
        GLuint,
        GLuint,
        GLuint
    ) = glDispatchCompute;


    GLenum error;

    GLuint expected_data[4] =
    {
        1u,
        2u,
        3u,
        4u
    };

    GLuint *mapped_data = NULL;

    GLint index;


    /*
     * --------------------------------------------------------
     * TEST ENVIRONMENT SETUP
     * --------------------------------------------------------
     */

    if (!prepare_gles31_environment())
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Could not prepare a valid OpenGL ES 3.1 environment"
        );

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Could not prepare a valid OpenGL ES 3.1 environment"
        );

        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "Could not prepare a valid OpenGL ES 3.1 environment"
        );

        fail_all_test_cases();

        return;
    }


    /*
     * Verify that the glDispatchCompute API entry point exists.
     */
    if (dispatch_compute == NULL)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "glDispatchCompute entry point is not available"
        );

        fail_all_test_cases();

        return;
    }


    /*
     * Create and link a valid GLSL ES 3.10 compute program.
     */
    if (!create_compute_program())
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Could not create a valid compute shader program"
        );

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Could not create a valid compute shader program"
        );

        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "Could not create a valid compute shader program"
        );

        fail_all_test_cases();

        return;
    }


    /*
     * Create the shader storage buffer used to observe
     * compute execution.
     */
    if (!create_output_buffer())
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Could not create the output shader storage buffer"
        );

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Could not create the output shader storage buffer"
        );

        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "Could not create the output shader storage buffer"
        );

        fail_all_test_cases();

        return;
    }


    /*
     * Activate the successfully linked compute program.
     */
    glUseProgram(compute_program);


    /*
     * Bind the output SSBO again immediately before dispatch
     * so that the dispatch precondition is explicit.
     */
    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        0,
        output_buffer
    );


    /*
     * Remove any errors produced during test setup.
     *
     * The error checked below must belong specifically to the
     * glDispatchCompute call under test.
     */
    clear_gl_errors();


    /*
     * --------------------------------------------------------
     * FUNCTION UNDER TEST
     * --------------------------------------------------------
     *
     * local_size = (1, 1, 1)
     *
     * dispatch    = (4, 1, 1)
     *
     * Therefore four compute shader invocations are expected.
     */
    dispatch_compute(
        4,
        1,
        1
    );


    /*
     * Capture the error state immediately after the
     * function under test.
     */
    error = glGetError();


    /*
     * --------------------------------------------------------
     * TC_013
     *
     * A valid dispatch shall not generate an OpenGL ES error.
     * --------------------------------------------------------
     */

    if (error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(
            test_case3,
            test_procedure,
            "Valid glDispatchCompute call generated GL error 0x%x",
            error
        );

        test_success3 = GL_FALSE;
    }


    /*
     * If the dispatch itself failed, execution output cannot
     * be meaningfully verified.
     */
    if (error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "glDispatchCompute did not accept the valid dispatch"
        );

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Compute shader execution could not be verified"
        );

        test_success1 = GL_FALSE;
        test_success2 = GL_FALSE;

        return;
    }


    /*
     * Ensure that shader writes are visible to the subsequent
     * buffer access used by the test.
     */
    glMemoryBarrier(
        GL_BUFFER_UPDATE_BARRIER_BIT
    );


    /*
     * Map the output SSBO for CPU-side verification.
     */
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        output_buffer
    );


    mapped_data = (GLuint *)glMapBufferRange(
        GL_SHADER_STORAGE_BUFFER,
        0,
        sizeof(expected_data),
        GL_MAP_READ_BIT
    );


    if (mapped_data == NULL)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "Could not map compute shader output buffer"
        );

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Could not verify compute shader execution"
        );

        test_success1 = GL_FALSE;
        test_success2 = GL_FALSE;

        return;
    }


    /*
     * --------------------------------------------------------
     * TC_001 / TC_002
     *
     * Verify actual work-group execution.
     *
     * Expected output:
     *
     *     [0] = 1
     *     [1] = 2
     *     [2] = 3
     *     [3] = 4
     * --------------------------------------------------------
     */

    for (index = 0; index < 4; ++index)
    {
        if (mapped_data[index] != expected_data[index])
        {
            TEST_LOG_FAIL(
                test_case1,
                test_procedure,
                "Unexpected compute output at index %d: "
                "expected %u, got %u",
                index,
                expected_data[index],
                mapped_data[index]
            );

            TEST_LOG_FAIL(
                test_case2,
                test_procedure,
                "Active compute shader did not produce "
                "the expected dispatch output"
            );

            test_success1 = GL_FALSE;
            test_success2 = GL_FALSE;

            break;
        }
    }


    /*
     * Finish CPU access to the buffer.
     */
    if (glUnmapBuffer(
            GL_SHADER_STORAGE_BUFFER) != GL_TRUE)
    {
        TEST_LOG_FAIL(
            test_case1,
            test_procedure,
            "glUnmapBuffer reported invalidated buffer contents"
        );

        TEST_LOG_FAIL(
            test_case2,
            test_procedure,
            "Output buffer contents could not be reliably verified"
        );

        test_success1 = GL_FALSE;
        test_success2 = GL_FALSE;
    }


    /*
     * --------------------------------------------------------
     * FINAL RESULT REPORTING
     * --------------------------------------------------------
     */

    if (test_success1)
    {
        TEST_LOG_SUCCESS(
            test_case1,
            test_procedure
        );
    }


    if (test_success2)
    {
        TEST_LOG_SUCCESS(
            test_case2,
            test_procedure
        );
    }


    if (test_success3)
    {
        TEST_LOG_SUCCESS(
            test_case3,
            test_procedure
        );
    }
}


/* ============================================================
 * DRAW
 *
 * No graphics rendering is required for this compute test.
 * ============================================================ */

void GS_GLES31_CS_DC_TP_001_draw(void)
{
}


/* ============================================================
 * CLEANUP
 * ============================================================ */

void GS_GLES31_CS_DC_TP_001_close(void)
{
    /*
     * OpenGL ES resources must be deleted while the context
     * is still current.
     */
    if (context != EGL_NO_CONTEXT &&
        eglGetCurrentContext() == context)
    {
        glUseProgram(0);


        if (output_buffer != 0)
        {
            glDeleteBuffers(
                1,
                &output_buffer
            );

            output_buffer = 0;
        }


        if (compute_program != 0)
        {
            glDeleteProgram(
                compute_program
            );

            compute_program = 0;
        }
    }


    /*
     * Release the current EGL context.
     */
    if (display != EGL_NO_DISPLAY)
    {
        (void)eglMakeCurrent(
            display,
            EGL_NO_SURFACE,
            EGL_NO_SURFACE,
            EGL_NO_CONTEXT
        );


        if (surface != EGL_NO_SURFACE)
        {
            (void)eglDestroySurface(
                display,
                surface
            );

            surface = EGL_NO_SURFACE;
        }


        if (context != EGL_NO_CONTEXT)
        {
            (void)eglDestroyContext(
                display,
                context
            );

            context = EGL_NO_CONTEXT;
        }


        (void)eglTerminate(display);

        display = EGL_NO_DISPLAY;
    }


    config = (EGLConfig)0;
}