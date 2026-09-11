#include <glad/glad.h>
#include <stdint.h>
#include <stdio.h>

/*
 * glGetBufferSubData belongs to desktop OpenGL, not OpenGL ES 3.1.
 *
 * With this project's OpenGL ES 3.1 GLAD header, GL_VERSION_4_3 is
 * not defined and only the availability scenario below is compiled.
 *
 * Compile this file with a desktop OpenGL 4.3 GLAD header to enable
 * the readback and error scenarios in the second branch.
 */

#if !defined(GL_VERSION_4_3)

void scenario_get_buffer_sub_data_not_available_in_es31(void)
{
    printf("Scenario 1 - glGetBufferSubData availability\n");
    printf("  Result: SKIPPED\n");
    printf("  Reason: glGetBufferSubData is not an OpenGL ES 3.1 core command.\n");
    printf("  Use glMapBufferRange(..., GL_MAP_READ_BIT) for ES 3.1 readback.\n\n");
}

void run_all_get_buffer_sub_data_scenarios(void)
{
    printf("\n===============================================\n");
    printf("glGetBufferSubData() Scenario Demonstration\n");
    printf("OpenGL ES 3.1 build\n");
    printf("===============================================\n\n");

    scenario_get_buffer_sub_data_not_available_in_es31();

    printf("===============================================\n");
    printf("Scenario demonstration completed.\n");
    printf("===============================================\n");
}

#else

static const char *readback_error_name(GLenum error)
{
    switch (error) {
    case GL_NO_ERROR:          return "GL_NO_ERROR";
    case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
    default:                   return "UNKNOWN_GL_ERROR";
    }
}

static void clear_readback_errors(void)
{
    while (glGetError() != GL_NO_ERROR) {
    }
}

static GLuint create_readback_buffer(void)
{
    static const GLuint values[] = { 10u, 20u, 30u, 40u };
    GLuint buffer = 0;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_COPY_READ_BUFFER, buffer);
    glBufferData(GL_COPY_READ_BUFFER, sizeof(values), values, GL_STATIC_READ);

    return buffer;
}

/*
 * Scenario 1
 *
 * Read the complete buffer data store into CPU memory.
 */
void scenario_get_buffer_sub_data_complete_store(void)
{
    GLuint actual[4] = { 0u };
    GLuint buffer = create_readback_buffer();

    clear_readback_errors();
    glGetBufferSubData(
        GL_COPY_READ_BUFFER,
        0,
        sizeof(actual),
        actual
    );
    GLenum error = glGetError();

    printf("Scenario 1 - Complete buffer readback\n");
    printf("  Expected error : GL_NO_ERROR\n");
    printf("  Actual error   : %s\n", readback_error_name(error));
    printf("  Expected data  : { 10, 20, 30, 40 }\n");
    printf("  Actual data    : { %u, %u, %u, %u }\n\n",
           actual[0], actual[1], actual[2], actual[3]);

    glDeleteBuffers(1, &buffer);
}

/*
 * Scenario 2
 *
 * Read only values at indices two and three by selecting a
 * byte offset and a smaller size.
 */
void scenario_get_buffer_sub_data_subset(void)
{
    GLuint actual[2] = { 0u };
    GLuint buffer = create_readback_buffer();

    clear_readback_errors();
    glGetBufferSubData(
        GL_COPY_READ_BUFFER,
        2 * (GLintptr)sizeof(GLuint),
        sizeof(actual),
        actual
    );
    GLenum error = glGetError();

    printf("Scenario 2 - Buffer subset readback\n");
    printf("  Offset         : %zu bytes\n", 2u * sizeof(GLuint));
    printf("  Expected error : GL_NO_ERROR\n");
    printf("  Actual error   : %s\n", readback_error_name(error));
    printf("  Expected data  : { 30, 40 }\n");
    printf("  Actual data    : { %u, %u }\n\n", actual[0], actual[1]);

    glDeleteBuffers(1, &buffer);
}

/*
 * Scenario 3
 *
 * GL_FRAMEBUFFER is not a buffer object target.
 *
 * Expected error:
 *     GL_INVALID_ENUM
 */
void scenario_get_buffer_sub_data_invalid_target(void)
{
    GLuint actual = 0u;

    clear_readback_errors();
    glGetBufferSubData(GL_FRAMEBUFFER, 0, sizeof(actual), &actual);
    GLenum error = glGetError();

    printf("Scenario 3 - Invalid target\n");
    printf("  Expected error : GL_INVALID_ENUM\n");
    printf("  Actual error   : %s\n\n", readback_error_name(error));
}

/*
 * Scenario 4
 *
 * A negative offset is invalid.
 *
 * Expected error:
 *     GL_INVALID_VALUE
 */
void scenario_get_buffer_sub_data_negative_offset(void)
{
    GLuint actual = 0u;
    GLuint buffer = create_readback_buffer();

    clear_readback_errors();
    glGetBufferSubData(GL_COPY_READ_BUFFER, -1, sizeof(actual), &actual);
    GLenum error = glGetError();

    printf("Scenario 4 - Negative offset\n");
    printf("  Expected error : GL_INVALID_VALUE\n");
    printf("  Actual error   : %s\n\n", readback_error_name(error));

    glDeleteBuffers(1, &buffer);
}

/*
 * Scenario 5
 *
 * offset + size extends beyond the data store.
 *
 * Expected error:
 *     GL_INVALID_VALUE
 */
void scenario_get_buffer_sub_data_range_out_of_bounds(void)
{
    GLuint actual[2] = { 0u };
    GLuint buffer = create_readback_buffer();

    clear_readback_errors();
    glGetBufferSubData(
        GL_COPY_READ_BUFFER,
        3 * (GLintptr)sizeof(GLuint),
        sizeof(actual),
        actual
    );
    GLenum error = glGetError();

    printf("Scenario 5 - Requested range exceeds buffer size\n");
    printf("  Expected error : GL_INVALID_VALUE\n");
    printf("  Actual error   : %s\n\n", readback_error_name(error));

    glDeleteBuffers(1, &buffer);
}

/*
 * Scenario 6
 *
 * Target has buffer name zero bound.
 *
 * Expected error:
 *     GL_INVALID_OPERATION
 */
void scenario_get_buffer_sub_data_zero_buffer_bound(void)
{
    GLuint actual = 0u;

    glBindBuffer(GL_COPY_READ_BUFFER, 0);

    clear_readback_errors();
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(actual), &actual);
    GLenum error = glGetError();

    printf("Scenario 6 - Reserved buffer name zero bound\n");
    printf("  Expected error : GL_INVALID_OPERATION\n");
    printf("  Actual error   : %s\n\n", readback_error_name(error));
}

/*
 * Scenario 7
 *
 * Reading a buffer while it is mapped is invalid.
 *
 * Expected error:
 *     GL_INVALID_OPERATION
 */
void scenario_get_buffer_sub_data_mapped_buffer(void)
{
    GLuint actual = 0u;
    GLuint buffer = create_readback_buffer();
    void *mapping = glMapBufferRange(
        GL_COPY_READ_BUFFER,
        0,
        sizeof(GLuint),
        GL_MAP_READ_BIT
    );

    if (mapping == NULL) {
        printf("Scenario 7 - Mapped buffer: SKIPPED (map failed).\n\n");
        glDeleteBuffers(1, &buffer);
        return;
    }

    clear_readback_errors();
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(actual), &actual);
    GLenum error = glGetError();

    printf("Scenario 7 - Readback from mapped buffer\n");
    printf("  Expected error : GL_INVALID_OPERATION\n");
    printf("  Actual error   : %s\n\n", readback_error_name(error));

    glUnmapBuffer(GL_COPY_READ_BUFFER);
    glDeleteBuffers(1, &buffer);
}

void run_all_get_buffer_sub_data_scenarios(void)
{
    printf("\n===============================================\n");
    printf("glGetBufferSubData() Scenario Demonstration\n");
    printf("Desktop OpenGL 4.3 Core build\n");
    printf("===============================================\n\n");

    scenario_get_buffer_sub_data_complete_store();
    scenario_get_buffer_sub_data_subset();
    scenario_get_buffer_sub_data_invalid_target();
    scenario_get_buffer_sub_data_negative_offset();
    scenario_get_buffer_sub_data_range_out_of_bounds();
    scenario_get_buffer_sub_data_zero_buffer_bound();
    scenario_get_buffer_sub_data_mapped_buffer();

    printf("===============================================\n");
    printf("Scenario demonstration completed.\n");
    printf("===============================================\n");
}

#endif
