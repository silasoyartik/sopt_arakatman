#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define GS_BUFFER_TEST_GL43
#include "../../compute_helpers.h"

/*
GL43 - ComputeShader - glGetBufferSubData

Compile/link the desktop OpenGL API signature; never emulate readback with a wrapper.

Covered requirements:
        - GS-GL43-CS-GBSD-001
*/

static const char* test_case = "GS_GL43_CS_GBSD_TC_001";
static const char* test_procedure = "GS_GL43_CS_GBSD_TP_001";

/* ---- Static state ---- */
static GLboolean test_success = GL_TRUE;

/* Initialization */
void GS_GL43_CS_GBSD_TP_001_init(void) {
    test_success = GL_TRUE;
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
    if (!test_success) {
        return;
    }

    // Test Case 001: standard API signature and link symbol.
    PFNGLGETBUFFERSUBDATAPROC volatile entry = glGetBufferSubData;
    if (entry == NULL) {
        TEST_LOG_FAIL(test_case, test_procedure, "glGetBufferSubData entry point unavailable");
        test_success = GL_FALSE;
    }
    if (test_success) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }
}

/* Draw */
void GS_GL43_CS_GBSD_TP_001_draw(void) {}

/* Cleanup */
void GS_GL43_CS_GBSD_TP_001_close(void) {
    CHECK_GL_ERROR(test_case, test_procedure, test_success);
}
