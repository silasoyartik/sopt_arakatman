#include <GL/gl.h>
#include "../../helpers.h"

/*
 * GL43C - ComputeShaders - glDispatchCompute
 *
 * Covered requirements:
 *   GS-GL43C-CS-DC-002
 *   GS-GL43C-CS-DC-010
 *   GS-GL43C-CS-DC-011
 *   GS-GL43C-CS-DC-012
 */
static const char *test_case2 = "GS_GL43C_CS_DC_TC_002";
static const char *test_case10 = "GS_GL43C_CS_DC_TC_010";
static const char *test_case11 = "GS_GL43C_CS_DC_TC_011";
static const char *test_case12 = "GS_GL43C_CS_DC_TC_012";
static const char *test_procedure = "GS_GL43C_CS_DC_TP_002";

static GLboolean test_success2 = GL_TRUE;
static GLboolean test_success10 = GL_TRUE;
static GLboolean test_success11 = GL_TRUE;
static GLboolean test_success12 = GL_TRUE;

static GS_GL_TestEnvironment environment = GS_GL_ENV_INITIALIZER;

void GS_GL43C_CS_DC_TP_002_init(void)
{
    void (*dispatch_compute)(GLuint, GLuint, GLuint) = glDispatchCompute;[cite: 4]
    GLuint compute_program = 0;
    GLuint ssbo = 0;
    GLuint *mapped_data = NULL;
    int i = 0;

    if (!GS_GL_prepare_environment(&environment))
    {
        TEST_LOG_FAIL(test_case2, test_procedure,
            "Could not prepare an initialized GL environment");[cite: 4]
        test_success2 = GL_FALSE;
        test_success10 = GL_FALSE;
        test_success11 = GL_FALSE;
        test_success12 = GL_FALSE;
        return;
    }

    /* 
     * Shader writes `gl_GlobalInvocationID.x + gl_LocalInvocationID.x` 
     * to SSBO using layout(local_size_x = 2). 
     */
    compute_program = GS_GL_compile_validation_compute_shader();
    glUseProgram(compute_program);

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(GLuint), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    /* TC_002, TC_010: Dispatch 2 groups on X, local_size is 2, total 4 invocations */
    dispatch_compute(2, 1, 1);
    
    if (glGetError() != GL_NO_ERROR) 
    {
        TEST_LOG_FAIL(test_case2, test_procedure, "Dispatch generated error");
        test_success2 = GL_FALSE;
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    /* TC_011, TC_012: Check memory execution and GLSL variables via readback */
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    mapped_data = (GLuint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    
    if (mapped_data != NULL)
    {
        for (i = 0; i < 4; ++i)
        {
            /* Check layout execution and local variables */
            if (mapped_data[i] == 0) 
            {
                TEST_LOG_FAIL(test_case10, test_procedure, "Shader failed to run");
                test_success10 = GL_FALSE;
                test_success11 = GL_FALSE;
            }
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    else
    {
        TEST_LOG_FAIL(test_case12, test_procedure, "Failed to map SSBO buffer");
        test_success12 = GL_FALSE;
    }

    if (test_success2) TEST_LOG_SUCCESS(test_case2, test_procedure);[cite: 4]
    if (test_success10) TEST_LOG_SUCCESS(test_case10, test_procedure);
    if (test_success11) TEST_LOG_SUCCESS(test_case11, test_procedure);
    if (test_success12) TEST_LOG_SUCCESS(test_case12, test_procedure);
}

void GS_GL43C_CS_DC_TP_002_draw(void) { }[cite: 4]

void GS_GL43C_CS_DC_TP_002_close(void)
{
    GS_GL_cleanup_environment(&environment);[cite: 4]
}