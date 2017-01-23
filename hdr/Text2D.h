#pragma once
#include "Common.h"
#include "Shader.h"
#include "Texture.h"

class Text2D
{
	GLuint textureID;
	GLuint vertexBufferID;
	GLuint uvBufferID;
	GLuint shaderID;
	GLuint uniformID;
public:
	Text2D()
	{
	}

	~Text2D()
	{
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &uvBufferID);

		glDeleteTextures(1, &textureID);

		glDeleteProgram(shaderID);
	}

	void initText2D(
		const char* texturePath,
		const char* vertexShader,
		const char* fragmentShader)
	{
		textureID = loadDDS(texturePath);
		glGenBuffers(1, &vertexBufferID);
		glGenBuffers(1, &uvBufferID);

		shaderID = loadShaders(vertexShader, fragmentShader);
		uniformID = glGetUniformLocation(shaderID, "myTextureSampler");
	}

	void printText(string text, int x, int y, int size)
	{
		unsigned int length = text.length();

		vector<vec2> vertices;
		vector<vec2> uvs;

		for (unsigned int i = 0; i < length; i++)
		{
			vec2 vertexUpLeft		= vec2(x + i * size, y + size);
			vec2 vertexUpRight		= vec2(x + i * size + size, y + size);
			vec2 vertexDownRight	= vec2(x + i * size + size, y);
			vec2 vertexDownLeft		= vec2(x + i * size, y);

			vertices.push_back(vertexUpLeft);
			vertices.push_back(vertexDownLeft);
			vertices.push_back(vertexUpRight);

			vertices.push_back(vertexDownRight);
			vertices.push_back(vertexUpRight);
			vertices.push_back(vertexDownLeft);

			char character = text[i];
			float uvX = (character%16) / 16.0f;
			float uvY = (character/16) / 16.0f;

			vec2 uvUpLeft		= vec2(uvX, uvY);
			vec2 uvUpRight		= vec2(uvX + 1.0f / 16.0f, uvY);
			vec2 uvDownRight	= vec2(uvX + 1.0f / 16.0f, uvY + 1.0f / 16.0f);
			vec2 uvDownLeft		= vec2(uvX, uvY + 1.0f / 16.0f);

			uvs.push_back(uvUpLeft);
			uvs.push_back(uvDownLeft);
			uvs.push_back(uvUpRight);

			uvs.push_back(uvDownRight);
			uvs.push_back(uvUpRight);
			uvs.push_back(uvDownLeft);

		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2),
			&vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2),
			&uvs[0], GL_STATIC_DRAW);

		glUseProgram(shaderID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glUniform1i(uniformID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisable(GL_BLEND);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
};

