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

/* Covered requirement: GS-GL43-CS-GBSD-007
 * General binding zero with otherwise valid arguments.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GL43_CS_GBSD_TC_007";
static const char *test_procedure = "GS_GL43_CS_GBSD_TP_006";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GL43_CS_GBSD_TP_006_init(void)
{
    GLenum actual_error, extra_error;
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

    unsigned char out[4] = {0};
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    GLint binding = -1;
    glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &binding);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (binding != 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Zero binding precondition failed");
        goto finish;
    }
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(out), out);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_INVALID_OPERATION || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Expected INVALID_OPERATION for zero binding", (unsigned)GL_INVALID_OPERATION,
                      (unsigned)actual_error, (unsigned)extra_error);
        goto finish;
    }

    test_result = 1;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
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
void GS_GL43_CS_GBSD_TP_006_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GL43_CS_GBSD_TP_006_close(void) {}
int GS_GL43_CS_GBSD_TP_006_result(void) { return test_result; }
