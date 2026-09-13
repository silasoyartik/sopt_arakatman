#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GL43
#include "../../compute_helpers.h"

/*
GL43 - ComputeShader - glGetBufferSubData

Read valid byte intervals and independently check the whole store after every call.

Covered requirements:
        - GS-GL43-CS-GBSD-002
        - GS-GL43-CS-GBSD-003
        - GS-GL43-CS-GBSD-012
*/

static const char* test_case2 = "GS_GL43_CS_GBSD_TC_002";
static const char* test_case3 = "GS_GL43_CS_GBSD_TC_003";
static const char* test_case12 = "GS_GL43_CS_GBSD_TC_012";
static const char* test_procedure = "GS_GL43_CS_GBSD_TP_002";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLboolean test_success2 = GL_FALSE;
static GLboolean test_executed2 = GL_FALSE;
static GLboolean test_success3 = GL_FALSE;
static GLboolean test_executed3 = GL_FALSE;
static GLboolean test_success12 = GL_FALSE;
static GLboolean test_executed12 = GL_FALSE;
static GLuint buffers[4] = {0};
static GLenum mapped_target = 0;

/* Initialization */
void GS_GL43_CS_GBSD_TP_002_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    const char* test_case = test_case2;
    int setup_complete = 0;
    test_success2 = GL_FALSE;
    test_executed2 = GL_FALSE;
    test_success3 = GL_FALSE;
    test_executed3 = GL_FALSE;
    test_success12 = GL_FALSE;
    test_executed12 = GL_FALSE;

    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
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
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    if (buffers[0] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer setup failed");
        goto finish;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    /* Observe buffer bytes by mapping, independently of the function under test. */
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    {
        void* read_pointer =
            glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(before), GL_MAP_READ_BIT);
        if (read_pointer != NULL)
            mapped_target = GL_COPY_READ_BUFFER;
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            goto finish;
        }
        if (read_pointer == NULL) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Baseline mapping failed");
            goto finish;
        }
        memcpy(before, read_pointer, (size_t)(sizeof(before)));
        GLboolean intact = glUnmapBuffer(GL_COPY_READ_BUFFER);
        mapped_target = 0;
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            goto finish;
        }
        if (intact != GL_TRUE) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Buffer contents invalidated during unmap");
            goto finish;
        }
    }
    if (!(memcmp(before, data, sizeof(data)) == 0)) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Baseline data differs");
        goto finish;
    }
    setup_complete = 1;
    /* All three requirements need the complete loop before PASS is justified. */
    test_executed2 = GL_TRUE;
    test_success2 = GL_FALSE;
    test_executed3 = GL_TRUE;
    test_success3 = GL_FALSE;
    test_executed12 = GL_TRUE;
    test_success12 = GL_FALSE;
    for (unsigned c = 0; c < sizeof(offsets) / sizeof(offsets[0]); ++c) {
        unsigned char out[18];
        int interval_ok;
        /* Restore the same known baseline so a faulty earlier read cannot mask a later one. */
        test_case = test_case2;
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
        glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(data), data);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            goto finish;
        }
        memset(out, 0xa5, sizeof(out));
        glGetBufferSubData(GL_COPY_READ_BUFFER, offsets[c], sizes[c], out + 1);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
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
        test_case = test_case12;
        /* Observe buffer bytes by mapping, independently of the function under test. */
        glBindBuffer(GL_COPY_READ_BUFFER, buffers[0]);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            goto finish;
        }
        {
            void* read_pointer =
                glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(after), GL_MAP_READ_BIT);
            if (read_pointer != NULL)
                mapped_target = GL_COPY_READ_BUFFER;
            err = glGetError();
            if (err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                              (unsigned)err);
                goto finish;
            }
            if (read_pointer == NULL) {
                TEST_LOG_FAIL(test_case, test_procedure, "%s", "Post-call mapping failed");
                goto finish;
            }
            memcpy(after, read_pointer, (size_t)(sizeof(after)));
            GLboolean intact = glUnmapBuffer(GL_COPY_READ_BUFFER);
            mapped_target = 0;
            err = glGetError();
            if (err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                              (unsigned)err);
                goto finish;
            }
            if (intact != GL_TRUE) {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Buffer contents invalidated during unmap");
                goto finish;
            }
        }
        if (memcmp(before, after, sizeof(before)) != 0)
            unchanged = 0;
    }
    test_success2 = copy_ok ? GL_TRUE : GL_FALSE;
    test_success3 = bytes_ok ? GL_TRUE : GL_FALSE;
    test_success12 = unchanged ? GL_TRUE : GL_FALSE;

    test_success = GL_TRUE;
    goto report;
finish:
    test_success = GL_FALSE;
    if (!setup_complete) {
        test_executed2 = GL_TRUE;
        test_success2 = GL_FALSE;
        test_executed3 = GL_TRUE;
        test_success3 = GL_FALSE;
        test_executed12 = GL_TRUE;
        test_success12 = GL_FALSE;
    }
report:
    if (test_executed2) {
        if (test_success2) {
            TEST_LOG_SUCCESS(test_case2, test_procedure);
        } else {
            TEST_LOG_FAIL(test_case2, test_procedure, "Requirement check or shared setup failed");
            test_success = GL_FALSE;
        }
    } else {
        TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_case2, test_procedure);
        test_success = GL_FALSE;
    }
    if (test_executed3) {
        if (test_success3) {
            TEST_LOG_SUCCESS(test_case3, test_procedure);
        } else {
            TEST_LOG_FAIL(test_case3, test_procedure, "Requirement check or shared setup failed");
            test_success = GL_FALSE;
        }
    } else {
        TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_case3, test_procedure);
        test_success = GL_FALSE;
    }
    if (test_executed12) {
        if (test_success12) {
            TEST_LOG_SUCCESS(test_case12, test_procedure);
        } else {
            TEST_LOG_FAIL(test_case12, test_procedure, "Requirement check or shared setup failed");
            test_success = GL_FALSE;
        }
    } else {
        TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_case12, test_procedure);
        test_success = GL_FALSE;
    }
}

/* Draw */
void GS_GL43_CS_GBSD_TP_002_draw(void) {}

/* Cleanup */
void GS_GL43_CS_GBSD_TP_002_close(void) {
    CHECK_GL_ERROR(test_case2, test_procedure, test_success);
    if (mapped_target != 0) {
        if (glUnmapBuffer(mapped_target) != GL_TRUE) {
            TEST_LOG_FAIL(test_case2, test_procedure, "Cleanup unmap failed");
            test_success = GL_FALSE;
        }
        mapped_target = 0;
    }
    glDeleteBuffers(4, buffers);
    memset(buffers, 0, sizeof(buffers));
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case2, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case2, test_procedure, test_success);
}
