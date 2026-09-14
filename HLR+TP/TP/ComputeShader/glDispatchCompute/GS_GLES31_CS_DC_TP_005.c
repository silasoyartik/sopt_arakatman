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
 * GS_GLES31_CS_DC_TP_005
 * HLR: GS-GLES31-CS-DC-003.
 * Dispatch (1,1,1) without any active compute executable: INVALID_OPERATION.
 *
 * Runner: current OpenGL ES >= 3.1 context, loaded entry points, clean GL
 * error state. Call init/draw/close in order, even after failed setup, and
 * keep the same context current through close. This TP uses no helper module.
 * It changes program/pipeline and any SSBO bindings used below; the next TP
 * must establish its own bindings. Only resources owned by this TP are deleted.
 */

static const char *test_case = "GS_GLES31_CS_DC_TC_003";
static const char *test_procedure = "GS_GLES31_CS_DC_TP_005";


void GS_GLES31_CS_DC_TP_005_init(void)
{
    const GLubyte *version;
    GLint major = 0, minor = 0;
    GLenum error;
    GLboolean success = GL_FALSE;
    const char *failure = "Test setup did not complete";
    GLint active_program = 0, active_pipeline = 0;
    GLint limits[3] = {0, 0, 0};
    GLuint index;

#if defined(GS_GLES31_USE_GLAD1) || defined(GS_GLES31_USE_GLAD2)
    if (glBindProgramPipeline == NULL ||
        glDispatchCompute == NULL ||
        glGetError == NULL ||
        glGetIntegeri_v == NULL ||
        glGetIntegerv == NULL ||
        glGetString == NULL ||
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

    glBindProgramPipeline(0);
    glUseProgram(0);
    glGetIntegerv(GL_CURRENT_PROGRAM, &active_program);
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &active_pipeline);
    if (glGetError() != GL_NO_ERROR || active_program != 0 ||
        active_pipeline != 0)
    {
        failure = "Could not clear the active program and pipeline";
        goto report;
    }

    /* Test Case #003: no active compute executable must cause INVALID_OPERATION. */
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

void GS_GLES31_CS_DC_TP_005_draw(void) {}

void GS_GLES31_CS_DC_TP_005_close(void)
{
    /* No GL resources were created by this test. */
}
