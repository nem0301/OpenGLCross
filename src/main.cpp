#include "Common.h"
#include "GLCommon.h"
#include "Shader.h"
// #include "Ipc.h"
#include "Texture.h"
#include "Control.h"
#include "Model.h"
#include "Text2D.h"


using namespace std;
using namespace glm;

int fpsCounter()
{
	static float oldTime = glfwGetTime();
	static int frames = 0;

	float curTime = glfwGetTime();

	frames++;
	if (curTime - oldTime > 1.0)
	{
		int ret = frames;
		frames = 0;
		oldTime += 1.0;
		return ret;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	if ( !glfwInit() )
	{
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

	// Open a window and create its OpenGL context
	//int width = 2560;
	//int height = 1440;
	int width = 1600;
	int height = 900;
	GLFWwindow* window;
	//window = glfwCreateWindow(width, height, "opengl", glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(width, height, "opengl", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// enable depth test
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	//vertext Array
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// load shader
	GLuint shaderID = Shader::loadShaders( "./shader/modelShader.vert", "./shader/modelShader.frag" );

	GLuint mvpMatrixID = glGetUniformLocation(shaderID, "MVP");
	GLuint viewMatrixID = glGetUniformLocation(shaderID, "V");
	GLuint modelMatrixID = glGetUniformLocation(shaderID, "M");
	GLuint mvID = glGetUniformLocation(shaderID, "MV");
	GLuint mv33ID = glGetUniformLocation(shaderID, "MV3x3");
	GLuint diffuseTextureID  = glGetUniformLocation(shaderID, "diffuseTextureSampler");
	GLuint normalTextureID  = glGetUniformLocation(shaderID, "normalTextureSampler");
	GLuint specularTextureID  = glGetUniformLocation(shaderID, "specularTextureSampler");
	GLuint lightID = glGetUniformLocation(shaderID, "lightPositionWorldSpace");

	GLuint cylinderDiffuseTexture = Texture::loadDds("./res/diffuse.DDS");
	GLuint cylinderNormalTexture = Texture::loadBmp("./res/normal.bmp");
	GLuint cylinderSpecularTexture = Texture::loadDds("./res/specular.DDS");

	Model* cylinderModel = new Model(vec3(4, 0, 0), 
        diffuseTextureID, cylinderDiffuseTexture,
        normalTextureID, cylinderNormalTexture,
        specularTextureID, cylinderSpecularTexture,
		shaderID, mvpMatrixID, viewMatrixID, modelMatrixID, 
        mvID, mv33ID,
        lightID);
	cylinderModel->loadObj("./res/cylinder.obj");

	Text2D* text = new Text2D();
	text->initText2D("./res/Holstein.DDS", "./shader/textShader.vert", "./shader/textShader.frag");

	// control
	Control* mainControl = new Control(window, width, height, vec3(0, 0, 5), 3.14f, 0.0f, 45.0f, 3.0f, 0.005f);

	string fpsText = "0";

	mat4 model = mat4(1.0f);
	mat4 view;
	mat4 proj;

	vec3 lightPos = vec3(4, 4, 4);
	cylinderModel->setLightPos(lightPos);

	do{
		float curTime = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// control
		mainControl->computeMVPByInputs(curTime);
		proj = mainControl->getProjMatrix();
		view = mainControl->getViewMatrix();

        cylinderModel->setModel(model);
        cylinderModel->setView(view);
        cylinderModel->setProj(proj);

		// draw objects
		cylinderModel->drawObj();

		//fps
		int frames = fpsCounter();
		if ( frames != -1 )
		{
			fpsText = to_string(frames);
		}
		text->printText(fpsText.c_str(), 0, 0, 30);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0 );

	glDeleteProgram(shaderID);
	glDeleteTextures(1, &diffuseTextureID);
	glDeleteTextures(1, &normalTextureID);
	glDeleteTextures(1, &specularTextureID);
	glDeleteVertexArrays(1, &vertexArrayID);

	delete text;
	glfwTerminate();

	return 0;
}
