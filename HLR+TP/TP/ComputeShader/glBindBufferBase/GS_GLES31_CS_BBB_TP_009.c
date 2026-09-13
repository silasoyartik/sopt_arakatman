#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Query explicit and default block bindings immediately after link; no override is used.

Covered requirements:
        - GS-GLES31-CS-BBB-011
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_011";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_009";

/* ---- Shader sources ---- */
static const char* sources[] = {"#version 310 es\nlayout(local_size_x=1) in;\n"
                                "layout(std430,binding=1) buffer Payload { uint value; };\n"
                                "void main(){ value=7u; }\n",
                                "#version 310 es\nlayout(local_size_x=1) in;\n"
                                "layout(std430) buffer Payload { uint value; };\n"
                                "void main(){ value=7u; }\n"};

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;
static GLuint shader = 0, program = 0;

/* Initialization */
void GS_GLES31_CS_BBB_TP_009_init(void) {
    test_success = GL_TRUE;

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
        return;
    }

    for (unsigned i = 0; i < 2; ++i) {
        GLint binding = -1;
        GLenum property = GL_BUFFER_BINDING;
        /* Compile and link the compute program used by this requirement. */
        {
            const GLchar* shader_source = sources[i];
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
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Program link failed");
            test_success = GL_FALSE;
            return;
        }
        GLuint block = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "Payload");
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (block == GL_INVALID_INDEX) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s", "Active storage block missing");
            test_success = GL_FALSE;
            return;
        }
        glGetProgramResourceiv(program, GL_SHADER_STORAGE_BLOCK, block, 1, &property, 1, NULL,
                               &binding);
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
        if (!(binding == (i == 0 ? 1 : 0))) {
            TEST_LOG_FAIL(test_case, test_procedure, "%s",
                          "Linked block binding differs from explicit/default binding");
            test_success = GL_FALSE;
            return;
        }
        glDeleteProgram(program);
        program = 0;
        err = glGetError();
        if (err != GL_NO_ERROR) {
            TEST_LOG_FAIL(test_case, test_procedure, "Expected GL_NO_ERROR, got: 0x%x",
                          (unsigned)err);
            test_success = GL_FALSE;
            return;
        }
    }

    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_009_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_009_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    glUseProgram(0);
    if (shader != 0)
        glDeleteShader(shader);
    if (program != 0)
        glDeleteProgram(program);
    shader = 0;
    program = 0;
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "Cleanup error: 0x%x", (unsigned)err);
        test_success = GL_FALSE;
    }
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
