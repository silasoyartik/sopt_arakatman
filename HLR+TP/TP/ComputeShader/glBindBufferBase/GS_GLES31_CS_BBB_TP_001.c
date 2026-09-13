#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GLES31
#include "../../compute_helpers.h"

/*
GLES31 - ComputeShader - glBindBufferBase

Compile/link the exact API signature; do not substitute a wrapper.

Covered requirements:
        - GS-GLES31-CS-BBB-001
*/

static const char* test_case = "GS_GLES31_CS_BBB_TC_001";
static const char* test_procedure = "GS_GLES31_CS_BBB_TP_001";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;

/* Initialization */
void GS_GLES31_CS_BBB_TP_001_init(void) {
    test_success = GL_TRUE;
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    if (!test_success) {
        return;
    }

    // Test Case 001: standard API signature and link symbol.
    PFNGLBINDBUFFERBASEPROC volatile entry = glBindBufferBase;
    if (entry == NULL) {
        TEST_LOG_FAIL(test_case, test_procedure, "glBindBufferBase entry point unavailable");
        test_success = GL_FALSE;
    }
    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GLES31_CS_BBB_TP_001_draw(void) {}

/* Cleanup */
void GS_GLES31_CS_BBB_TP_001_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
