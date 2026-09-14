#include <string.h>

#if defined(GS_GLES31_USE_GLAD1) && defined(GS_GLES31_USE_GLAD2)
#error "Select only one GLAD loader."
#elif defined(GS_GLES31_USE_GLAD1)
#include <glad/glad.h>
#elif defined(GS_GLES31_USE_GLAD2)
#include <glad/gles2.h>
#else
#ifndef GL_GLES_PROTOTYPES
#define GL_GLES_PROTOTYPES 1
#endif
#include <GLES3/gl31.h>
#endif

#include "../../macros.h"

/*
 * GS_GLES31_CS_DC_TP_008
 * HLR: GS-GLES31-CS-DC-006, GS-GLES31-CS-DC-007.
 * Exceed Y and Z limits independently; each must return INVALID_VALUE.
 *
 * Runner: current OpenGL ES >= 3.1 context, loaded entry points, clean GL
 * error state. Call init/draw/close in order, even after failed setup, and
 * keep the same context current through close. This TP uses no helper module.
 * It changes program/pipeline and any SSBO bindings used below; the next TP
 * must establish its own bindings. Only resources owned by this TP are deleted.
 */

static const char *test_cases[] = {"GS_GLES31_CS_DC_TC_006", "GS_GLES31_CS_DC_TC_007"};
static const char *test_procedure = "GS_GLES31_CS_DC_TP_008";
static GLuint shader, program;

static const char *compute_shader_source =
    "#version 310 es\n"
    "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "void main() {}\n";

void GS_GLES31_CS_DC_TP_008_init(void)
{
    const GLubyte *version;
    GLint major = 0, minor = 0;
    GLenum error;
    GLboolean success[2] = {GL_FALSE, GL_FALSE};
    const char *case_failure[2] = {NULL, NULL};
    const char *failure = "Test setup did not complete";
    GLint status = GL_FALSE;
    GLint active_program = 0, active_pipeline = 0;
    char info_log[512] = {0};
    GLuint index;

    if (shader != 0 || program != 0)
    {
        failure = "Previous run must be closed before init is called again";
        goto report;
    }

#if defined(GS_GLES31_USE_GLAD1) || defined(GS_GLES31_USE_GLAD2)
    if (glAttachShader == NULL ||
        glBindProgramPipeline == NULL ||
        glCompileShader == NULL ||
        glCreateProgram == NULL ||
        glCreateShader == NULL ||
        glDeleteProgram == NULL ||
        glDeleteShader == NULL ||
        glDispatchCompute == NULL ||
        glGetError == NULL ||
        glGetIntegeri_v == NULL ||
        glGetIntegerv == NULL ||
        glGetProgramInfoLog == NULL ||
        glGetProgramiv == NULL ||
        glGetShaderInfoLog == NULL ||
        glGetShaderiv == NULL ||
        glGetString == NULL ||
        glLinkProgram == NULL ||
        glShaderSource == NULL ||
        glUseProgram == NULL)
    {
        failure = "Runner has not loaded all required GLES entry points";
        goto report;
    }
#endif

    version = glGetString(GL_VERSION);
    if (version == NULL || strncmp((const char *)version, "OpenGL ES ", 10) != 0)
    {
        failure = "Runner must provide a current OpenGL ES context";
        goto report;
    }
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    error = glGetError();
    if (error != GL_NO_ERROR || major < 3 || (major == 3 && minor < 1))
    {
        failure = "GLES 3.1 or later and a clean initial GL error state are required";
        goto report;
    }

    shader = glCreateShader(GL_COMPUTE_SHADER);
    if (glGetError() != GL_NO_ERROR || shader == 0)
    {
        failure = "Could not create the compute shader";
        goto report;
    }
    glShaderSource(shader, 1, &compute_shader_source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (glGetError() != GL_NO_ERROR || status != GL_TRUE)
    {
        glGetShaderInfoLog(shader, (GLsizei)sizeof(info_log), NULL, info_log);
        TEST_LOG_INFO("Compute shader compilation: %s", info_log);
        failure = "Compute shader compilation failed";
        goto report;
    }

    program = glCreateProgram();
    if (glGetError() != GL_NO_ERROR || program == 0)
    {
        failure = "Could not create the compute program";
        goto report;
    }
    glAttachShader(program, shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (glGetError() != GL_NO_ERROR || status != GL_TRUE)
    {
        glGetProgramInfoLog(program, (GLsizei)sizeof(info_log), NULL, info_log);
        TEST_LOG_INFO("Compute program linking: %s", info_log);
        failure = "Compute program linking failed";
        goto report;
    }

    glBindProgramPipeline(0);
    glUseProgram(program);
    glGetIntegerv(GL_CURRENT_PROGRAM, &active_program);
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &active_pipeline);
    if (glGetError() != GL_NO_ERROR || (GLuint)active_program != program ||
        active_pipeline != 0)
    {
        failure = "The linked compute program could not be made active";
        goto report;
    }

    /* Independent Y and Z cases; consume errors before starting the next. */
    for (index = 0; index < 2; ++index)
    {
        GLint maximum = 0;
        GLuint groups[3] = {1u, 1u, 1u};
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, index + 1u, &maximum);
        error = glGetError();
        if (error != GL_NO_ERROR || maximum <= 0)
        {
            case_failure[index] = "Could not query a positive work-group count limit";
        }
        else
        {
            groups[index + 1u] = (GLuint)maximum + 1u;
            /* Test Case #006: index == 0 exceeds only the Y limit. */
            /* Test Case #007: index == 1 exceeds only the Z limit. */
            glDispatchCompute(groups[0], groups[1], groups[2]);
            error = glGetError();
            success[index] = (error == GL_INVALID_VALUE);
            case_failure[index] = "Dispatch above the axis limit did not generate INVALID_VALUE";
            if (!success[index])
                TEST_LOG_INFO("Axis %u: expected 0x%x, got 0x%x",
                              index + 1u, GL_INVALID_VALUE, error);
        }
        while ((error = glGetError()) != GL_NO_ERROR)
        {
            success[index] = GL_FALSE;
            case_failure[index] = "Additional GL error in the axis test";
            TEST_LOG_INFO("Axis %u: additional error 0x%x", index + 1u, error);
        }
    }

report:
    for (index = 0; index < 2; ++index)
    {
        if (success[index])
            TEST_LOG_SUCCESS(test_cases[index], test_procedure);
        else
            TEST_LOG_FAIL(test_cases[index], test_procedure, "%s",
                          case_failure[index] != NULL ? case_failure[index] : failure);
    }
}

void GS_GLES31_CS_DC_TP_008_draw(void) {}

void GS_GLES31_CS_DC_TP_008_close(void)
{
    GLenum error;

    /* Partial setup and repeated close are safe. No calls before loading
     * the API: nonzero handles exist only after entry-point validation. */
    if (shader == 0 && program == 0)
        return;
    if (program != 0)
    {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
    if (shader != 0)
    {
        glDeleteShader(shader);
        shader = 0;
    }
    while ((error = glGetError()) != GL_NO_ERROR)
        TEST_LOG_FAIL(test_cases[0], test_procedure,
                      "GL error during cleanup: 0x%x", error);
}
