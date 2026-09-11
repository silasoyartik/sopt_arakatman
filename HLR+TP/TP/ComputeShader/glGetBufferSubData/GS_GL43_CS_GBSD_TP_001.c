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

/* Covered requirement: GS-GL43-CS-GBSD-001
 * Compile/link the desktop OpenGL API signature; never emulate readback with a wrapper.
 * Precondition: the harness supplies a fresh, isolated current context and
 * loaded entry points for this API. Context lifetime belongs to the harness.
 * This procedure creates and deletes only its own GL objects.
 */
static const char *test_case = "GS_GL43_CS_GBSD_TC_001";
static const char *test_procedure = "GS_GL43_CS_GBSD_TP_001";

/* Result values: -1 = not run, 0 = fail, 1 = pass. */
static int test_result = -1;

void GS_GL43_CS_GBSD_TP_001_init(void)
{
    /* Compile/link check: exact standard API signature, no GL objects needed.
     * A dynamic loader, if used, must already have resolved the entry point.
     */
    PFNGLGETBUFFERSUBDATAPROC volatile entry = glGetBufferSubData;
    test_result = 0;
    if (entry == NULL)
    {
        TEST_LOG_FAIL(test_case, test_procedure, "glGetBufferSubData entry point unavailable");
        return;
    }
    test_result = 1;
    TEST_LOG_SUCCESS(test_case, test_procedure);
}
void GS_GL43_CS_GBSD_TP_001_draw(void) {}
void GS_GL43_CS_GBSD_TP_001_close(void) {}
int GS_GL43_CS_GBSD_TP_001_result(void) { return test_result; }
