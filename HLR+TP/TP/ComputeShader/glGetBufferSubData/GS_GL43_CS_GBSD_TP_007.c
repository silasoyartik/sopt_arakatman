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

/* Covered requirements:
 * GS-GL43-CS-GBSD-008
 * GS-GL43-CS-GBSD-009
 * GS-GL43-CS-GBSD-010
 * Isolate each invalid range argument in separate calls on one valid unmapped buffer.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_procedure = "GS_GL43_CS_GBSD_TP_007";
static const char *test_cases[] = {"GS_GL43_CS_GBSD_TC_008", "GS_GL43_CS_GBSD_TC_009",
                                   "GS_GL43_CS_GBSD_TC_010"};
static const unsigned requirement_numbers[] = {8, 9, 10};
static int requirement_results[3] = {-1, -1, -1};

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GL43_CS_GBSD_TP_007_init(void)
{
    GLenum actual_error, extra_error;
    GLuint buffers[4] = {0};
    int cleanup_ok = 1;
    test_result = 0; /* No early exit may report success. */
    const char *test_case = test_cases[0];
    int setup_complete = 0;
    for (unsigned i = 0; i < 3; ++i)
        requirement_results[i] = -1;
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
    unsigned char out[32] = {0};
    GLint64 actual_size = 0;
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
    glGetBufferParameteri64v(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &actual_size);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (!(actual_size == sizeof(data)))
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer-size precondition failed");
        goto finish;
    }
    setup_complete = 1;

    requirement_results[0] = 0;
    glGetBufferSubData(GL_COPY_READ_BUFFER, (GLintptr)-1, 4, out);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_INVALID_VALUE || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Expected INVALID_VALUE for negative offset", (unsigned)GL_INVALID_VALUE,
                      (unsigned)actual_error, (unsigned)extra_error);
        goto finish;
    }
    requirement_results[0] = 1;

    test_case = test_cases[1];
    requirement_results[1] = 0;
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, (GLsizeiptr)-1, out);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_INVALID_VALUE || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Expected INVALID_VALUE for negative size", (unsigned)GL_INVALID_VALUE,
                      (unsigned)actual_error, (unsigned)extra_error);
        goto finish;
    }
    requirement_results[1] = 1;

    test_case = test_cases[2];
    requirement_results[2] = 0;
    glGetBufferSubData(GL_COPY_READ_BUFFER, (GLintptr)actual_size - 1, 2, out);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_INVALID_VALUE || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Expected INVALID_VALUE for end overrun", (unsigned)GL_INVALID_VALUE,
                      (unsigned)actual_error, (unsigned)extra_error);
        goto finish;
    }
    glGetBufferSubData(GL_COPY_READ_BUFFER, 0, (GLsizeiptr)actual_size + 1, out);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_INVALID_VALUE || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Expected INVALID_VALUE for oversized interval", (unsigned)GL_INVALID_VALUE,
                      (unsigned)actual_error, (unsigned)extra_error);
        goto finish;
    }
    requirement_results[2] = 1;

    test_result = 1;
    for (unsigned i = 0; i < 3; ++i)
        if (requirement_results[i] != 1)
            test_result = 0;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
    if (!setup_complete)
        for (unsigned i = 0; i < 3; ++i)
            requirement_results[i] = 0;
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
    if (!cleanup_ok)
        for (unsigned i = 0; i < 3; ++i)
            if (requirement_results[i] == 1)
                requirement_results[i] = 0;
    for (unsigned i = 0; i < 3; ++i)
    {
        if (requirement_results[i] == 1)
            TEST_LOG_SUCCESS(test_cases[i], test_procedure);
        else if (requirement_results[i] == 0)
            TEST_LOG_FAIL(test_cases[i], test_procedure,
                          "Requirement check or shared setup/cleanup failed");
        else
            TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_cases[i],
                          test_procedure);
    }
}
void GS_GL43_CS_GBSD_TP_007_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GL43_CS_GBSD_TP_007_close(void) {}
int GS_GL43_CS_GBSD_TP_007_result(void) { return test_result; }
/* The argument is the HLR suffix, not the TP number or an array index. */
int GS_GL43_CS_GBSD_TP_007_requirement_result(unsigned requirement_number)
{
    for (unsigned i = 0; i < 3; ++i)
        if (requirement_numbers[i] == requirement_number)
            return requirement_results[i];
    return -1;
}
