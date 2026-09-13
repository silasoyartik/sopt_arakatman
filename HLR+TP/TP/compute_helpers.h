#ifndef GS_COMPUTE_HELPERS_H
#define GS_COMPUTE_HELPERS_H

/* Shared logging only; no dependency on the EGL helpers.h. */
#include "macros.h"

/* Each TP selects its API before including this header. */
#if defined(GS_BUFFER_TEST_GLES31) == defined(GS_BUFFER_TEST_GL43)
#error "Select exactly one buffer test API: GLES31 or GL43"
#endif

/* A target SDK or GL loader header may replace the standard API header.
 * It must provide the selected API's types, entry points and PFNGL* types.
 * Context creation and entry-point loading belong to the external runner.
 */
#ifdef GS_BUFFER_TEST_API_HEADER
#include GS_BUFFER_TEST_API_HEADER
#elif defined(GS_BUFFER_TEST_GLES31)
#ifndef GL_GLES_PROTOTYPES
#define GL_GLES_PROTOTYPES 1
#endif
#include <GLES3/gl31.h>
#else
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/glcorearb.h>
#endif

/* For unexpected errors only. Expected negative-test errors are checked
 * directly in the TP, immediately after the function under test.
 * Does not reset a previous failure or return from the caller (close must
 * still release resources). Kept distinct from the EGL CHECK_ERROR macro.
 */
#define CHECK_GL_ERROR(test_case, test_procedure, success)                          \
    do {                                                                         \
        GLenum gs_buffer_error = glGetError();                                    \
        if (gs_buffer_error != GL_NO_ERROR) {                                     \
            TEST_LOG_FAIL(test_case, test_procedure,                              \
                          "Unexpected GL error: 0x%x", (unsigned)gs_buffer_error); \
            (success) = GL_FALSE;                                                \
        }                                                                        \
    } while (0)

#endif
