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
 * GS-GL43-CS-GBSD-002
 * GS-GL43-CS-GBSD-003
 * GS-GL43-CS-GBSD-012
 * Read valid byte intervals and independently check the whole store after every call.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_procedure = "GS_GL43_CS_GBSD_TP_002";
static const char *test_cases[] = {"GS_GL43_CS_GBSD_TC_002", "GS_GL43_CS_GBSD_TC_003",
                                   "GS_GL43_CS_GBSD_TC_012"};
static const unsigned requirement_numbers[] = {2, 3, 12};
static int requirement_results[3] = {-1, -1, -1};

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GL43_CS_GBSD_TP_002_init(void)
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

    const unsigned char data[16] = {3,  17,  29,  41,  53,  67,  79,  83,
                                    97, 109, 127, 139, 151, 163, 179, 193};
    /* Union of the previous full/partial/boundary/unaligned/zero-size cases. */
    const GLintptr offsets[] = {0, 3, 11, 16, 0, 1, 5};
    const GLsizeiptr sizes[] = {16, 7, 5, 0, 1, 3, 7};
    unsigned char before[16] = {0}, after[16] = {0};
    int copy_ok = 1, bytes_ok = 1, unchanged = 1;
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
    /* Observe buffer bytes by mapping, independently of the function under test. */
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
    {
        void *read_pointer =
            glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(before), GL_MAP_READ_BIT);
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
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Baseline mapping failed");
            goto finish;
        }
        memcpy(before, read_pointer, (size_t)(sizeof(before)));
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
    if (!(memcmp(before, data, sizeof(data)) == 0))
    {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Baseline data differs");
        goto finish;
    }
    setup_complete = 1;
    /* All three requirements need the complete loop before PASS is justified. */
    for (unsigned i = 0; i < 3; ++i)
        requirement_results[i] = 0;
    for (unsigned c = 0; c < sizeof(offsets) / sizeof(offsets[0]); ++c)
    {
        unsigned char out[18];
        int interval_ok;
        /* Restore the same known baseline so a faulty earlier read cannot mask a later one. */
        test_case = test_cases[0];
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
        glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(data), data);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        memset(out, 0xa5, sizeof(out));
        glGetBufferSubData(GL_COPY_READ_BUFFER, offsets[c], sizes[c], out + 1);
        actual_error = glGetError();
        extra_error = glGetError();
        if (actual_error != GL_NO_ERROR || extra_error != GL_NO_ERROR)
        {
            TEST_LOG_FAIL(test_case, test_procedure, "%s (expected 0x%x, got 0x%x, extra 0x%x)",
                          "Unexpected GL error", (unsigned)GL_NO_ERROR, (unsigned)actual_error,
                          (unsigned)extra_error);
            goto finish;
        }
        interval_ok = memcmp(out + 1, before + offsets[c], (size_t)sizes[c]) == 0 && out[0] == 0xa5;
        for (size_t i = 1 + (size_t)sizes[c]; i < sizeof(out); ++i)
            if (out[i] != 0xa5)
                interval_ok = 0;
        if (!interval_ok)
            copy_ok = 0;
        if (c >= 4 && !interval_ok)
            bytes_ok = 0;
        test_case = test_cases[2];
        /* Observe buffer bytes by mapping, independently of the function under test. */
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
        {
            void *read_pointer =
                glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(after), GL_MAP_READ_BIT);
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
                TEST_LOG_FAIL(test_case, test_procedure, "%s", "Post-call mapping failed");
                goto finish;
            }
            memcpy(after, read_pointer, (size_t)(sizeof(after)));
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
        if (memcmp(before, after, sizeof(before)) != 0)
            unchanged = 0;
    }
    requirement_results[0] = copy_ok ? 1 : 0;
    requirement_results[1] = bytes_ok ? 1 : 0;
    requirement_results[2] = unchanged ? 1 : 0;

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
void GS_GL43_CS_GBSD_TP_002_draw(void) {}
/* init releases resources on both success and failure; repeated close is harmless. */
void GS_GL43_CS_GBSD_TP_002_close(void) {}
int GS_GL43_CS_GBSD_TP_002_result(void) { return test_result; }
/* The argument is the HLR suffix, not the TP number or an array index. */
int GS_GL43_CS_GBSD_TP_002_requirement_result(unsigned requirement_number)
{
    for (unsigned i = 0; i < 3; ++i)
        if (requirement_numbers[i] == requirement_number)
            return requirement_results[i];
    return -1;
}
