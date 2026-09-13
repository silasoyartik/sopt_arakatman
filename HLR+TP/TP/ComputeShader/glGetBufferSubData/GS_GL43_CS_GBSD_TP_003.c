#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GL43
#include "../../compute_helpers.h"

/*
GL43 - ComputeShader - glGetBufferSubData

Exercise all thirteen GL 4.3 buffer targets; use a real VAO for element-array binding.

Covered requirements:
        - GS-GL43-CS-GBSD-004
*/

static const char* test_case = "GS_GL43_CS_GBSD_TC_004";
static const char* test_procedure = "GS_GL43_CS_GBSD_TP_003";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};
static GLuint vao = 0;

/* Initialization */
void GS_GL43_CS_GBSD_TP_003_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    const unsigned char data[16] = {3,  17,  29,  41,  53,  67,  79,  83,
                                    97, 109, 127, 139, 151, 163, 179, 193};
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (buffers[0] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Buffer setup failed");
        test_success = GL_FALSE;
        return;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    glBindBuffer(GL_COPY_READ_BUFFER, buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    const GLenum targets[] = {
        GL_ARRAY_BUFFER,          GL_ATOMIC_COUNTER_BUFFER,    GL_COPY_READ_BUFFER,
        GL_COPY_WRITE_BUFFER,     GL_DISPATCH_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,  GL_PIXEL_PACK_BUFFER,        GL_PIXEL_UNPACK_BUFFER,
        GL_SHADER_STORAGE_BUFFER, GL_TEXTURE_BUFFER,           GL_TRANSFORM_FEEDBACK_BUFFER,
        GL_UNIFORM_BUFFER};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (vao == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Core-profile VAO setup failed");
        test_success = GL_FALSE;
        return;
    }
    for (unsigned i = 0; i < sizeof(targets) / sizeof(targets[0]); ++i) {
        unsigned char out[16] = {0};
        glBindBuffer(targets[i], buffers[0]);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        glGetBufferSubData(targets[i], 0, sizeof(out), out);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (!(memcmp(out, data, sizeof(out)) == 0)) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Valid target did not return buffer data");
            test_success = GL_FALSE;
            return;
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GL43_CS_GBSD_TP_003_draw(void) {}

/* Cleanup */
void GS_GL43_CS_GBSD_TP_003_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    glDeleteBuffers(4, buffers);
    memset(buffers, 0, sizeof(buffers));
    if (vao != 0) {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
