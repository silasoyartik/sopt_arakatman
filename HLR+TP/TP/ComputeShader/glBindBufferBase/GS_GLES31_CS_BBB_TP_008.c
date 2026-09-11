#ifdef GS_GLES31_API_HEADER
#include GS_GLES31_API_HEADER
#else
#include <GLES3/gl31.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirement: GS-GLES31-CS-BBB-010
 * Check index==limit and index>limit for each target with otherwise valid inputs.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GLES31_CS_BBB_TC_010";
static const char *test_procedure = "GS_GLES31_CS_BBB_TP_008";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GLES31_CS_BBB_TP_008_init(void)
{
    GLenum actual_error, extra_error;
    GLuint buffers[4] = {0};
    int cleanup_ok = 1;
    test_result = 0; /* No early exit may report success. */
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Pre-existing GL error before test setup", (unsigned)GL_NO_ERROR,
                      (unsigned)actual_error, (unsigned)extra_error);
        goto finish;
    }

    const GLenum targets[] = {GL_ATOMIC_COUNTER_BUFFER, GL_SHADER_STORAGE_BUFFER,
                              GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER};
    const GLenum bindings[] = {GL_ATOMIC_COUNTER_BUFFER_BINDING, GL_SHADER_STORAGE_BUFFER_BINDING,
                               GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, GL_UNIFORM_BUFFER_BINDING};
    const GLenum limits[] = {
        GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
        GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, GL_MAX_UNIFORM_BUFFER_BINDINGS};
    const GLuint data[4] = {1, 2, 3, 4};
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[0]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (buffers[0] == 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer setup failed");
        goto finish;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Buffer setup failed", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }

    for (unsigned t = 0; t < 4; ++t)
    {
        GLint limit = 0;
        glGetIntegerv(limits[t], &limit);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (!(limit > 0))
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Invalid runtime binding limit");
            goto finish;
        }
        {
            GLint actual_general = -1, actual_indexed = -1;
            glGetIntegerv(bindings[t], &actual_general);
            glGetIntegeri_v(bindings[t], 0, &actual_indexed);
            actual_error = glGetError();
            extra_error = glGetError();
            if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                              "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                              (unsigned)extra_error);
                goto finish;
            }
            if (!((GLuint)actual_general == 0 && (GLuint)actual_indexed == 0))
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial binding state not clean");
                goto finish;
            }
        }
        for (GLuint extra = 0; extra < 2; ++extra)
        {
            glBindBufferBase(targets[t], (GLuint)limit + extra, buffers[0]);
            actual_error = glGetError();
            extra_error = glGetError();
            if (actual_error != GL_INVALID_VALUE || extra_error != GL_NO_ERROR)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                              "Expected INVALID_VALUE at/above binding limit",
                              (unsigned)GL_INVALID_VALUE, (unsigned)actual_error,
                              (unsigned)extra_error);
                goto finish;
            }
        }
    }

    test_result = 1;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
    glDeleteBuffers(4, buffers);
    {
        GLenum cleanup_error = glGetError();
        GLenum extra_error = glGetError();
        if (cleanup_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            cleanup_ok = 0;
            TEST_LOG_FAIL(test_case, test_procedure, "Cleanup GL error: 0x%x / 0x%x",
                          (unsigned)cleanup_error, (unsigned)extra_error);
        }
    }
    if (!cleanup_ok)
        test_result = 0;
    if (test_result == 1)
        TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_GLES31_CS_BBB_TP_008_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GLES31_CS_BBB_TP_008_close(void) {}
int GS_GLES31_CS_BBB_TP_008_result(void) { return test_result; }
