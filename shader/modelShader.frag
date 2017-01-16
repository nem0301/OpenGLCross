#version 330 core

in vec2 UV;
in vec3 positionWorldSpace;
in vec3 eyeDirectionCameraSpace;
in vec3 lightDirectionCameraSpace;

in vec3 lightDirectionTangentSpace;
in vec3 eyeDirectionTangentSpace;

// Ouput data
out vec3 color;

uniform sampler2D diffuseTextureSampler;
uniform sampler2D normalTextureSampler;
uniform sampler2D specularTextureSampler;
uniform mat4 V;
uniform mat4 M;
uniform mat4 MV3x3;
uniform vec3 lightPositionWorldSpace;

void main()
{
	vec3 lightColor = vec3(1, 1, 1);
	float lightPower = 50.0f;

	vec3 materialDiffuseColor = texture(diffuseTextureSampler, UV).rgb;
	vec3 materialAmbientColor = vec3(0.1, 0.1, 0.1) * materialDiffuseColor;
	vec3 materialSpecularColor = texture(specularTextureSampler, UV).rgb * 0.3;

    vec3 textureNormalTangentSpace = normalize(texture(normalTextureSampler, 
        vec2(UV.x,-UV.y)).rgb * 2.0 - 1.0);

	float distance = length(lightPositionWorldSpace - positionWorldSpace);
	vec3 n = textureNormalTangentSpace; 
	vec3 l = normalize(lightDirectionTangentSpace);

	float cosTheta = clamp( dot(n, l), 0, 1);

	vec3 E = normalize(eyeDirectionTangentSpace);

	vec3 R = reflect(-l, n);

	float cosAlpha = clamp( dot(E, R), 0, 1);
	
	color = 
		materialAmbientColor + 
		materialDiffuseColor * lightColor * lightPower * cosTheta / (distance * distance) + 
		materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5) / (distance * distance);
}
