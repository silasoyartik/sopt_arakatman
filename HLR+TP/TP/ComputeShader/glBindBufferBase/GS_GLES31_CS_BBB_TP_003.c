#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Test a generated but never bound name and an unused nonzero name in an isolated context.

Covered requirements:
        - GS-GLES31-CS-BBB-005
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_005";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_003";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};

/* Initialization */
void GS_GLES31_CS_BBB_TP_003_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    for (int subcase = 0; subcase < 2; ++subcase) {
        GLuint* name = &buffers[subcase];
        GLint value = -1;
        GLint64 wide = -1;
        void* pointer = (void*)&value;
        if (subcase == 0) {
            glGenBuffers(1, name); /* Deliberately no glBindBuffer/glBufferData. */
        } else {
            *name = buffers[0] == 12345u ? 12346u : 12345u;
        }
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (!(*name != 0 && glIsBuffer(*name) == GL_FALSE)) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Name already has object state");
            test_success = GL_FALSE;
            return;
        }
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *name);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (!(glIsBuffer(*name) == GL_TRUE)) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "First binding did not create object state");
            test_success = GL_FALSE;
            return;
        }
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        const GLenum zero_wide[] = {GL_BUFFER_SIZE, GL_BUFFER_MAP_OFFSET, GL_BUFFER_MAP_LENGTH};
        for (unsigned i = 0; i < sizeof(zero_wide) / sizeof(zero_wide[0]); ++i) {
            glGetBufferParameteri64v(GL_SHADER_STORAGE_BUFFER, zero_wide[i], &wide);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                              (unsigned)err);
                test_success = GL_FALSE;
                return;
            }
            if (wide != 0) {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Initial size/map range is not zero");
                test_success = GL_FALSE;
                return;
            }
        }
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_USAGE, &value);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (value != GL_STATIC_DRAW) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial usage is not STATIC_DRAW");
            test_success = GL_FALSE;
            return;
        }
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_ACCESS_FLAGS, &value);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (value != 0) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial access flags are not zero");
            test_success = GL_FALSE;
            return;
        }
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_MAPPED, &value);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (value != GL_FALSE) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "New object is mapped");
            test_success = GL_FALSE;
            return;
        }
        glGetBufferPointerv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_MAP_POINTER, &pointer);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (pointer != NULL) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Initial map pointer is not NULL");
            test_success = GL_FALSE;
            return;
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_003_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_003_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    glDeleteBuffers(4, buffers);
    memset(buffers, 0, sizeof(buffers));
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
