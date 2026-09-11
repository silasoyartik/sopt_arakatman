/* Infrastructure regression tests using GLAD function-pointer stubs.
 * No window, GPU context or simulated shader execution is involved.
 * Expected failures below exercise error handling and print FAIL diagnostics.
 */
#include "../HLR+TP/TP/helpers_gles31.h"

#include <assert.h>
#include <string.h>

static GLenum errors[4];
static size_t error_index;
static GLuint storage[4] = { 3u, 5u, 7u, 11u };
static int map_failure, unmap_failure, barrier_failure;
static int barrier_seen, unmap_calls, cleanup_calls, deleted_buffers, deleted_programs;
static GLuint active_program, active_pipeline;
static int wrong_program;

static GLenum APIENTRY fake_get_error(void)
{
    GLenum error = errors[error_index];
    if (error != GL_NO_ERROR) ++error_index;
    return error;
}

static void set_errors(GLenum first, GLenum second)
{
    memset(errors, 0, sizeof(errors));
    errors[0] = first;
    errors[1] = second;
    error_index = 0;
}

static void APIENTRY fake_barrier(GLbitfield bits)
{
    assert(bits == GL_BUFFER_UPDATE_BARRIER_BIT);
    barrier_seen = 1;
    if (barrier_failure) set_errors(GL_INVALID_OPERATION, GL_NO_ERROR);
}

static void APIENTRY fake_bind_buffer(GLenum target, GLuint buffer)
{
    assert(target == GL_SHADER_STORAGE_BUFFER && buffer == 17);
}

static void APIENTRY fake_bind_buffer_base(GLenum target, GLuint binding, GLuint buffer)
{
    assert(target == GL_SHADER_STORAGE_BUFFER && binding == 0);
    assert(buffer == 0 || buffer == 17);
}

static void *APIENTRY fake_map(GLenum target, GLintptr offset,
    GLsizeiptr length, GLbitfield access)
{
    assert(barrier_seen);
    assert(target == GL_SHADER_STORAGE_BUFFER && offset == 0);
    assert(length == sizeof(storage) && access == GL_MAP_READ_BIT);
    return map_failure ? NULL : storage;
}

static GLboolean APIENTRY fake_unmap(GLenum target)
{
    assert(target == GL_SHADER_STORAGE_BUFFER);
    ++unmap_calls;
    return unmap_failure ? GL_FALSE : GL_TRUE;
}

static void APIENTRY fake_use_program(GLuint program) { active_program = program; }
static void APIENTRY fake_pipeline(GLuint pipeline) { active_pipeline = pipeline; }

static void APIENTRY fake_get_integer(GLenum name, GLint *value)
{
    if (name == GL_CURRENT_PROGRAM)
        *value = wrong_program ? 99 : (GLint)active_program;
    else
    {
        assert(name == GL_PROGRAM_PIPELINE_BINDING);
        *value = (GLint)active_pipeline;
    }
}

static void APIENTRY fake_delete_buffers(GLsizei count, const GLuint *buffers)
{
    assert(count == 1 && *buffers == 17);
    ++deleted_buffers;
}

static void APIENTRY fake_delete_program(GLuint program)
{
    assert(program == 42 && active_program == 0);
    ++deleted_programs;
}

int GS_GLES31_prepare_environment(GS_GLES31_TestEnvironment *environment)
{
    environment->platform_context = storage;
    environment->initialized = 1;
    return 1;
}

int GS_GLES31_environment_is_current(const GS_GLES31_TestEnvironment *environment)
{
    return environment->initialized && environment->platform_context == storage;
}

void GS_GLES31_cleanup_environment(GS_GLES31_TestEnvironment *environment)
{
    if (environment->initialized) ++cleanup_calls;
    environment->initialized = 0;
    environment->platform_context = NULL;
}

int main(void)
{
    GS_GLES31_TestResult results[] = {
        GS_GLES31_RESULT("TC_A", "helper_regression"),
        GS_GLES31_RESULT("TC_B", "helper_regression")
    };
    GS_GLES31_TestFixture fixture = GS_GLES31_FIXTURE_INITIALIZER;
    GLuint copy[4] = {0};

    glGetError = fake_get_error;
    glMemoryBarrier = fake_barrier;
    glBindBuffer = fake_bind_buffer;
    glBindBufferBase = fake_bind_buffer_base;
    glMapBufferRange = fake_map;
    glUnmapBuffer = fake_unmap;
    glUseProgram = fake_use_program;
    glBindProgramPipeline = fake_pipeline;
    glGetIntegerv = fake_get_integer;
    glDeleteBuffers = fake_delete_buffers;
    glDeleteProgram = fake_delete_program;

    /* Failures are sticky within a run but cannot contaminate the next run. */
    GS_GLES31_reset_results(results, 2);
    assert(!GS_GLES31_CHECK(&results[0], 0, "Injected assertion failure"));
    assert(GS_GLES31_CHECK(&results[0], 1, "Later successful assertion"));
    assert(results[0].status == GS_GLES31_FAIL);
    GS_GLES31_reset_results(results, 2);
    assert(results[0].status == GS_GLES31_NOT_RUN);
    assert(GS_GLES31_CHECK(&results[0], 1, "Successful retry"));
    assert(results[0].status == GS_GLES31_PASS);
    GS_GLES31_FAIL_RESULTS(results, 2, "Injected setup failure %u", 7u);
    assert(results[0].status == GS_GLES31_FAIL && results[1].status == GS_GLES31_FAIL);

    /* One TC's dispatch error must not change another TC's result. */
    GS_GLES31_reset_results(results, 2);
    set_errors(GL_INVALID_VALUE, GL_NO_ERROR);
    assert(GS_GLES31_EXPECT_ERROR(&results[0], GL_INVALID_VALUE, "negative dispatch"));
    set_errors(GL_INVALID_VALUE, GL_INVALID_OPERATION);
    assert(!GS_GLES31_EXPECT_ERROR(&results[1], GL_INVALID_VALUE, "extra error"));
    assert(results[0].status == GS_GLES31_PASS && results[1].status == GS_GLES31_FAIL);
    assert(glGetError() == GL_NO_ERROR);
    GS_GLES31_reset_results(results, 2);
    assert(GS_GLES31_EXPECT_ERROR(&results[1], GL_NO_ERROR, "successful retry"));
    assert(results[1].status == GS_GLES31_PASS);

    /* Readback requires the proper barrier and a valid unmap, not just data. */
    assert(GS_GLES31_read_ssbo(17, sizeof(copy), copy));
    assert(memcmp(copy, storage, sizeof(copy)) == 0 && unmap_calls == 1);
    map_failure = 1;
    assert(!GS_GLES31_read_ssbo(17, sizeof(copy), copy));
    assert(unmap_calls == 1);
    map_failure = 0;
    unmap_failure = 1;
    assert(!GS_GLES31_read_ssbo(17, sizeof(copy), copy));
    assert(unmap_calls == 2);
    unmap_failure = 0;
    barrier_failure = 1;
    assert(!GS_GLES31_read_ssbo(17, sizeof(copy), copy));
    assert(unmap_calls == 2);
    barrier_failure = 0;

    /* Program binding checks detect a wrong active program and clear pipeline. */
    active_pipeline = 23;
    assert(GS_GLES31_use_program(42));
    assert(active_program == 42 && active_pipeline == 0);
    wrong_program = 1;
    assert(!GS_GLES31_use_program(42));
    wrong_program = 0;

    /* Missing entry points fail setup safely before TP execution. */
    assert(glGetIntegeri_v == NULL);
    assert(!GS_GLES31_begin_fixture(&fixture));
    GS_GLES31_end_fixture(&fixture);
    assert(cleanup_calls == 1 && fixture.environment.initialized == 0);
    assert(deleted_buffers == 0 && deleted_programs == 0);

    /* A partial resource fixture is fully released; repeated close is safe. */
    assert(GS_GLES31_prepare_environment(&fixture.environment));
    fixture.program = 42;
    fixture.buffers[0] = 17;
    fixture.buffer_count = 1;
    GS_GLES31_end_fixture(&fixture);
    GS_GLES31_end_fixture(&fixture);
    assert(deleted_buffers == 1 && deleted_programs == 1 && cleanup_calls == 2);
    assert(fixture.program == 0 && fixture.buffer_count == 0 && fixture.buffers[0] == 0);
    puts("GLES31 helper regression checks passed (injected failures above are expected).");
    return 0;
}
