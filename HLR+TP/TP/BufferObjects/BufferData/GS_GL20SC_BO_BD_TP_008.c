#include <stdio.h>
#include <stdlib.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferData

Function shall copy the data in the memory area pointed by the data to the created data store if the data is not NULL.

Function shall correctly upload data with STATIC_DRAW hint and draw a triangle.

Covered requirements:
	- GS-GL20SC-BO-BD-003
	- GS-GL20SC-BO-BD-004
*/


static const char* test_case3 = "GS_GL20SC_BO_BD_TC_003";
static const char* test_case4 = "GS_GL20SC_BO_BD_TC_004";
static const char* test_procedure = "GS_GL20SC_BO_BD_TP_008";

#ifdef __ubuntu__

/* ---- Shader sources ---- */
static const char* vertexShaderSource =
    "#version 100\n"
    "attribute vec2 aPos;\n"
	"attribute vec3 aCol;\n"
	"varying vec3 vCol;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
	"	vCol = aCol;"
    "}\n";

static const char* fragmentShaderSource =
    "#version 100\n"
    "precision mediump float;\n"
	"varying vec3 vCol;\n"
    "void main() {\n"
    "   gl_FragColor = vec4(vCol, 1.0);\n"
    "}\n";

#else

static const uint8_t binary_program[] =
{
#ifdef __x86_64__
	#include "shaders/le/GS_GL20SC_BO_BD_TP_008.dat"
#else
	#include "shaders/be/GS_GL20SC_BO_BD_TP_008.dat"
#endif
};

#endif

/* ---- Static state ---- */
static GLuint s_shaderProgram;
static GLuint s_vbo;
static GLint vp[4];

static GLboolean test_success3 = GL_TRUE;
static GLboolean test_success4 = GL_TRUE;
static int first_entry = 1;

/* Initialization */
void GS_GL20SC_BO_BD_TP_008_init(void) {
    // Create shader program
#ifdef __ubuntu__
    s_shaderProgram = create_program(vertexShaderSource, fragmentShaderSource);
#else
    s_shaderProgram = create_program(binary_program, sizeof(binary_program));
#endif

    // Define a simple triangle
    GLfloat vertices[] = {
    	// Position	   // Color (GREEN)
        -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.0f,  0.5f,  0.0f, 1.0f, 0.0f
    };

    // Generate and bind VBO
    glGenBuffers(1, &s_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

    // Upload data with STATIC_DRAW hint and non NULL data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // Test Case 003

    // Check the buffer size and error status.
    // Data test will be done with the inspection method.

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) {
    	TEST_LOG_FAIL(test_case3, test_procedure, "No error expected on glBufferData call with non null Data, got: 0x%x", err);
    	test_success3 = GL_FALSE;
    }

    GLint size;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    if (size != sizeof(vertices)) {
    	TEST_LOG_FAIL(test_case3, test_procedure, "Buffer size shall equal: %d, got: %d", sizeof(vertices), size);
    	test_success3 = GL_FALSE;
    }

	CHECK_ERROR(test_procedure);

    // Test Case 004

    GLint usage;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);

    if (usage != GL_STATIC_DRAW) {
    	TEST_LOG_FAIL(test_case4, test_procedure, "buffer usage expected GL_STATIC_DRAW, got: 0x%x", usage);
    	test_success4 = GL_FALSE;
    }

    // Use shader program
	glUseProgram(s_shaderProgram);

	// Bind VBO and configure vertex attribute
	GLint posAttrib = glGetAttribLocation(s_shaderProgram, "aPos");
	GLint colAttrib = glGetAttribLocation(s_shaderProgram, "aCol");

	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(GL_FLOAT) * 2));

	CHECK_ERROR(test_procedure);

	glGetIntegerv(GL_VIEWPORT, vp);
	CHECK_ERROR(test_procedure);

	first_entry = 1;
}

/* Draw */
void GS_GL20SC_BO_BD_TP_008_draw(void) {
	GLubyte pixel[4];

    // Draw triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        TEST_LOG_FAIL(test_case4, test_procedure, "no error was expecting on drawing with GL_STATIC_DRAW, got: 0x%x.", err);
        test_success4 = GL_FALSE;
    }

#ifdef __ubuntu__
    glReadPixels(vp[0] + vp[2]/2, vp[1] + vp[3]/2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
#else
    glReadnPixels(vp[0] + vp[2]/2, vp[1] + vp[3]/2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, sizeof(pixel), pixel);
#endif

    if (!color_check(GL_RG, pixel) && first_entry) {
    	TEST_LOG_FAIL(test_case3, test_procedure, "Green triangle expected at the center of viewport, center pixel color got: %u %u %u", pixel[0], pixel[1], pixel[2]);
    	test_success3 = GL_FALSE;
    }

    if(test_success3 && first_entry) {
        TEST_LOG_SUCCESS(test_case3, test_procedure);
    }

    if(test_success4 && first_entry) {
        TEST_LOG_SUCCESS(test_case4, test_procedure);
    }

    first_entry = 0;
}

/* Cleanup */
void GS_GL20SC_BO_BD_TP_008_close(void) {
#ifdef __ubuntu__
    glDeleteBuffers(1, &s_vbo);
    glDeleteProgram(s_shaderProgram);
#endif
}


