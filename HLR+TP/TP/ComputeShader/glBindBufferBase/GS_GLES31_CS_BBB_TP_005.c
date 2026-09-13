#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Observe zero start, whole-store shader access, and resize without indexed rebinding.

Covered requirements:
        - GS-GLES31-CS-BBB-007
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_007";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_005";

/* ---- Shader sources ---- */
static const char* source =
    "#version 310 es\nlayout(local_size_x=1) in;\n"
    "layout(std430,binding=0) buffer Payload { uint words[]; };\n"
    "void main(){ uint n=uint(words.length()); words[0]=n; words[n-1u]=79u; }\n";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint buffers[4] = {0};
static GLuint shader = 0, program = 0;
static GLenum mapped_target = 0;

/* Initialization */
void GS_GLES31_CS_BBB_TP_005_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    GLuint initial[8] = {0}, observed[8] = {0};
    GLint64 start = -1, binding_size = -1;
    GLint alignment = 0;
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (!(alignment > 0 && (size_t)alignment <= (size_t)PTRDIFF_MAX - sizeof(initial))) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Invalid/overflowing SSBO alignment");
        test_success = GL_FALSE;
        return;
    }
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
    glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)alignment + sizeof(initial), NULL,
                 GL_STATIC_DRAW);
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
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Compute program setup failed");
        test_success = GL_FALSE;
        return;
    }
    /* A restricted baseline detects implementations that retain a prior range. */
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffers[0], alignment, 2 * sizeof(GLuint));
    glUseProgram(program);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_START, 0, &start);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    if (start != alignment) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s", "Nonzero start baseline missing");
        test_success = GL_FALSE;
        return;
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[0]);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_START, 0, &start);
    glGetInteger64i_v(GL_SHADER_STORAGE_BUFFER_SIZE, 0, &binding_size);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }
    /* ES 3.1 section 6.6.1: queried size 0 means whole current store. */
    if (!(start == 0 && binding_size == 0)) {
        TEST_LOG_FAIL(test_case, test_procedure, "%s",
                      "Whole-buffer binding sentinel/start incorrect");
        test_success = GL_FALSE;
        return;
    }
    for (unsigned count = 4; count <= 8; count += 4) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, buffers[0]);
        glBufferData(GL_COPY_WRITE_BUFFER, count * sizeof(GLuint), initial, GL_STATIC_DRAW);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        } /* No glBindBufferBase after resizing. */
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
        /* Observe buffer bytes by mapping, independently of the function under test. */
        glBindBuffer(GL_COPY_READ_BUFFER, buffers[0]);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        {
            void* read_pointer =
                glMapBufferRange(GL_COPY_READ_BUFFER, 0, count * sizeof(GLuint), GL_MAP_READ_BIT);
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
            memcpy(observed, read_pointer, (size_t)(count * sizeof(GLuint)));
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
        if (!(observed[0] == count && observed[count - 1] == 79u)) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Whole store/updated size not accessible");
            test_success = GL_FALSE;
            return;
        }
        for (unsigned i = 1; i + 1 < count; ++i)
            if (!(observed[i] == 0)) {
                TEST_LOG_FAIL(test_case, test_procedure, "%s",
                              "Unexpected write outside selected elements");
                test_success = GL_FALSE;
                return;
            }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_005_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_005_close(void) {
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
