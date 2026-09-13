#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Check indexed/general replacement and preservation of the newly bound buffer in one fixture.

Covered requirements:
        - GS-GLES31-CS-BBB-002
        - GS-GLES31-CS-BBB-003
        - GS-GLES31-CS-BBB-004
*/

static const char* test_case2 = "GS_GLES31_CS_BBB_TC_002";
static const char* test_case3 = "GS_GLES31_CS_BBB_TC_003";
static const char* test_case4 = "GS_GLES31_CS_BBB_TC_004";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_002";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLboolean test_success2 = GL_FALSE;
static GLboolean test_executed2 = GL_FALSE;
static GLboolean test_success3 = GL_FALSE;
static GLboolean test_executed3 = GL_FALSE;
static GLboolean test_success4 = GL_FALSE;
static GLboolean test_executed4 = GL_FALSE;
static GLuint buffers[4] = {0};
static GLenum mapped_target = 0;

/* Initialization */
void GS_GLES31_CS_BBB_TP_002_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    const char* test_case = test_case2;
    int setup_complete = 0;
    test_success2 = GL_FALSE;
    test_executed2 = GL_FALSE;
    test_success3 = GL_FALSE;
    test_executed3 = GL_FALSE;
    test_success4 = GL_FALSE;
    test_executed4 = GL_FALSE;

    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }

    const GLuint data[4] = {17, 29, 41, 53};
    GLuint observed[4] = {0};
    GLint indexed = -1, general = -1, usage = 0;
    GLint64 size = -1;
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    if (buffers[0] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Old buffer setup failed");
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
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[1]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    if (buffers[1] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "New buffer setup failed");
        goto finish;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[1]);
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
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffers[0], 0, sizeof(data));
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    {
        GLint actual_general = -1, actual_indexed = -1;
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &actual_general);
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &actual_indexed);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            goto finish;
        }
        if (!((GLuint)actual_general == buffers[0] && (GLuint)actual_indexed == buffers[0])) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Previous bindings missing");
            goto finish;
        }
    }
    setup_complete = 1;
    test_executed2 = GL_TRUE;
    test_success2 = GL_FALSE;
    test_executed3 = GL_TRUE;
    test_success3 = GL_FALSE;
    test_executed4 = GL_TRUE;
    test_success4 = GL_FALSE;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[1]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    /* No intervening glBindBuffer: query both effects of this exact call. */
    glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &indexed);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    test_success2 = (GLuint)indexed == buffers[1] ? GL_TRUE : GL_FALSE;
    test_case = test_case3;
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &general);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    test_success3 = (GLuint)general == buffers[1] ? GL_TRUE : GL_FALSE;

    test_case = test_case4;
    /* Query the new object through the copy target, even if general binding is wrong. */
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[1]);
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_USAGE, &usage);
    glGetBufferParameteri64v(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    /* Observe buffer bytes by mapping, independently of the function under test. */
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[1]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        goto finish;
    }
    {
        void* read_pointer =
            glMapBufferRange(GL_COPY_READ_BUFFER, 0, sizeof(observed), GL_MAP_READ_BIT);
        if (read_pointer != NULL)
            mapped_target = GL_COPY_READ_BUFFER;
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            goto finish;
        }
        if (read_pointer == NULL) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Independent observation failed");
            goto finish;
        }
        memcpy(observed, read_pointer, (size_t)(sizeof(observed)));
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
    test_success4 = test_success2 == GL_TRUE && test_success3 == GL_TRUE &&
                            usage == GL_STATIC_DRAW && size == sizeof(data) &&
                            memcmp(data, observed, sizeof(data)) == 0
                        ? GL_TRUE
                        : GL_FALSE;

    test_success = GL_TRUE;
    goto report;
finish:
    test_success = GL_FALSE;
    if (!setup_complete) {
        test_executed2 = GL_TRUE;
        test_success2 = GL_FALSE;
        test_executed3 = GL_TRUE;
        test_success3 = GL_FALSE;
        test_executed4 = GL_TRUE;
        test_success4 = GL_FALSE;
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
    if (test_executed4) {
        if (test_success4) {
            TEST_LOG_SUCCESS(test_case4, test_procedure);
        } else {
            TEST_LOG_FAIL(test_case4, test_procedure, "Requirement check or shared setup failed");
            test_success = GL_FALSE;
        }
    } else {
        TEST_LOG_INFO("[ %s ][ %s ] Not run: earlier step failed", test_case4, test_procedure);
        test_success = GL_FALSE;
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_002_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_002_close(void) {
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
