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

/* Covered requirement: GS-GL43-CS-GBSD-004
 * Exercise all thirteen GL 4.3 buffer targets; use a real VAO for element-array binding.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GL43_CS_GBSD_TC_004";
static const char *test_procedure = "GS_GL43_CS_GBSD_TP_003";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GL43_CS_GBSD_TP_003_init(void)
{
    GLenum actual_error, extra_error;
    GLuint buffers[4] = {0};
    GLuint vao = 0;
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

    const GLenum targets[] = {
        GL_ARRAY_BUFFER,          GL_ATOMIC_COUNTER_BUFFER,    GL_COPY_READ_BUFFER,
        GL_COPY_WRITE_BUFFER,     GL_DISPATCH_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,  GL_PIXEL_PACK_BUFFER,        GL_PIXEL_UNPACK_BUFFER,
        GL_SHADER_STORAGE_BUFFER, GL_TEXTURE_BUFFER,           GL_TRANSFORM_FEEDBACK_BUFFER,
        GL_UNIFORM_BUFFER};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (vao == 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Core-profile VAO setup failed");
        goto finish;
    }
    for (unsigned i = 0; i < sizeof(targets) / sizeof(targets[0]); ++i)
    {
        unsigned char out[16] = {0};
        glBindBuffer(targets[i], buffers[0]);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        glGetBufferSubData(targets[i], 0, sizeof(out), out);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (!(memcmp(out, data, sizeof(out)) == 0))
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Valid target did not return buffer data");
            goto finish;
        }
    }

    test_result = 1;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
    glDeleteBuffers(4, buffers);
    if (vao != 0)
    {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vao);
    }
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
void GS_GL43_CS_GBSD_TP_003_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GL43_CS_GBSD_TP_003_close(void) {}
int GS_GL43_CS_GBSD_TP_003_result(void) { return test_result; }
