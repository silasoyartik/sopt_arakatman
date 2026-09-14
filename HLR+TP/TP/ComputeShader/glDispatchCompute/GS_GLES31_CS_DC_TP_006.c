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
 * GS_GLES31_CS_DC_TP_006
 * HLR: GS-GLES31-CS-DC-004.
 * Dispatch (1,1,1) with a linked graphics-only program: INVALID_OPERATION.
 *
 * Runner: current OpenGL ES >= 3.1 context, loaded entry points, clean GL
 * error state. Call init/draw/close in order, even after failed setup, and
 * keep the same context current through close. This TP uses no helper module.
 * It changes program/pipeline and any SSBO bindings used below; the next TP
 * must establish its own bindings. Only resources owned by this TP are deleted.
 */

static const char *test_case = "GS_GLES31_CS_DC_TC_004";
static const char *test_procedure = "GS_GLES31_CS_DC_TP_006";
static GLuint shaders[2], program;

static const char *vertex_shader_source =
    "#version 310 es\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "}\n";

static const char *fragment_shader_source =
    "#version 310 es\n"
    "precision mediump float;\n"
    "layout(location = 0) out vec4 fragmentColor;\n"
    "void main()\n"
    "{\n"
    "    fragmentColor = vec4(1.0);\n"
    "}\n";


void GS_GLES31_CS_DC_TP_006_init(void)
{
    const GLubyte *version;
    GLint major = 0, minor = 0;
    GLenum error;
    GLboolean success = GL_FALSE;
    const char *failure = "Test setup did not complete";
    GLint status = GL_FALSE;
    GLint active_program = 0, active_pipeline = 0;
    char info_log[512] = {0};
    const GLenum stages[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
    const char *sources[] = {vertex_shader_source, fragment_shader_source};
    GLint limits[3] = {0, 0, 0};
    GLuint index;

    if (shaders[0] != 0 || shaders[1] != 0 || program != 0)
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

    for (index = 0; index < 3; ++index)
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, index, &limits[index]);
    if (glGetError() != GL_NO_ERROR || limits[0] < (GLint)(1) ||
        limits[1] < (GLint)(1) || limits[2] < (GLint)(1))
    {
        failure = "Could not establish valid dispatch limits";
        goto report;
    }

    for (index = 0; index < 2; ++index)
    {
        shaders[index] = glCreateShader(stages[index]);
        if (glGetError() != GL_NO_ERROR || shaders[index] == 0)
        {
            failure = "Could not create a graphics shader";
            goto report;
        }
        glShaderSource(shaders[index], 1, &sources[index], NULL);
        glCompileShader(shaders[index]);
        status = GL_FALSE;
        glGetShaderiv(shaders[index], GL_COMPILE_STATUS, &status);
        if (glGetError() != GL_NO_ERROR || status != GL_TRUE)
        {
            glGetShaderInfoLog(shaders[index], (GLsizei)sizeof(info_log), NULL, info_log);
            TEST_LOG_INFO("Graphics shader %u: %s", index, info_log);
            failure = "Graphics shader compilation failed";
            goto report;
        }
    }
    program = glCreateProgram();
    if (glGetError() != GL_NO_ERROR || program == 0)
    {
        failure = "Could not create the graphics program";
        goto report;
    }
    glAttachShader(program, shaders[0]);
    glAttachShader(program, shaders[1]);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (glGetError() != GL_NO_ERROR || status != GL_TRUE)
    {
        glGetProgramInfoLog(program, (GLsizei)sizeof(info_log), NULL, info_log);
        TEST_LOG_INFO("Graphics-only program linking: %s", info_log);
        failure = "Graphics-only program linking failed";
        goto report;
    }

    glBindProgramPipeline(0);
    glUseProgram(program);
    glGetIntegerv(GL_CURRENT_PROGRAM, &active_program);
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &active_pipeline);
    if (glGetError() != GL_NO_ERROR || (GLuint)active_program != program ||
        active_pipeline != 0)
    {
        failure = "The linked graphics-only program could not be made active";
        goto report;
    }

    /* Test Case #004: a graphics-only program must cause INVALID_OPERATION. */
    glDispatchCompute(1, 1, 1);
    error = glGetError();
    success = (error == GL_INVALID_OPERATION);
    failure = "Unexpected dispatch error result";
    if (!success)
        TEST_LOG_INFO("Dispatch: expected error 0x%x, got 0x%x", GL_INVALID_OPERATION, error);
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        success = GL_FALSE;
        failure = "Unexpected additional dispatch error";
        TEST_LOG_INFO("Additional dispatch error: 0x%x", error);
    }

report:
    if (success)
        TEST_LOG_SUCCESS(test_case, test_procedure);
    else
        TEST_LOG_FAIL(test_case, test_procedure, "%s", failure);
}

void GS_GLES31_CS_DC_TP_006_draw(void) {}

void GS_GLES31_CS_DC_TP_006_close(void)
{
    GLenum error;
    GLuint index;
    if (program == 0 && shaders[0] == 0 && shaders[1] == 0)
        return;
    if (program != 0)
    {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
    for (index = 0; index < 2; ++index)
    {
        if (shaders[index] != 0)
        {
            glDeleteShader(shaders[index]);
            shaders[index] = 0;
        }
    }
    while ((error = glGetError()) != GL_NO_ERROR)
        TEST_LOG_FAIL(test_case, test_procedure, "GL error during cleanup: 0x%x", error);
}
