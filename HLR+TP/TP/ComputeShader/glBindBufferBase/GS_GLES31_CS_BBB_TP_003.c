#ifdef GS_GLES31_API_HEADER
#include GS_GLES31_API_HEADER
#else
#include <GLES3/gl31.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirement: GS-GLES31-CS-BBB-005
 * Test a generated but never bound name and an unused nonzero name in an isolated context.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GLES31_CS_BBB_TC_005";
static const char *test_procedure = "GS_GLES31_CS_BBB_TP_003";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GLES31_CS_BBB_TP_003_init(void)
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

    for (int subcase = 0; subcase < 2; ++subcase)
    {
        GLuint *name = &buffers[subcase];
        GLint value = -1;
        GLint64 wide = -1;
        void *pointer = (void *)&value;
        if (subcase == 0)
        {
            glGenBuffers(1, name); /* Deliberately no glBindBuffer/glBufferData. */
        }
        else
        {
            *name = buffers[0] == 12345u ? 12346u : 12345u;
        }
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (!(*name != 0 && glIsBuffer(*name) == GL_FALSE))
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Name already has object state");
            goto finish;
        }
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *name);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (!(glIsBuffer(*name) == GL_TRUE))
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "First binding did not create object state");
            goto finish;
        }
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        const GLenum zero_wide[] = {GL_BUFFER_SIZE, GL_BUFFER_MAP_OFFSET, GL_BUFFER_MAP_LENGTH};
        for (unsigned i = 0; i < sizeof(zero_wide) / sizeof(zero_wide[0]); ++i)
        {
            glGetBufferParameteri64v(GL_SHADER_STORAGE_BUFFER, zero_wide[i], &wide);
            actual_error = glGetError();
            extra_error = glGetError();
            if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                              "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                              (unsigned)extra_error);
                goto finish;
            }
            if (wide != 0)
            {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Initial size/map range is not zero");
                goto finish;
            }
        }
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_USAGE, &value);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (value != GL_STATIC_DRAW)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial usage is not STATIC_DRAW");
            goto finish;
        }
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_ACCESS_FLAGS, &value);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (value != 0)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial access flags are not zero");
            goto finish;
        }
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_MAPPED, &value);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (value != GL_FALSE)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "New object is mapped");
            goto finish;
        }
        glGetBufferPointerv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_MAP_POINTER, &pointer);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (pointer != NULL)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial map pointer is not NULL");
            goto finish;
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
void GS_GLES31_CS_BBB_TP_003_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GLES31_CS_BBB_TP_003_close(void) {}
int GS_GLES31_CS_BBB_TP_003_result(void) { return test_result; }
