#include "Common.h"
#include "GLCommon.h"

class Control
{
	GLFWwindow* window;
	int width;
	int height;
	vec3 position;
	float horizontalAngle;
	float verticalAngle;
	float fov;
	float speed;
	float mouseSpeed;
	double mx, my;

	mat4 proj;
	mat4 view;
	
	
public:
	Control (GLFWwindow* window, int width, int height, vec3 pos, float hAngle, float vAngle, float fov, float speed, float mouseSpeed)
	{
		this->window = window;
		this->width = width;
		this->height = height;
		this->position = pos;
		this->horizontalAngle = hAngle;
		this->verticalAngle = vAngle;
		this->fov = fov;
		this->speed = speed;
		this->mouseSpeed = mouseSpeed;
		this->mx = width/2;
		this->my = height/2;
	}
	
	void computeMVPByInputs()
	{
		static float lastTime = glfwGetTime();
		float curTime = glfwGetTime();
		float deltaTime = (curTime - lastTime);
		lastTime = curTime;

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glfwSetCursorPos(window, mx, my);

		horizontalAngle += mouseSpeed * deltaTime * (mx - xpos) * 8;
		verticalAngle += mouseSpeed * deltaTime * (my - ypos) * 8;

		vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
			);

		vec3 right(
			sin(horizontalAngle - 3.14f/2.0f),
			0,
			cos(horizontalAngle - 3.14f/2.0f)
			);
		
		vec3 up = cross(right, direction);

		if ( glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS )
		{
			position += direction * deltaTime * speed;
		}

		if ( glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS )
		{
			position -= direction * deltaTime * speed;
		}

		if ( glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS )
		{
			position += right * deltaTime * speed;
		}

		if ( glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS )
		{
			position -= right * deltaTime * speed;
		}

		if ( glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS )
		{
			position += vec3(0.0f, 1.0f, 0.0f) * deltaTime * speed;
		}
		/*
		else if ( position[1] > 0 )
		{
			position -= vec3(0.0f, 1.0f, 0.0f) * deltaTime * speed * 0.2f;
		}
		*/


		proj = perspective(fov, (float)width/(float)height, 0.1f, 100.f);
		view = lookAt(
			position,
			position+direction,
			up
			);

	}

	mat4 getProjMatrix()
	{
		return proj;
	}

	mat4 getViewMatrix()
	{
		return view;
	}

};
