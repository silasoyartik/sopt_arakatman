#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Function shall correctly upload data with GL_STREAM_DRAW hint and allow updating buffer contents.

Covered requirements:
	- GS-GL20SC-BO-BD-006
*/

static const char* test_case = "GS_GL20SC_BO_BD_TC_006";
static const char* test_procedure = "GS_GL20SC_BO_BD_TP_010";

#ifdef __ubuntu__

/* ---- Shader sources ---- */
static const char* vertexShaderSource =
    "#version 100\n"
    "attribute vec2 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\n";

static const char* fragmentShaderSource =
    "#version 100\n"
    "precision mediump float;\n"
    "void main() {\n"
    "   gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
    "}\n";

#else

static const uint8_t binary_program[] =
{
#ifdef __x86_64__
	#include "shaders/le/GS_GL20SC_BO_BD_TP_010.dat"
#else
	#include "shaders/be/GS_GL20SC_BO_BD_TP_010.dat"
#endif
};

#endif

/* ---- Static state ---- */
static GLuint s_shaderProgram;
static GLuint s_vbo;
static GLboolean test_success = GL_TRUE;

static int first_entry;
static float offset = 0.0f;
static float step = 0.01f;

static GLuint frame = 0;

static GLfloat vertices[] = {
	-0.5f, -0.5f,
	 0.5f, -0.5f,
	 0.0f,  0.5f
};

/* Initialization */
void GS_GL20SC_BO_BD_TP_010_init(void) {
    // Create shader program
#ifdef __ubuntu__
    s_shaderProgram = create_program(vertexShaderSource, fragmentShaderSource);
#else
    s_shaderProgram = create_program(binary_program, sizeof(binary_program));
#endif

    // Define initial triangle
    GLfloat vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.0f,  0.5f
    };

    // Generate and bind VBO
    glGenBuffers(1, &s_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

    // Upload data with STREAM_DRAW hint
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

    CHECK_ERROR(test_procedure);

    GLint usage;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);

    // Covered requirement 006
    if (usage != GL_STREAM_DRAW) {
        TEST_LOG_FAIL(test_case, test_procedure, "buffer usage expected GL_STREAM_DRAW, got: 0x%x", usage);
        test_success = GL_FALSE;
    }

    // Use shader program
    glUseProgram(s_shaderProgram);

    // Bind VBO and configure vertex attribute
    GLint posAttrib = glGetAttribLocation(s_shaderProgram, "aPos");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    CHECK_ERROR(test_procedure);

    first_entry = 1;
}

void GS_GL20SC_BO_BD_TP_010_draw(void) {
    // Oscillate triangle vertically
    if (offset > 0.25f || offset < -0.25f) step = -step;
    offset += step;

    if (frame >= 100) {
    	frame = 0;

    	vertices[1] = -0.5f + offset;
    	vertices[3] = -0.5f + offset;
    	vertices[5] =  0.5f + offset;

        // Update buffer contents dynamically
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    }

    // Draw triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case, test_procedure, "no error was expecting on drawing with GL_DYNAMIC_DRAW, got: 0x%x.", err);
        test_success = GL_FALSE;
    }

    if (test_success && first_entry) {
        TEST_LOG_SUCCESS(test_case, test_procedure);
    }

    first_entry = 0;
    ++frame;
}

/* Cleanup */
void GS_GL20SC_BO_BD_TP_010_close(void) {
#ifdef __ubuntu__
    glDeleteBuffers(1, &s_vbo);
    glDeleteProgram(s_shaderProgram);
#endif
}


