#include "Common.h"
#include "GLCommon.h"
#include "Shader.h"
#include "Ipc.h"
#include "Texture.h"
#include "Control.h"
#include "Model.h"

using namespace std;
using namespace glm;

int main(int argc, char *argv[])
{
	if ( !glfwInit() )
	{
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	// Open a window and create its OpenGL context
	int width = 2560;
	int height = 1440;
	GLFWwindow* window;
	window = glfwCreateWindow(width, height, "opengl", glfwGetPrimaryMonitor(), NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// enable depth test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	//vertext Array
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// load shader
	GLuint programID = Shader::loadShaders( "vertexShader.vert", "fragmentShader.frag" );

	GLuint mvpMatrixID = glGetUniformLocation(programID, "MVP");
	GLuint viewMatrixID = glGetUniformLocation(programID, "V");
	GLuint modelMatrixID = glGetUniformLocation(programID, "M");

	GLuint Texture = Texture::loadDds("./res/uvtemplate.DDS");

	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");


	Control* mainControl = new Control(window, width, height, vec3(0, 0, 5), 3.14f, 0.0f, 45.0f, 3.0f, 0.005f);

	Model* model = new Model();
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	model->loadObj("./res/cube.obj", vertices, uvs, normals);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3),
		&vertices[0], GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2),
		&uvs[0], GL_STATIC_DRAW);

	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3),
		&normals[0], GL_STATIC_DRAW);
	
	glUseProgram(programID);
	GLuint lightID = glGetUniformLocation(programID, "lightPositionWorldSpace");

	do{
		float curTime = glfwGetTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use shader
		glUseProgram(programID);

		mainControl->computeMVPByInputs(curTime);
		mat4 proj = mainControl->getProjMatrix();
		mat4 view = mainControl->getViewMatrix();
		mat4 model = mat4(1.0f);
		mat4 mvp = proj * view * model;

		// mvp uniform
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);

		vec3 lightPos = vec3(4, 4, 4);
		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// vertex buffer
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

		// uv buffer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glVertexAttribPointer(
				1,			// attribute 0. shader layout
				2,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// nomarlized?
				0,			// stride
				(void*)0	// array buffer offset
				);

		// normal buffer
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glVertexAttribPointer(
				2,			// attribute 0. shader layout
				3,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// nomarlized?
				0,			// stride
				(void*)0	// array buffer offset
				);

		// draw
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		
		// disable
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0 );

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &vertexArrayID);

	glfwTerminate();

	return 0;
}
