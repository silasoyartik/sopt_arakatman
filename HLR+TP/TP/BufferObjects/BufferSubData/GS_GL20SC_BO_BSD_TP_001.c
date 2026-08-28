#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../../test_utility.h"

/*
GL20SC - BufferObjects - BufferSubData

Graphics library shall provide glBufferSubData function.

Testing the main functionality. Function shall copy the data to buffer object, starting at offset.

Covered requirements:
	- GS-GL20SC-BO-BSD-001
	- GS-GL20SC-BO-BSD-002
*/

static const char* test_case2 = "GS_GL20SC_BO_BSD_TC_002";
static const char* test_case1 = "GS_GL20SC_BO_BSD_TC_001";
static const char* test_procedure = "GS_GL20SC_BO_BSD_TP_001";

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
    "   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

#else

static const uint8_t binary_program[] =
{
#ifdef __x86_64__
	#include "shaders/le/GS_GL20SC_BO_BSD_TP_001.dat"
#else
	#include "shaders/be/GS_GL20SC_BO_BSD_TP_001.dat"
#endif
};

#endif

/* ---- Static state ---- */
static GLboolean test_success1 = GL_TRUE;
static GLboolean test_success2 = GL_TRUE;
static GLuint s_shaderProgram;
static GLuint s_vbo;
static GLuint s_ibo;
static GLint posAttrib;
static int first_entry = 1;

/* Initialization */
void GS_GL20SC_BO_BSD_TP_001_init(void) {
	CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    s_shaderProgram = create_program(vertexShaderSource, fragmentShaderSource);
#else
    s_shaderProgram = create_program(binary_program, sizeof(binary_program));
#endif
	CHECK_ERROR(test_procedure);

	// Generate and bind VBO
	// Define a simple triangle
	GLfloat vertices[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.0f,  0.5f
	};

	glGenBuffers(1, &s_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

	// Upload data with DYNAMIC_DRAW hint
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	CHECK_ERROR(test_procedure);

	// Generate a simple IBO
	glGenBuffers(1, &s_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibo);

	// Wrong indices at first
	GLuint wrong_indices[] = {0,0,0};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wrong_indices), wrong_indices, GL_DYNAMIC_DRAW);
	CHECK_ERROR(test_procedure);

	// Use shader program
	glUseProgram(s_shaderProgram);

	// Bind VBO and configure vertex attribute
	posAttrib = glGetAttribLocation(s_shaderProgram, "aPos");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

	CHECK_ERROR(test_procedure);

	GLenum err;

	// Overwrite the first vertice {-0.5f, -0.5f}
	GLfloat newVertice[2] = {-0.8f, -0.8f};

	// Test Case 001

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newVertice), newVertice);

	err = glGetError();

	if (err != GL_NO_ERROR) {
		TEST_LOG_FAIL(test_case1, test_procedure, "No error expected when using the glBufferSubData function, got:0x%x", err);
		test_success1 = GL_FALSE;
	}

	if (test_success1) {
		TEST_LOG_SUCCESS(test_case1, test_procedure);
	}

	// Test Case 002

	// Overwrite the second vertice {0.5f, -0.5f}
	newVertice[0] = 0.8f; newVertice[1] = -0.8f;
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(newVertice), sizeof(newVertice), newVertice);

	err = glGetError();

	if (err != GL_NO_ERROR) {
	    TEST_LOG_FAIL(test_case2, test_procedure, "glBufferSubData with GL_ARRAY_BUFFER failed, got: 0x%x", err);
		test_success2 = GL_FALSE;
	}

	// Overwrite the last vertice {0.0f, 0.8f}
	newVertice[0] = 0.0f; newVertice[1] = 0.8f;
	glBufferSubData(GL_ARRAY_BUFFER, 2* sizeof(newVertice), sizeof(newVertice), newVertice);

	err = glGetError();

	if (err != GL_NO_ERROR) {
	    TEST_LOG_FAIL(test_case2, test_procedure, "glBufferSubData with GL_ARRAY_BUFFER failed, got: 0x%x", err);
		test_success2 = GL_FALSE;
	}

	// Overwrite wrong indices with correct ones
	GLushort correctIndices[] = {0, 1, 2};
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(correctIndices), correctIndices);

	err = glGetError();
	if (err != GL_NO_ERROR) {
	    TEST_LOG_FAIL(test_case2, test_procedure, "glBufferSubData with GL_ELEMENT_ARRAY_BUFFER failed, got: 0x%x", err);
	    test_success2 = GL_FALSE;
	}

	first_entry = 1;
}

/* Draw */
void GS_GL20SC_BO_BSD_TP_001_draw(void) {

#ifdef __ubuntu__
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR && first_entry) {
	    TEST_LOG_FAIL(test_case2, test_procedure, "glDrawElements failed, got: 0x%x", err);
	    test_success2 = GL_FALSE;
	}
#else
	glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, 0);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR && first_entry) {
	    TEST_LOG_FAIL(test_case2, test_procedure, "glDrawRangeElements failed, got: 0x%x", err);
	    test_success2 = GL_FALSE;
	}
#endif

	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);;

	float test_cord[3][2] = {
			{-0.7f, -0.7f},
			{ 0.7f, -0.7f},
			{ 0.0f,  0.7f}
	};

	/* Here we check if the BufferSubData correctly set the new data */
	/* We check coordinates where with initial data would be black, but with SubData should be red */
	for (int i = 0; i < 3; ++i) {
		/* Turn Normalized coordinates into pixel coordinates */
		int px = vp[0] + (int)((test_cord[i][0] * 0.5f + 0.5f) * vp[2]);
		int py = vp[1] + (int)((test_cord[i][1] * 0.5f + 0.5f) * vp[3]);

		GLubyte pixel[4];
#ifdef __ubuntu__
		glReadPixels(px, py, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
#else
		glReadnPixels(px, py, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, sizeof(pixel), pixel);
#endif

		/* Red pixel expected */
		if (!(pixel[0] > 250 && pixel[1] < 5 && pixel[2] < 5) && first_entry) {
			TEST_LOG_FAIL(test_case2, test_procedure, "Red pixel expected. Instead got, r:%u, g:%u, b:%u", pixel[0], pixel[1], pixel[2]);
			test_success2 = GL_FALSE;
		}
	}

	if (test_success2 && first_entry) {
		TEST_LOG_SUCCESS(test_case2, test_procedure);
	}

	first_entry = 0;
}

/* Cleanup */
void GS_GL20SC_BO_BSD_TP_001_close(void) {
	CHECK_ERROR(test_procedure);
#ifdef __ubuntu__
    glDeleteBuffers(1, &s_vbo);
    glDeleteBuffers(1, &s_ibo);
#endif
	CHECK_ERROR(test_procedure);
}
