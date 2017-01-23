#include "Common.h"
#include "GLCommon.h"
#include "Shader.h"
#include "Texture.h"
#include "Control.h"
#include "Model.h"
#include "Text2D.h"

GLFWwindow* window;

int fpsCounter();
int initGL(int &width, int &height);
void initAttributes();

int main(int argc, char *argv[])
{
	int width = 1600;
	int height = 900;

	if (initGL(width, height) < 0)
	{
		return -1;
	}
	
	initAttributes();

	//vertext Array
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	
	GLuint shaderID = loadShaders(
		"./shader/modelShader.vert", 
		"./shader/modelShader.frag");
	
	GLuint mvpMatrixID = glGetUniformLocation(shaderID, "MVP");
	GLuint viewMatrixID = glGetUniformLocation(shaderID, "V");
	GLuint modelMatrixID = glGetUniformLocation(shaderID, "M");
	GLuint mv33MatrixID = glGetUniformLocation(shaderID, "MV3x3");

	GLuint diffuseTexture = loadDDS("./res/diffuse.DDS");
	GLuint normalTexture = loadBMP("./res/normal.bmp");
	GLuint specularTexture = loadDDS("./res/specular.DDS");

	GLuint diffuseTextureID = glGetUniformLocation(shaderID, "diffuseTextureSampler");
	GLuint normalTextureID = glGetUniformLocation(shaderID, "normalTextureSampler");
	GLuint specularTextureID = glGetUniformLocation(shaderID, "specularTextureSampler");

	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;

	// load object
	loadOBJ("./res/cylinder.obj", vertices, uvs, normals);

	vector<vec3> tangents;
	vector<vec3> bitangents;

	// compute normal
	computeTangentsBasis(
		vertices, uvs, normals,
		tangents, bitangents);

	vector<unsigned short> indices;
	vector<vec3> indexedVertices;
	vector<vec2> indexedUvs;
	vector<vec3> indexedNormals;
	vector<vec3> indexedTangents;
	vector<vec3> indexedBitangents;
	
	// indexing
	indexVBOTBN(
		vertices, uvs, normals, 
		tangents, bitangents,
		indices,
		indexedVertices, indexedUvs, indexedNormals,
		indexedTangents, indexedBitangents);
	
		
	GLuint vertexBufferID;
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(vec3), 
		&indexedVertices[0], GL_STATIC_DRAW);

	GLuint uvBufferID;
	glGenBuffers(1, &uvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, indexedUvs.size() * sizeof(vec2), 
		&indexedUvs[0], GL_STATIC_DRAW);

	GLuint normalBufferID;
	glGenBuffers(1, &normalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(vec3), 
		&indexedNormals[0], GL_STATIC_DRAW);

	GLuint tangentBufferID;
	glGenBuffers(1, &tangentBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, tangentBufferID);
	glBufferData(GL_ARRAY_BUFFER, indexedTangents.size() * sizeof(vec3), 
		&indexedTangents[0], GL_STATIC_DRAW);

	GLuint bitangentBufferID;
	glGenBuffers(1, &bitangentBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentBufferID);
	glBufferData(GL_ARRAY_BUFFER, indexedBitangents.size() * sizeof(vec3), 
		&indexedBitangents[0], GL_STATIC_DRAW);

	GLuint elementBufferID;
	glGenBuffers(1, &elementBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short),
		&indices[0], GL_STATIC_DRAW);

	glUseProgram(shaderID);

	GLuint lightID = glGetUniformLocation(shaderID, "lightPositionWorldSpace");

	string fpsText = "0";
	// for control width and hight
	int w = 1600;
	int h = 900;
	Control* control = new Control (window, w, h, vec3(0, 0 ,5), 
			3.14f, 0.0f, 45.0f, 1.0f, 0.005f);

	Text2D* text = new Text2D();
	text->initText2D("./res/Holstein.DDS", "./shader/textShader.vert", "./shader/textShader.frag");


	// render to texture
	GLuint frameBufferID = 0;
	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

	GLuint renderedTextureID;
	glGenTextures(1, &renderedTextureID);
	glBindTexture(GL_TEXTURE_2D, renderedTextureID);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint depthRenderBufferID;
	glGenRenderbuffers(1, &depthRenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferID);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTextureID, 0);
	
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}	

	// draw simple quad
	GLuint quadVertexArrayID;
	glGenVertexArrays(1, &quadVertexArrayID);
	glBindVertexArray(quadVertexArrayID);

	static const GLfloat quadVertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	
	GLuint quadVertexBufferID;
	glGenBuffers(1, &quadVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, quadVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexBufferData), 
		quadVertexBufferData, GL_STATIC_DRAW);

	GLuint quadShaderID = loadShaders(
			"./shader/passThrough.vert",
			"./shader/passThrough.frag");
	GLuint texID = glGetUniformLocation(quadShaderID, "renderedTexture");
	GLuint timeID = glGetUniformLocation(quadShaderID, "time");
	

	do{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		glViewport(0, 0, width, height);
		
		//fps
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderID);

		control->computeMVPByInputs();

		mat4 proj = control->getProjMatrix();
		mat4 view = control->getViewMatrix();
		mat4 model = mat4(1.0f);
		mat4 mvp = proj * view * model;
		mat4 mv = view * model;
		mat3 mv33 = mat3(mv);

		// uniform model's matrices
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix3fv(mv33MatrixID, 1, GL_FALSE, &mv33[0][0]);

		// uniform light
		vec3 lightPos = vec3(4, 4, 4);
		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(diffuseTextureID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glUniform1i(normalTextureID, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, specularTexture);
		glUniform1i(specularTextureID, 2);


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(
			0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(
			1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glVertexAttribPointer(
			2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, tangentBufferID);
		glVertexAttribPointer(
			3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, bitangentBufferID);
		glVertexAttribPointer(
			4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
		// draw triangles
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

		for (int i = 0; i < 5; i++)
		{
			glDisableVertexAttribArray(i);
		}

		int frames = fpsCounter();
		if ( frames != -1 )
		{
			fpsText = to_string(frames);
		}
		text->printText(fpsText.c_str(), 0, 0, 30);

		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
		// Render on the whole framebuffer, complete from the lower left corner to the upper right
		glViewport(0, 0, width, height);

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(quadShaderID);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderedTextureID);
		// Set our "renderedTexture" sampler to user Texture Unit 0
		glUniform1i(texID, 0); 

		glUniform1f(timeID, (float)(glfwGetTime()*10.0f) );

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quadVertexBufferID);
		glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
				);  

		// Draw the triangles !
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

		glDisableVertexAttribArray(0);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0 );

	glfwTerminate();

	return 0;
}

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

int initGL(int &width, int &height)
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

	//window = glfwCreateWindow(width, height, "opengl", glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(width, height, "opengl", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW

	glfwGetFramebufferSize(window, &width, &height);

	glewExperimental=GL_TRUE; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");	
		return -1;
	}
	return 0;
}

void initAttributes()
{
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// enable depth test
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
}
