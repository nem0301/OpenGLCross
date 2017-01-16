#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPositionModelSpace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormalModelSpace;
layout(location = 3) in vec3 vertexTangentModelSpcae;
layout(location = 3) in vec3 vertexBitangentModelSpcae;


out vec2 UV;
out vec3 positionWorldSpace;
out vec3 eyeDirectionCameraSpace;
out vec3 lightDirectionCameraSpace;

out vec3 lightDirectionTangentSpace;
out vec3 eyeDirectionTangentSpace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
unifomr mat4 MV3x3
uniform vec3 lightPositionWorldSpace;

void main(){
    gl_Position = MVP * vec4(vertexPositionModelSpace, 1);

	positionWorldSpace = (M * vec4(vertexPositionModelSpace, 1)).xyz;

	vec3 vertexPositionCameraSpace = (V * M * vec4(vertexPositionModelSpace, 1)).xyz;
	eyeDirectionCameraSpace = vec3(0, 0, 0) - vertexPositionCameraSpace;

	vec3 lightPositionCameraSpace = (V * vec4(lightPositionWorldSpace, 1)).xyz;
	lightDirectionCameraSpace = lightPositionCameraSpace + eyeDirectionCameraSpace;

	UV = vertexUV;

    vec3 vertexTangentCameraSpace = MV3x3 * vertexTangentModelSpace;
    vec3 vertexBitangentsCameraSpace = MV3x3 * vertexBitangentModelSpace;
    vec3 vertexNormalCameraSpcae = MV3x3 * vertexNormalModelSpace;

    mat3 TBN = transpose(mat3{
            vertexTangentCameraSpace,
            vertexBitangentCameraSpace,
            vertexNormalCameraSpace));
    

    lightDirectionTangentSpace = TBN * lightDirectionCameraSpace;
    eyeDirectionTangentSpace = TBN * eyeDirectionCameraSpace;
}

