#include "Common.h"
#include "GLCommon.h"
#include "Shader.h"
#include "Ipc.h"
#include "Texture.h"
#include "Control.h"
#include "Model.h"

using namespace std;
using namespace glm;

void fpsCounter()
{
	static float oldTime = glfwGetTime();
	float curTime = glfwGetTime();
	float fps = 1.0 / (curTime - oldTime);

	cout << "FPS : " << fps << endl;

	oldTime = curTime;
}

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
	GLuint Texture2 = Texture::loadDds("./res/uvmap.DDS");

	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");


	Control* mainControl = new Control(window, width, height, vec3(0, 0, 5), 3.14f, 0.0f, 45.0f, 3.0f, 0.005f);

	Model* cubeModel = new Model(vec3(4, 0, 0), TextureID, Texture);
	cubeModel->loadObj("./res/cube.obj");

	Model* suzanneModel = new Model(vec3(6, 7, 6), TextureID, Texture2);
	suzanneModel->loadObj("./res/suzanne.obj");
	
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

		cubeModel->drawObj();
		suzanneModel->drawObj();
		
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

	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &vertexArrayID);

	glfwTerminate();

	return 0;
}
