#ifndef GS_GLES31_TEST_HELPERS_H
#define GS_GLES31_TEST_HELPERS_H

/* Platform-independent test support. Link helpers_gles31.c and one platform
 * implementation (the reference backend is helpers_gles31_glfw.c).
 * Select at most one GLAD loader; otherwise the target supplies GLES 3.1.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#if defined(GS_GLES31_USE_GLAD1) && defined(GS_GLES31_USE_GLAD2)
#error "Select either GS_GLES31_USE_GLAD1 or GS_GLES31_USE_GLAD2, not both."
#endif

#if defined(GS_GLES31_USE_GLAD2)
#include <glad/gles2.h>
#elif defined(GS_GLES31_USE_GLAD1)
#include <glad/glad.h>
#else
#include <GLES3/gl31.h>
#endif

#include "macros.h"

/* The platform owns this opaque context; TPs must not interpret it. */
typedef struct
{
    void *platform_context;
    int initialized;
} GS_GLES31_TestEnvironment;

#define GS_GLES31_ENV_INITIALIZER { NULL, 0 }

/* A successful prepare establishes a current OpenGL ES >= 3.1 context,
 * loads the required entry points, and sets initialized. Cleanup must be
 * safe after partial setup. These functions belong to the platform backend.
 */
int GS_GLES31_prepare_environment(GS_GLES31_TestEnvironment *environment);
int GS_GLES31_environment_is_current(
    const GS_GLES31_TestEnvironment *environment);
void GS_GLES31_cleanup_environment(GS_GLES31_TestEnvironment *environment);

/* Existing low-level helpers, implemented in helpers_gles31.c. Explicit
 * error clearing is for setup/before a tested call, never after that call.
 */
void GS_GLES31_clear_errors(void);
const char *GS_GLES31_error_name(GLenum error);
GLuint GS_GLES31_compile_shader(GLenum shader_type, const char *source);
GLuint GS_GLES31_create_compute_program(const char *compute_shader_source);
GLuint GS_GLES31_create_graphics_program(
    const char *vertex_shader_source, const char *fragment_shader_source);
GLuint GS_GLES31_create_ssbo(
    GLuint binding_point, GLsizeiptr size, const void *initial_data, GLenum usage);
void GS_GLES31_bind_ssbo(GLuint buffer, GLuint binding_point);
void GS_GLES31_delete_program(GLuint *program);
void GS_GLES31_delete_buffer(GLuint *buffer);

typedef enum
{
    GS_GLES31_NOT_RUN,
    GS_GLES31_PASS,
    GS_GLES31_FAIL
} GS_GLES31_TestStatus;

typedef struct
{
    const char *test_case;
    const char *test_procedure;
    GS_GLES31_TestStatus status;
} GS_GLES31_TestResult;

#define GS_GLES31_RESULT(tc, tp) { tc, tp, GS_GLES31_NOT_RUN }

/* Initialize statically or with GS_GLES31_FIXTURE_INITIALIZER, and call end
 * after every attempted begin. A fixture owns one program and up to four
 * SSBOs; no resource ownership is transferred to the TP.
 */
typedef struct
{
    GS_GLES31_TestEnvironment environment;
    GLuint program;
    GLuint buffers[4];
    GLuint buffer_bindings[4];
    size_t buffer_count;
} GS_GLES31_TestFixture;

#define GS_GLES31_FIXTURE_INITIALIZER \
    { GS_GLES31_ENV_INITIALIZER, 0, {0}, {0}, 0 }

void GS_GLES31_reset_results(GS_GLES31_TestResult *results, size_t count);
int GS_GLES31_check_at(GS_GLES31_TestResult *result, int condition,
    const char *file, int line, const char *format, ...);
void GS_GLES31_fail_results_at(GS_GLES31_TestResult *results, size_t count,
    const char *file, int line, const char *format, ...);
void GS_GLES31_report_results(GS_GLES31_TestResult *results, size_t count);

#define GS_GLES31_CHECK(result, condition, ...) \
    GS_GLES31_check_at(result, condition, __FILE__, __LINE__, __VA_ARGS__)
#define GS_GLES31_FAIL_RESULTS(results, count, ...) \
    GS_GLES31_fail_results_at(results, count, __FILE__, __LINE__, __VA_ARGS__)

/* Infrastructure failures return zero and print a diagnostic. The TP marks
 * its affected results failed explicitly. No helper dispatches compute work.
 */
int GS_GLES31_begin_fixture(GS_GLES31_TestFixture *fixture);
int GS_GLES31_fixture_compute_program(
    GS_GLES31_TestFixture *fixture, const char *source);
int GS_GLES31_fixture_graphics_program(
    GS_GLES31_TestFixture *fixture, const char *vs, const char *fs);
int GS_GLES31_use_program(GLuint program);
GLuint GS_GLES31_fixture_ssbo(GS_GLES31_TestFixture *fixture,
    GLuint binding, GLsizeiptr size, const void *initial, GLenum usage);
int GS_GLES31_reset_ssbo(GLuint buffer, GLuint binding,
    GLsizeiptr size, const void *data);

/* CPU readback only: BUFFER_UPDATE barrier -> map -> copy -> unmap.
 * The caller must consume/check the dispatch error before calling this.
 */
int GS_GLES31_read_ssbo(GLuint buffer, GLsizeiptr size, void *destination);
int GS_GLES31_work_group_limit(GLuint axis, GLuint *limit);
void GS_GLES31_fill_uint(GLuint *data, size_t count, GLuint value);
int GS_GLES31_expect_error_at(GS_GLES31_TestResult *result, GLenum expected,
    const char *file, int line, const char *operation);
#define GS_GLES31_EXPECT_ERROR(result, expected, operation) \
    GS_GLES31_expect_error_at(result, expected, __FILE__, __LINE__, operation)
void GS_GLES31_end_fixture(GS_GLES31_TestFixture *fixture);
const char *GS_GLES31_noop_compute_source(void);

#endif
