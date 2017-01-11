#version 330 core

layout(location = 0) in vec2 vertexPositionScreenSpace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

void main ()
{
	vec2 vertexPositionHomoSpace = vertexPositionScreenSpace - vec2(400, 300);
	vertexPositionHomoSpace /= vec2(400, 300);
	gl_Position = vec4(vertexPositionHomoSpace, 0, 1);

	UV = vertexUV;
}
