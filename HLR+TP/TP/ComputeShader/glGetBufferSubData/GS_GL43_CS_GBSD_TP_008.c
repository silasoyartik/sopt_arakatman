#ifdef GS_GL43_API_HEADER
#include GS_GL43_API_HEADER
#else
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/glcorearb.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirement: GS-GL43-CS-GBSD-011
 * Use ordinary nonpersistent mapping of a mutable buffer, preserving a valid read interval.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GL43_CS_GBSD_TC_011";
static const char *test_procedure = "GS_GL43_CS_GBSD_TP_008";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GL43_CS_GBSD_TP_008_init(void)
{
    GLenum actual_error, extra_error;
    GLuint buffers[4] = {0};
    GLenum mapped_target = 0;
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

    const unsigned char data[16] = {3,  17,  29,  41,  53,  67,  79,  83,
                                    97, 109, 127, 139, 151, 163, 179, 193};
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
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[0]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }

    unsigned char out[4] = {0};
    void *mapped = glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(data), GL_MAP_READ_BIT);
    if (mapped)
        mapped_target = GL_COPY_READ_BUFFER;
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (mapped == NULL)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Mapping setup failed");
        goto finish;
    }
    GLint mapped_state = GL_FALSE;
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_MAPPED, &mapped_state);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (mapped_state != GL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Mapped precondition missing");
        goto finish;
    }
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(out), out);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_INVALID_OPERATION || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Expected INVALID_OPERATION for mapped buffer",
                      (unsigned)GL_INVALID_OPERATION, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    GLboolean intact = glUnmapBuffer(GL_COPY_READ_BUFFER);
    mapped_target = 0;
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (intact != GL_TRUE)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Unmap reported data corruption");
        goto finish;
    }

    test_result = 1;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
    if (mapped_target != 0)
    {
        if (glUnmapBuffer(mapped_target) != GL_TRUE)
        {
            cleanup_ok = 0;
            TEST_LOG_FAIL(test_case, test_procedure, "Cleanup unmap failed");
        }
    }
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
void GS_GL43_CS_GBSD_TP_008_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GL43_CS_GBSD_TP_008_close(void) {}
int GS_GL43_CS_GBSD_TP_008_result(void) { return test_result; }
