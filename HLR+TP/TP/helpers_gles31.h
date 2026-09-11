#ifndef GS_GLES31_TEST_HELPERS_H
#define GS_GLES31_TEST_HELPERS_H


/*
 * GS OpenGL ES 3.1 Test Helpers
 * =============================
 *
 * This header contains common helper functionality used by the
 * OpenGL ES 3.1 test procedures.
 *
 * IMPORTANT - PLATFORM INDEPENDENCE
 * ---------------------------------
 *
 * The test procedures must not depend directly on GLFW, EGL,
 * WGL, GLX, Wayland or any other platform-specific context
 * creation mechanism.
 *
 * The reference implementation supplied with this test suite
 * uses GLFW.  It is implemented separately in:
 *
 *     helpers_gles31_glfw.c
 *
 * A target integration that does not use GLFW may replace that
 * implementation with another source file, for example:
 *
 *     helpers_gles31_egl.c
 *     helpers_gles31_native.c
 *     helpers_gles31_target.c
 *
 * The replacement implementation only needs to provide the
 * platform functions declared in this header and establish the
 * documented OpenGL ES 3.1 test preconditions.
 *
 *
 * OPENGL ES FUNCTION LOADER
 * -------------------------
 *
 * Three configurations are supported by this header:
 *
 *     GS_GLES31_USE_GLAD1
 *         GLAD 1 style loader.
 *
 *     GS_GLES31_USE_GLAD2
 *         GLAD 2 style loader.
 *
 *     neither defined
 *         The target is responsible for providing OpenGL ES 3.1
 *         headers and entry points through its normal platform
 *         development environment.
 *
 * Only one GLAD option shall be enabled at a time.
 */


#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


/* ============================================================
 * OPENGL ES HEADER SELECTION
 * ============================================================
 *
 * The loader is not part of glDispatchCompute and is not part
 * of the requirement being tested.
 *
 * It exists only to make the OpenGL ES entry points available
 * to the test implementation.
 */

#if defined(GS_GLES31_USE_GLAD1) && defined(GS_GLES31_USE_GLAD2)

#error "Select either GS_GLES31_USE_GLAD1 or GS_GLES31_USE_GLAD2, not both."

#endif


#if defined(GS_GLES31_USE_GLAD2)

/*
 * GLAD 2 generated for the OpenGL ES API normally exposes this
 * header.
 *
 * If a target uses a differently generated GLAD package, adapt
 * this include without modifying the test procedures.
 */
#include <glad/gles2.h>


#elif defined(GS_GLES31_USE_GLAD1)

/*
 * GLAD 1 generated sources normally use glad/glad.h.
 *
 * The generated GLAD configuration must include OpenGL ES 3.1
 * functionality required by the test suite.
 */
#include <glad/glad.h>


#else

/*
 * Reference fallback for environments that expose OpenGL ES
 * directly through the platform SDK.
 *
 * A different target may replace this include with the
 * appropriate OpenGL ES 3.1 header.
 */
#include <GLES3/gl31.h>

#endif


#include "macros.h"


/* ============================================================
 * TEST ENVIRONMENT
 * ============================================================
 *
 * platform_context is deliberately opaque.
 *
 * For the GLFW reference implementation it contains a
 * GLFWwindow pointer.
 *
 * Other platform implementations are free to store another
 * context/window/fixture object here.
 *
 * The test procedures must never interpret platform_context.
 */

typedef struct
{
    void *platform_context;

    /*
     * Indicates that the platform implementation successfully
     * created the environment and owns resources that need to
     * be released by GS_GLES31_cleanup_environment().
     */
    int initialized;

} GS_GLES31_TestEnvironment;


#define GS_GLES31_ENV_INITIALIZER \
    { NULL, 0 }


/* ============================================================
 * PLATFORM INTEGRATION INTERFACE
 * ============================================================
 *
 * These functions are implemented by a platform adaptation
 * source file.
 *
 * The supplied reference implementation is:
 *
 *     helpers_gles31_glfw.c
 *
 * A replacement implementation shall provide the same
 * externally linked functions.
 */


/*
 * Prepare an OpenGL ES 3.1 test environment.
 *
 * Required postconditions on success:
 *
 *   1. An OpenGL ES 3.1 or later context exists.
 *
 *   2. The context is current on the calling test thread.
 *
 *   3. The OpenGL ES entry points required by the test suite
 *      are available.
 *
 *   4. Compute shaders are available as required by OpenGL
 *      ES 3.1.
 *
 *   5. environment->initialized is non-zero.
 *
 * The function returns non-zero on success and zero on failure.
 *
 * GLFW is NOT a requirement of this interface.
 */
int GS_GLES31_prepare_environment(
    GS_GLES31_TestEnvironment *environment
);


/*
 * Return non-zero only if the context represented by environment
 * is current on the calling thread.
 */
int GS_GLES31_environment_is_current(
    const GS_GLES31_TestEnvironment *environment
);


/*
 * Release every platform resource owned by the environment.
 *
 * The function shall be safe when called after a partially
 * failed setup.
 */
void GS_GLES31_cleanup_environment(
    GS_GLES31_TestEnvironment *environment
);


/* ============================================================
 * CLEAR OPENGL ES ERRORS
 * ============================================================
 *
 * Intended for test setup.
 *
 * Negative tests shall normally call this immediately before
 * invoking the function under test so that glGetError() observes
 * only errors associated with the operation being verified.
 */

static void GS_GLES31_clear_errors(void)
{
    while (glGetError() != GL_NO_ERROR)
    {
        /*
         * Consume pre-existing error states.
         */
    }
}


/* ============================================================
 * OPENGL ES ERROR NAME
 * ============================================================ */

static const char *GS_GLES31_error_name(
    GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";

        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";

        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";

        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";

        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";

        default:
            return "UNKNOWN_GL_ERROR";
    }
}


/* ============================================================
 * SHADER COMPILATION
 * ============================================================
 *
 * This is common OpenGL ES functionality and therefore does not
 * depend on the mechanism used to create the context.
 */

static GLuint GS_GLES31_compile_shader(
    GLenum shader_type,
    const char *source)
{
    GLuint shader;
    GLint compile_status = GL_FALSE;

    GLchar info_log[1024];
    GLsizei info_log_length = 0;


    if (source == NULL)
    {
        return 0;
    }


    shader = glCreateShader(
        shader_type
    );


    if (shader == 0)
    {
        return 0;
    }


    glShaderSource(
        shader,
        1,
        &source,
        NULL
    );


    glCompileShader(
        shader
    );


    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &compile_status
    );


    if (compile_status != GL_TRUE)
    {
        glGetShaderInfoLog(
            shader,
            (GLsizei)sizeof(info_log),
            &info_log_length,
            info_log
        );


        fprintf(
            stderr,
            "OpenGL ES shader compilation failed:\n%s\n",
            info_log
        );


        glDeleteShader(
            shader
        );


        return 0;
    }


    return shader;
}


/* ============================================================
 * COMPUTE PROGRAM CREATION
 * ============================================================
 *
 * Creates a program containing one compute shader.
 *
 * The supplied source is expected to use the GLSL ES version
 * required by the procedure, normally:
 *
 *     #version 310 es
 */

static GLuint GS_GLES31_create_compute_program(
    const char *compute_shader_source)
{
    GLuint shader;
    GLuint program;

    GLint link_status = GL_FALSE;

    GLchar info_log[1024];
    GLsizei info_log_length = 0;


    shader = GS_GLES31_compile_shader(
        GL_COMPUTE_SHADER,
        compute_shader_source
    );


    if (shader == 0)
    {
        return 0;
    }


    program = glCreateProgram();


    if (program == 0)
    {
        glDeleteShader(
            shader
        );

        return 0;
    }


    glAttachShader(
        program,
        shader
    );


    glLinkProgram(
        program
    );


    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &link_status
    );


    /*
     * The shader object is no longer needed after the program
     * has been linked.
     */
    glDetachShader(
        program,
        shader
    );


    glDeleteShader(
        shader
    );


    if (link_status != GL_TRUE)
    {
        glGetProgramInfoLog(
            program,
            (GLsizei)sizeof(info_log),
            &info_log_length,
            info_log
        );


        fprintf(
            stderr,
            "OpenGL ES compute program linking failed:\n%s\n",
            info_log
        );


        glDeleteProgram(
            program
        );


        return 0;
    }


    return program;
}


/* ============================================================
 * GRAPHICS-ONLY PROGRAM CREATION
 * ============================================================
 *
 * This helper is primarily intended for negative
 * glDispatchCompute tests.
 *
 * It creates a valid linked program containing graphics shader
 * stages but no compute shader executable.
 *
 * This allows a test to establish:
 *
 *     valid program
 *     +
 *     no compute shader
 *
 * without introducing unrelated invalid state.
 */

static GLuint GS_GLES31_create_graphics_program(
    const char *vertex_shader_source,
    const char *fragment_shader_source)
{
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;

    GLint link_status = GL_FALSE;

    GLchar info_log[1024];
    GLsizei info_log_length = 0;


    if (vertex_shader_source == NULL ||
        fragment_shader_source == NULL)
    {
        return 0;
    }


    vertex_shader = GS_GLES31_compile_shader(
        GL_VERTEX_SHADER,
        vertex_shader_source
    );


    if (vertex_shader == 0)
    {
        return 0;
    }


    fragment_shader = GS_GLES31_compile_shader(
        GL_FRAGMENT_SHADER,
        fragment_shader_source
    );


    if (fragment_shader == 0)
    {
        glDeleteShader(
            vertex_shader
        );

        return 0;
    }


    program = glCreateProgram();


    if (program == 0)
    {
        glDeleteShader(
            vertex_shader
        );

        glDeleteShader(
            fragment_shader
        );

        return 0;
    }


    glAttachShader(
        program,
        vertex_shader
    );


    glAttachShader(
        program,
        fragment_shader
    );


    glLinkProgram(
        program
    );


    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &link_status
    );


    glDetachShader(
        program,
        vertex_shader
    );


    glDetachShader(
        program,
        fragment_shader
    );


    glDeleteShader(
        vertex_shader
    );


    glDeleteShader(
        fragment_shader
    );


    if (link_status != GL_TRUE)
    {
        glGetProgramInfoLog(
            program,
            (GLsizei)sizeof(info_log),
            &info_log_length,
            info_log
        );


        fprintf(
            stderr,
            "OpenGL ES graphics program linking failed:\n%s\n",
            info_log
        );


        glDeleteProgram(
            program
        );


        return 0;
    }


    return program;
}


/* ============================================================
 * SHADER STORAGE BUFFER CREATION
 * ============================================================
 *
 * Creates and binds an SSBO.
 *
 * This helper does not perform synchronization.  Memory barrier
 * selection belongs to the individual test procedure because
 * the required barrier depends on how the test subsequently
 * accesses the resource.
 */

static GLuint GS_GLES31_create_ssbo(
    GLuint binding_point,
    GLsizeiptr size,
    const void *initial_data,
    GLenum usage)
{
    GLuint buffer = 0;
    GLenum error;


    if (size <= 0)
    {
        return 0;
    }


    /*
     * Setup helpers may discard errors produced before they
     * started.  A test checking an API error shall clear the
     * error state again immediately before the function under
     * test.
     */
    GS_GLES31_clear_errors();


    glGenBuffers(
        1,
        &buffer
    );


    if (buffer == 0)
    {
        return 0;
    }


    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        buffer
    );


    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        size,
        initial_data,
        usage
    );


    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        binding_point,
        buffer
    );


    error = glGetError();


    if (error != GL_NO_ERROR)
    {
        glDeleteBuffers(
            1,
            &buffer
        );


        return 0;
    }


    return buffer;
}


/* ============================================================
 * SSBO BINDING
 * ============================================================ */

static void GS_GLES31_bind_ssbo(
    GLuint buffer,
    GLuint binding_point)
{
    glBindBuffer(
        GL_SHADER_STORAGE_BUFFER,
        buffer
    );


    glBindBufferBase(
        GL_SHADER_STORAGE_BUFFER,
        binding_point,
        buffer
    );
}


/* ============================================================
 * PROGRAM CLEANUP
 * ============================================================ */

static void GS_GLES31_delete_program(
    GLuint *program)
{
    if (program == NULL)
    {
        return;
    }


    if (*program != 0)
    {
        glDeleteProgram(
            *program
        );


        *program = 0;
    }
}


/* ============================================================
 * BUFFER CLEANUP
 * ============================================================ */

static void GS_GLES31_delete_buffer(
    GLuint *buffer)
{
    if (buffer == NULL)
    {
        return;
    }


    if (*buffer != 0)
    {
        glDeleteBuffers(
            1,
            buffer
        );


        *buffer = 0;
    }
}


#endif