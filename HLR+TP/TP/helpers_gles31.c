#include "helpers_gles31.h"

#include <stdarg.h>
#include <string.h>

/* Infrastructure calls consume and report every error; only the TP explicitly
 * discards old errors before the operation under test.
 */
static int check_gl(const char *operation)
{
    GLenum error;
    int success = 1;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        fprintf(stderr, "%s: GL error 0x%x (%s)\n", operation,
            error, GS_GLES31_error_name(error));
        success = 0;
    }
    return success;
}

void GS_GLES31_clear_errors(void)
{
    while (glGetError() != GL_NO_ERROR) { }
}

const char *GS_GLES31_error_name(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        default: return "UNKNOWN_GL_ERROR";
    }
}

GLuint GS_GLES31_compile_shader(GLenum shader_type, const char *source)
{
    GLuint shader;
    GLint status = GL_FALSE;
    GLchar log[1024] = {0};

    if (source == NULL || !check_gl("Before shader creation")) return 0;
    shader = glCreateShader(shader_type);
    if (shader == 0) { check_gl("Create shader"); return 0; }
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!check_gl("Compile shader") || status != GL_TRUE)
    {
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "Shader compilation failed: %s\n", log);
        glDeleteShader(shader);
        check_gl("Failed shader cleanup");
        return 0;
    }
    return shader;
}

/* Takes ownership of the compiled shader objects on every path. */
static GLuint link_program(GLuint *shaders, size_t count)
{
    GLuint program = glCreateProgram();
    GLint status = GL_FALSE;
    GLchar log[1024] = {0};
    size_t i;
    int success;

    if (program != 0)
    {
        for (i = 0; i < count; ++i) glAttachShader(program, shaders[i]);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE)
        {
            glGetProgramInfoLog(program, sizeof(log), NULL, log);
            fprintf(stderr, "Program linking failed: %s\n", log);
        }
        for (i = 0; i < count; ++i) glDetachShader(program, shaders[i]);
    }
    for (i = 0; i < count; ++i) glDeleteShader(shaders[i]);
    success = check_gl("Link program and release shaders");
    if (!success || status != GL_TRUE)
    {
        if (program != 0) glDeleteProgram(program);
        check_gl("Failed program cleanup");
        return 0;
    }
    return program;
}

GLuint GS_GLES31_create_compute_program(const char *source)
{
    GLuint shader = GS_GLES31_compile_shader(GL_COMPUTE_SHADER, source);
    return shader != 0 ? link_program(&shader, 1) : 0;
}

GLuint GS_GLES31_create_graphics_program(const char *vs, const char *fs)
{
    GLuint shaders[2];
    shaders[0] = GS_GLES31_compile_shader(GL_VERTEX_SHADER, vs);
    if (shaders[0] == 0) return 0;
    shaders[1] = GS_GLES31_compile_shader(GL_FRAGMENT_SHADER, fs);
    if (shaders[1] == 0)
    {
        glDeleteShader(shaders[0]);
        check_gl("Partial graphics program cleanup");
        return 0;
    }
    return link_program(shaders, 2);
}

void GS_GLES31_bind_ssbo(GLuint buffer, GLuint binding)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
}

GLuint GS_GLES31_create_ssbo(GLuint binding, GLsizeiptr size,
    const void *initial_data, GLenum usage)
{
    GLuint buffer = 0;
    if (size <= 0 || !check_gl("Before SSBO creation")) return 0;
    glGenBuffers(1, &buffer);
    if (buffer == 0) { check_gl("Generate SSBO"); return 0; }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, initial_data, usage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
    if (!check_gl("Create and bind SSBO"))
    {
        glDeleteBuffers(1, &buffer);
        check_gl("Failed SSBO cleanup");
        return 0;
    }
    return buffer;
}

void GS_GLES31_delete_program(GLuint *program)
{
    if (program != NULL && *program != 0)
    {
        glDeleteProgram(*program);
        *program = 0;
    }
}

void GS_GLES31_delete_buffer(GLuint *buffer)
{
    if (buffer != NULL && *buffer != 0)
    {
        glDeleteBuffers(1, buffer);
        *buffer = 0;
    }
}

void GS_GLES31_reset_results(GS_GLES31_TestResult *results, size_t count)
{
    size_t i;
    for (i = 0; i < count; ++i) results[i].status = GS_GLES31_NOT_RUN;
}

static void fail_result(GS_GLES31_TestResult *result, const char *file,
    int line, const char *format, va_list arguments)
{
    result->status = GS_GLES31_FAIL;
    printf("[FAIL   ][ %s ][ %s : %s:%d ] Test failed: ",
        result->test_case, result->test_procedure, file, line);
    vprintf(format, arguments);
    printf("\n");
}

int GS_GLES31_check_at(GS_GLES31_TestResult *result, int condition,
    const char *file, int line, const char *format, ...)
{
    if (!condition)
    {
        va_list arguments;
        va_start(arguments, format);
        fail_result(result, file, line, format, arguments);
        va_end(arguments);
    }
    else if (result->status != GS_GLES31_FAIL)
        result->status = GS_GLES31_PASS;
    return condition != 0;
}

void GS_GLES31_fail_results_at(GS_GLES31_TestResult *results, size_t count,
    const char *file, int line, const char *format, ...)
{
    size_t i;
    for (i = 0; i < count; ++i)
    {
        va_list arguments;
        va_start(arguments, format);
        fail_result(&results[i], file, line, format, arguments);
        va_end(arguments);
    }
}

void GS_GLES31_report_results(GS_GLES31_TestResult *results, size_t count)
{
    size_t i;
    for (i = 0; i < count; ++i)
    {
        if (results[i].status == GS_GLES31_PASS)
            TEST_LOG_SUCCESS(results[i].test_case, results[i].test_procedure);
        else if (results[i].status == GS_GLES31_NOT_RUN)
            GS_GLES31_FAIL_RESULTS(&results[i], 1, "No test checks were completed");
    }
}

int GS_GLES31_expect_error_at(GS_GLES31_TestResult *result, GLenum expected,
    const char *file, int line, const char *operation)
{
    GLenum error = glGetError();
    int success = GS_GLES31_check_at(result, error == expected, file, line,
        "%s: expected 0x%x (%s), got 0x%x (%s)", operation,
        expected, GS_GLES31_error_name(expected), error, GS_GLES31_error_name(error));
    while ((error = glGetError()) != GL_NO_ERROR)
    {
        GS_GLES31_check_at(result, 0, file, line,
            "%s: additional GL error 0x%x (%s)", operation,
            error, GS_GLES31_error_name(error));
        success = 0;
    }
    return success;
}

int GS_GLES31_begin_fixture(GS_GLES31_TestFixture *fixture)
{
    if (fixture == NULL || fixture->environment.initialized ||
        fixture->program != 0 || fixture->buffer_count != 0)
    {
        fprintf(stderr, "Fixture must be initialized and closed before reuse\n");
        return 0;
    }
    if (!GS_GLES31_prepare_environment(&fixture->environment) ||
        !GS_GLES31_environment_is_current(&fixture->environment))
    {
        fprintf(stderr, "Could not prepare a current GLES 3.1 environment\n");
        return 0;
    }

    /* Never capture a loader pointer before prepare_environment has loaded it. */
#if defined(GS_GLES31_USE_GLAD1) || defined(GS_GLES31_USE_GLAD2)
#define REQUIRE_ENTRY_POINT(entry) do { \
    if ((entry) == NULL) { \
        fprintf(stderr, "Missing GLES entry point: %s\n", #entry); \
        return 0; \
    } \
} while (0)
    REQUIRE_ENTRY_POINT(glGetError);
    REQUIRE_ENTRY_POINT(glGetIntegerv);
    REQUIRE_ENTRY_POINT(glGetIntegeri_v);
    REQUIRE_ENTRY_POINT(glCreateShader);
    REQUIRE_ENTRY_POINT(glShaderSource);
    REQUIRE_ENTRY_POINT(glCompileShader);
    REQUIRE_ENTRY_POINT(glGetShaderiv);
    REQUIRE_ENTRY_POINT(glGetShaderInfoLog);
    REQUIRE_ENTRY_POINT(glDeleteShader);
    REQUIRE_ENTRY_POINT(glCreateProgram);
    REQUIRE_ENTRY_POINT(glAttachShader);
    REQUIRE_ENTRY_POINT(glLinkProgram);
    REQUIRE_ENTRY_POINT(glGetProgramiv);
    REQUIRE_ENTRY_POINT(glGetProgramInfoLog);
    REQUIRE_ENTRY_POINT(glDetachShader);
    REQUIRE_ENTRY_POINT(glDeleteProgram);
    REQUIRE_ENTRY_POINT(glUseProgram);
    REQUIRE_ENTRY_POINT(glBindProgramPipeline);
    REQUIRE_ENTRY_POINT(glGenBuffers);
    REQUIRE_ENTRY_POINT(glBindBuffer);
    REQUIRE_ENTRY_POINT(glBufferData);
    REQUIRE_ENTRY_POINT(glBindBufferBase);
    REQUIRE_ENTRY_POINT(glBufferSubData);
    REQUIRE_ENTRY_POINT(glDeleteBuffers);
    REQUIRE_ENTRY_POINT(glDispatchCompute);
    REQUIRE_ENTRY_POINT(glMemoryBarrier);
    REQUIRE_ENTRY_POINT(glMapBufferRange);
    REQUIRE_ENTRY_POINT(glUnmapBuffer);
#undef REQUIRE_ENTRY_POINT
#endif
    return check_gl("Fixture setup");
}

int GS_GLES31_use_program(GLuint program)
{
    GLint current_program = -1, pipeline = -1;
    if (!check_gl("Before program activation")) return 0;
    glBindProgramPipeline(0);
    glUseProgram(program);
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &pipeline);
    if (!check_gl("Activate program")) return 0;
    if ((GLuint)current_program != program || pipeline != 0)
    {
        fprintf(stderr, "Program/pipeline state does not match requested bindings\n");
        return 0;
    }
    return 1;
}

int GS_GLES31_fixture_compute_program(GS_GLES31_TestFixture *fixture,
    const char *source)
{
    if (fixture == NULL || fixture->program != 0) return 0;
    fixture->program = GS_GLES31_create_compute_program(source);
    return fixture->program != 0 && GS_GLES31_use_program(fixture->program);
}

int GS_GLES31_fixture_graphics_program(GS_GLES31_TestFixture *fixture,
    const char *vs, const char *fs)
{
    if (fixture == NULL || fixture->program != 0) return 0;
    fixture->program = GS_GLES31_create_graphics_program(vs, fs);
    return fixture->program != 0 && GS_GLES31_use_program(fixture->program);
}

GLuint GS_GLES31_fixture_ssbo(GS_GLES31_TestFixture *fixture,
    GLuint binding, GLsizeiptr size, const void *initial, GLenum usage)
{
    GLuint buffer;
    if (fixture == NULL || fixture->buffer_count >=
        sizeof(fixture->buffers) / sizeof(fixture->buffers[0]))
    {
        fprintf(stderr, "Fixture SSBO capacity exceeded\n");
        return 0;
    }
    buffer = GS_GLES31_create_ssbo(binding, size, initial, usage);
    if (buffer != 0)
    {
        fixture->buffers[fixture->buffer_count] = buffer;
        fixture->buffer_bindings[fixture->buffer_count++] = binding;
    }
    return buffer;
}

int GS_GLES31_reset_ssbo(GLuint buffer, GLuint binding,
    GLsizeiptr size, const void *data)
{
    if (buffer == 0 || size <= 0 || data == NULL ||
        !check_gl("Before SSBO reset")) return 0;
    /* Order API writes after any preceding shader writes to this buffer. */
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    if (!check_gl("SSBO reset barrier")) return 0;
    GS_GLES31_bind_ssbo(buffer, binding);
    if (!check_gl("SSBO reset binding")) return 0;
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
    return check_gl("SSBO reset data");
}

int GS_GLES31_read_ssbo(GLuint buffer, GLsizeiptr size, void *destination)
{
    void *mapped;
    GLboolean valid;
    int success;
    if (buffer == 0 || size <= 0 || destination == NULL ||
        !check_gl("Before SSBO readback")) return 0;
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    if (!check_gl("SSBO readback barrier")) return 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    if (!check_gl("SSBO readback binding")) return 0;
    mapped = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT);
    success = check_gl("Map SSBO for CPU readback");
    if (mapped == NULL)
    {
        fprintf(stderr, "SSBO mapping returned NULL\n");
        return 0;
    }
    if (success) memcpy(destination, mapped, (size_t)size);
    valid = glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    if (!check_gl("Unmap SSBO")) success = 0;
    if (valid != GL_TRUE)
    {
        fprintf(stderr, "SSBO contents invalidated during unmap\n");
        success = 0;
    }
    return success;
}

int GS_GLES31_work_group_limit(GLuint axis, GLuint *limit)
{
    GLint value = 0;
    if (axis > 2 || limit == NULL || !check_gl("Before work-group limit query"))
        return 0;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, axis, &value);
    if (!check_gl("Work-group limit query")) return 0;
    if (value <= 0)
    {
        fprintf(stderr, "Invalid maximum work-group count on axis %u: %d\n", axis, value);
        return 0;
    }
    *limit = (GLuint)value;
    return 1;
}

void GS_GLES31_fill_uint(GLuint *data, size_t count, GLuint value)
{
    size_t i;
    for (i = 0; i < count; ++i) data[i] = value;
}

void GS_GLES31_end_fixture(GS_GLES31_TestFixture *fixture)
{
    size_t i;
    if (fixture == NULL) return;
    if (GS_GLES31_environment_is_current(&fixture->environment))
    {
        /* With no GL resources this also works after a partial loader failure. */
        for (i = 0; i < fixture->buffer_count; ++i)
        {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, fixture->buffer_bindings[i], 0);
            GS_GLES31_delete_buffer(&fixture->buffers[i]);
        }
        if (fixture->program != 0)
        {
            glUseProgram(0);
            GS_GLES31_delete_program(&fixture->program);
        }
    }
    GS_GLES31_cleanup_environment(&fixture->environment);
    fixture->program = 0;
    memset(fixture->buffers, 0, sizeof(fixture->buffers));
    memset(fixture->buffer_bindings, 0, sizeof(fixture->buffer_bindings));
    fixture->buffer_count = 0;
}

const char *GS_GLES31_noop_compute_source(void)
{
    return "#version 310 es\n"
        "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
        "void main() {}\n";
}
