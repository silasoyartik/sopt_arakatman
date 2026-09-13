#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Route shader reads/writes to indexed buffer, with a distinct general binding as a decoy.

Covered requirements:
        - GS-GLES31-CS-BBB-012
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_012";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_010";

/* ---- Shader sources ---- */
static const char* source = "#version 310 es\nlayout(local_size_x=1) in;\n"
                            "layout(std430,binding=1) buffer Payload { uint words[4]; };\n"
                            "void main(){ words[1]=words[0]+37u; }\n";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};
static GLuint shader = 0, program = 0;
static GLenum mapped_target = 0;

/* Initialization */
void GS_GLES31_CS_BBB_TP_010_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    const GLuint first[4] = {101, 0, 301, 401}, second[4] = {202, 0, 302, 402};
    GLuint observed[4] = {0};
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (buffers[0] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "First buffer setup failed");
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
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(first), first, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    /* Prepare storage through a non-indexed target. */
    glGenBuffers(1, &buffers[1]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (buffers[1] == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Second buffer setup failed");
        test_success = GL_FALSE;
        return;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[1]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(second), second, GL_STATIC_DRAW);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    /* Compile and link the compute program used by this requirement. */
    {
        const GLchar* shader_source = source;
        GLint compiled = GL_FALSE, linked = GL_FALSE;
        GLchar info_log[1024] = {0};
        shader = glCreateShader(GL_COMPUTE_SHADER);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (shader == 0) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Could not create compute shader");
            test_success = GL_FALSE;
            return;
        }
        glShaderSource(shader, 1, &shader_source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (compiled != GL_TRUE) {
            glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
            TEST_LOG_FAIL(test_case, test_procedure, "Shader compilation failed: %s", info_log);
            test_success = GL_FALSE;
            return;
        }
        program = glCreateProgram();
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (program == 0) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Could not create program");
            test_success = GL_FALSE;
            return;
        }
        glAttachShader(program, shader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (linked != GL_TRUE) {
            glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
            TEST_LOG_FAIL(test_case, test_procedure, "Program link failed: %s", info_log);
            test_success = GL_FALSE;
            return;
        }
        glDetachShader(program, shader);
        glDeleteShader(shader);
        shader = 0;
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
    }
    if (program == 0) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Program setup failed");
        test_success = GL_FALSE;
        return;
    }
    GLuint block = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "Payload");
    GLenum property = GL_BUFFER_DATA_SIZE;
    GLint required_size = 0;
    if (block == GL_INVALID_INDEX) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Active block missing");
        test_success = GL_FALSE;
        return;
    }
    glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, block, 1, &property, 1, NULL,
                           &required_size);
    glUseProgram(program);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (!(required_size > 0 && required_size <= (GLint)sizeof(first))) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s",
                      "Fixture smaller than shader block requirement");
        test_success = GL_FALSE;
        return;
    }
    for (unsigned selected = 0; selected < 2; ++selected) {
        /* Restore known content without modifying indexed bindings. */
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
        glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(first), first);
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[1]);
        glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(second), second);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[selected]);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1 - selected]);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        glDispatchCompute(1, 1, 1);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        for (unsigned i = 0; i < 2; ++i) {
            GLuint expected[4];
            memcpy(expected, i == 0 ? first : second, sizeof(expected));
            if (i == selected)
                expected[1] = expected[0] + 37u;
            /* Observe buffer bytes by mapping, independently of the function under test. */
            glBindBuffer(GL_COPY_READ_BUFFER, buffers[i]);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                              (unsigned)err);
                test_success = GL_FALSE;
                return;
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
                    test_success = GL_FALSE;
                    return;
                }
                if (read_pointer == NULL) {
                    TEST_LOG_FAIL(test_case, test_procedure, "%s", "Readback failed");
                    test_success = GL_FALSE;
                    return;
                }
                memcpy(observed, read_pointer, (size_t)(sizeof(observed)));
                GLboolean intact = glUnmapBuffer(GL_COPY_READ_BUFFER);
                mapped_target = 0;
                err = glGetError();
                if (err != GL_NO_ERROR) {
                    TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                                  (unsigned)err);
                    test_success = GL_FALSE;
                    return;
                }
                if (intact != GL_TRUE) {
                    TEST_LOG_FAIL(test_case, test_procedure, "%s",
                                  "Buffer contents invalidated during unmap");
                    test_success = GL_FALSE;
                    return;
                }
            }
            if (!(memcmp(expected, observed, sizeof(expected)) == 0)) {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Shader used wrong binding/buffer or changed unrelated data");
                test_success = GL_FALSE;
                return;
            }
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_010_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_010_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    if (mapped_target != 0) {
        if (glUnmapBuffer(mapped_target) != GL_TRUE) {
            TEST_LOG_FAIL(test_case, test_procedure, "Cleanup unmap failed");
            test_success = GL_FALSE;
        }
        mapped_target = 0;
    }
    glUseProgram(0);
    if (shader != 0)
        glDeleteShader(shader);
    if (program != 0)
        glDeleteProgram(program);
    shader = 0;
    program = 0;
    glDeleteBuffers(4, buffers);
    memset(buffers, 0, sizeof(buffers));
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
