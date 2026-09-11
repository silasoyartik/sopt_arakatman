#ifdef GS_GLES31_API_HEADER
#include GS_GLES31_API_HEADER
#else
#include <GLES3/gl31.h>
#endif
#include <stdint.h>
#include <string.h>
#include "../../macros.h"

/* Covered requirements:
 * GS-GLES31-CS-BBB-002
 * GS-GLES31-CS-BBB-003
 * GS-GLES31-CS-BBB-004
 * Check indexed/general replacement and preservation of the newly bound buffer in one fixture.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_procedure = "GS_GLES31_CS_BBB_TP_002";
static const char *test_cases[] = {"GS_GLES31_CS_BBB_TC_002", "GS_GLES31_CS_BBB_TC_003",
                                   "GS_GLES31_CS_BBB_TC_004"};
static const unsigned requirement_numbers[] = {2, 3, 4};
static int requirement_results[3] = {-1, -1, -1};

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GLES31_CS_BBB_TP_002_init(void)
{
    GLenum actual_error, extra_error;
    GLuint buffers[4] = {0};
    GLenum mapped_target = 0;
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

    const GLuint data[4] = {17, 29, 41, 53};
    GLuint observed[4] = {0};
    GLint indexed = -1, general = -1, usage = 0;
    GLint64 size = -1;
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
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Old buffer setup failed");
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
                      "Old buffer setup failed", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[1]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    if (buffers[1] == 0)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "New buffer setup failed");
        goto finish;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[1]);
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
                      "New buffer setup failed", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffers[0], 0, sizeof(data));
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    {
        GLint actual_general = -1, actual_indexed = -1;
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &actual_general);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &actual_indexed);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        if (!((GLuint)actual_general == buffers[0] && (GLuint)actual_indexed == buffers[0]))
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Previous bindings missing");
            goto finish;
        }
    }
    setup_complete = 1;
    for (unsigned i = 0; i < 3; ++i)
        requirement_results[i] = 0;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[1]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    /* No intervening glBindBuffer: query both effects of this exact call. */
    glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &indexed);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    requirement_results[0] = (GLuint)indexed == buffers[1] ? 1 : 0;
    test_case = test_cases[1];
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &general);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    requirement_results[1] = (GLuint)general == buffers[1] ? 1 : 0;

    test_case = test_cases[2];
    /* Query the new object through the copy target, even if general binding is wrong. */
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[1]);
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
    glGetBufferParameteri64v(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    /* Observe buffer bytes by mapping, independently of the function under test. */
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[1]);
    actual_error = glGetError();
    extra_error = glGetError();
    if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                      "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                      (unsigned)extra_error);
        goto finish;
    }
    {
        void *read_pointer =
            glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(observed), GL_MAP_READ_BIT);
        if (read_pointer != NULL)
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
        if (read_pointer == NULL)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Independent observation failed");
            goto finish;
        }
        memcpy(observed, read_pointer, (size_t)(sizeof(observed)));
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
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Buffer contents invalidated during unmap");
            goto finish;
        }
    }
    requirement_results[2] = requirement_results[0] == 1 && requirement_results[1] == 1 &&
                                     usage == GL_STATIC_DRAW && size == sizeof(data) &&
                                     memcmp(data, observed, sizeof(data)) == 0
                                 ? 1
                                 : 0;

    test_result = 1;
    for (unsigned i = 0; i < 3; ++i)
        if (requirement_results[i] != 1)
            test_result = 0;
finish:
    /* Release test-owned GL objects while the harness context is still current. */
    if (!setup_complete)
        for (unsigned i = 0; i < 3; ++i)
            requirement_results[i] = 0;
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
void GS_GLES31_CS_BBB_TP_002_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GLES31_CS_BBB_TP_002_close(void) {}
int GS_GLES31_CS_BBB_TP_002_result(void) { return test_result; }
/* The argument is the HLR suffix, not the TP number or an array index. */
int GS_GLES31_CS_BBB_TP_002_requirement_result(unsigned requirement_number)
{
    for (unsigned i = 0; i < 3; ++i)
        if (requirement_numbers[i] == requirement_number)
            return requirement_results[i];
    return -1;
}
