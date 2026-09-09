#include <glad/glad.h>
#include <stdio.h>
#include <stdint.h>

/*
 * All-in-one scenario file for glDispatchCompute(num_groups_x, num_groups_y, num_groups_z).
 * Validates successful dispatches as well as boundary, state, and parameter error conditions.
 */

static const char *gl_error_name(GLenum error)
{
    switch (error) {
    case GL_NO_ERROR:                      return "GL_NO_ERROR";
    case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
    case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
    default:                               return "UNKNOWN_GL_ERROR";
    }
}

static void run_dispatch_scenario(const char *name,
                                  GLuint num_groups_x,
                                  GLuint num_groups_y,
                                  GLuint num_groups_z,
                                  GLenum expected_error)
{
    GLenum error;
    GLboolean passed;

    // Önceki hataları temizle
    while (glGetError() != GL_NO_ERROR);

    // Compute shader tetikleme fonksiyonu çağrılır
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    
    error = glGetError();

    if (expected_error == GL_NO_ERROR) {
        passed = (error == GL_NO_ERROR);
    } else {
        passed = (error == expected_error);
    }

    printf("%s: %s (error=%s)\n",
           name,
           passed ? "PASS" : "FAIL",
           gl_error_name(error));
}

/* SCENARIO 1 - Normal 1D dispatch with valid work group counts. */
void scenario_dispatch_normal_1d(void)
{
    run_dispatch_scenario("Scenario 1 - normal 1d dispatch", 64, 1, 1, GL_NO_ERROR);
}

/* SCENARIO 2 - Normal 2D dispatch with valid work group counts. */
void scenario_dispatch_normal_2d(void)
{
    run_dispatch_scenario("Scenario 2 - normal 2d dispatch", 16, 16, 1, GL_NO_ERROR);
}

/* SCENARIO 3 - Normal 3D dispatch with valid work group counts. */
void scenario_dispatch_normal_3d(void)
{
    run_dispatch_scenario("Scenario 3 - normal 3d dispatch", 4, 4, 4, GL_NO_ERROR);
}

/* SCENARIO 4 - Zero work groups in X axis (valid call, executes nothing, produces no error). */
void scenario_dispatch_zero_groups_x(void)
{
    run_dispatch_scenario("Scenario 4 - zero groups x", 0, 1, 1, GL_NO_ERROR);
}

/* SCENARIO 5 - Zero work groups in Y axis (valid call, executes nothing). */
void scenario_dispatch_zero_groups_y(void)
{
    run_dispatch_scenario("Scenario 5 - zero groups y", 1, 0, 1, GL_NO_ERROR);
}

/* SCENARIO 6 - Zero work groups in Z axis (valid call, executes nothing). */
void scenario_dispatch_zero_groups_z(void)
{
    run_dispatch_scenario("Scenario 6 - zero groups z", 1, 1, 0, GL_NO_ERROR);
}

/* 
 * SCENARIO 7 - Exceeding the maximum hardware work group count limit in X.
 * The fixture must query GL_MAX_COMPUTE_WORK_GROUP_COUNT first and supply limit + 1.
 */
void scenario_dispatch_exceed_max_x(void)
{
    GLint max_x = 65535; // Varsayılan min sınır
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_x);
    
    run_dispatch_scenario("Scenario 7 - exceed max work groups x", (GLuint)(max_x + 1), 1, 1, GL_INVALID_VALUE);
}

/* 
 * SCENARIO 8 - Exceeding the maximum hardware work group count limit in Y.
 */
void scenario_dispatch_exceed_max_y(void)
{
    GLint max_y = 65535;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_y);
    
    run_dispatch_scenario("Scenario 8 - exceed max work groups y", 1, (GLuint)(max_y + 1), 1, GL_INVALID_VALUE);
}

/* 
 * SCENARIO 9 - Exceeding the maximum hardware work group count limit in Z.
 */
void scenario_dispatch_exceed_max_z(void)
{
    GLint max_z = 65535;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_z);
    
    run_dispatch_scenario("Scenario 9 - exceed max work groups z", 1, 1, (GLuint)(max_z + 1), GL_INVALID_VALUE);
}

/* 
 * SCENARIO 10 - Dispatching without an active shader program (glUseProgram(0)).
 * Expected to fail with GL_INVALID_OPERATION.
 */
void scenario_dispatch_no_active_program(void)
{
    glUseProgram(0);
    run_dispatch_scenario("Scenario 10 - no active program", 1, 1, 1, GL_INVALID_OPERATION);
}

/* 
 * SCENARIO 11 - Dispatching with an active program that contains NO compute shader 
 * (e.g., only vertex/fragment shaders are attached).
 * Expected to fail with GL_INVALID_OPERATION.
 */
void scenario_dispatch_graphics_program_active(GLuint graphics_program)
{
    glUseProgram(graphics_program);
    run_dispatch_scenario("Scenario 11 - graphics program active", 1, 1, 1, GL_INVALID_OPERATION);
}