#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ipc.h"
#include "Common.h"
#include "Shader.h"

using namespace std;
using namespace glm;


int main(int argc, char *argv[])
{
	if ( !glfwInit() )
	{
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global)
	window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental=GL_TRUE; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint vertexArrayId;
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	static const GLfloat gVertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	GLuint vertexBuffer;

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData),
		gVertexBufferData, GL_STATIC_DRAW);
	
	GLuint programID = Shader::loadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

	GLuint matrixID = glGetUniformLocation(programID, "MVP");
	mat4 proj = perspective(45.0f, 4.0f/3.0f, 0.1f, 100.f);

	mat4 view = lookAt(
			vec3(4, 3, 3), // camera
			vec3(0, 0, 0), // look at
			vec3(0, 1, 0) // head is up
			);

	mat4 model = mat4(1.0f);
	mat4 mvp = proj * view * model;

	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);

		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
				0,			// attribute 0. shader layout
				3,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// nomarlized?
				0,			// stride
				(void*)0	// array buffer offset
				);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0 );

	return 0;
}
