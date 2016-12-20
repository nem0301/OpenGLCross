#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Graphic
{
pulbic:
	Graphic(GLFWwindow* window)
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
			fprintf( stderr, "Failed to open GLFW window. If you have an Intel
					GPU, they are not 3.3 compatible. Try the 2.1 version of the
					tutorials.\n" );
			glfwTerminate();
			return -1; 
		}   



	}

};
