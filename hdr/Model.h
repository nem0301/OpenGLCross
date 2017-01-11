#include "Common.h"
#include "GLCommon.h"

struct PackedVertex
{
	vec3 postion;
	vec2 uv;
	vec3 normal;
	bool operator<(const struct PackedVertex that) const {
		return memcmp((void*)this, (void*)&that, sizeof(struct PackedVertex)) > 0;
	};
};

class Model
{
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	vector<unsigned short> indices;

	mat4* model;
	mat4* view;
	mat4* proj;

	vec3 lightPos;

	GLuint vertexBufferID;
	GLuint uvBufferID;
	GLuint normalBufferID;
	GLuint elementBufferID;

	vec3 position;
	
	GLuint textureID;
	GLuint texture;

	GLuint shaderID;

	GLuint uniformMvpMatrixID;
	GLuint uniformViewMatrixID;
	GLuint uniformModelMatrixID;
	GLuint uniformLightID;
	
public:
	Model(vec3 pos, GLuint textureID, GLuint texture,
		GLuint shaderID, GLuint mvpID, GLuint viewID, GLuint modelID, GLuint lightID)
	{
		this->position = pos;
		this->textureID = textureID;
		this->texture = texture;
		this->shaderID = shaderID;
		this->uniformMvpMatrixID = mvpID;
		this->uniformViewMatrixID = viewID;
		this->uniformModelMatrixID = modelID;
		this->uniformLightID = lightID;
	}

	~Model()
	{
		clearBufferData();
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &uvBufferID);
		glDeleteBuffers(1, &normalBufferID);
	}

	void setPosition(vec3 pos)
	{
		this->position = pos;
	}

	vec3 getPosition()
	{
		return  this->position;
	}

	void setTexture(GLuint textureID, GLuint texture)
	{
		this->textureID = textureID;
		this->texture = texture;
	}

	void moveObj(vec3 delta)
	{
		this->position += delta;
	}

	void setModel(mat4* &m)
	{
		model = m;
	}

	void setView(mat4* &v)
	{
		view = v;
	}

	void setProj(mat4* &p)
	{
		proj = p;
	}
	
	void setLightPos(vec3 &lightPos)
	{
		this->lightPos = lightPos;
	}

	bool getSimilarVertexIndex(
		struct PackedVertex &packed,
		map<struct PackedVertex, unsigned short> &vertexToOutIndex,
		unsigned short &result)
	{
		map<struct PackedVertex, unsigned short>::iterator it = vertexToOutIndex.find(packed);
		if ( it == vertexToOutIndex.end())
		{
			return false;
		}
		else
		{
			result = it->second;
			return true;
		}
	}

	void indexVBO(
		vector<vec3> &inVertices,
		vector<vec2> &inUvs,
		vector<vec3> &inNormals,
		vector<unsigned short> &outIndices,
		vector<vec3> &outVertices,
		vector<vec2> &outUvs,
		vector<vec3> &outNormals)
	{
		map<struct PackedVertex, unsigned short> vertexToOutIndex;

		for (unsigned int i = 0; i < inVertices.size(); i++)
		{
			struct PackedVertex packed =  {
				inVertices[i],
				inUvs[i], 
				inNormals[i]
			};

			unsigned short index;;
			bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);

			if (found)
			{
				outIndices.push_back(index);
			}
			else
			{
				outVertices.push_back(inVertices[i]);
				outUvs.push_back(inUvs[i]);
				outNormals.push_back(inNormals[i]);

				unsigned short newIndex = (unsigned short)outVertices.size() - 1;
				outIndices.push_back(newIndex);
				vertexToOutIndex[packed] = newIndex;
			}
		}
	}

	void loadObj(string path) 
//			vector<vec3>& outVertices,
//			vector<vec2>& outUvs,
//			vector<vec3>& outNormals)
	{
		vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		vector<vec3> tmpVertices;
		vector<vec2> tmpUvs;
		vector<vec3> tmpNormals;

		FILE* fp = fopen(path.c_str(), "r");
		if ( fp == NULL )
		{
			perror("fopen : ");
			return;
		}
		
		while ( true )
		{
			char lineHeader[128];
			int res = fscanf(fp, "%s", lineHeader);

			if ( res == EOF )
				break;

			if ( strcmp(lineHeader, "v") == 0 )
			{
				vec3 vertex;
				fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				tmpVertices.push_back(vertex);
			}
			else if ( strcmp(lineHeader, "vt") == 0 )
			{
				vec2 uv;
				fscanf(fp, "%f %f\n", &uv.x, &uv.y);
				tmpUvs.push_back(uv);
			}
			else if ( strcmp(lineHeader, "vn") == 0 )
			{
				vec3 normal;
				fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				tmpNormals.push_back(normal);
			}
			else if ( strcmp(lineHeader, "f") == 0 )
			{
				string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(fp , "%d/%d/%d %d/%d/%d %d/%d/%d\n",
						&vertexIndex[0], &uvIndex[0], &normalIndex[0],
						&vertexIndex[1], &uvIndex[1], &normalIndex[1],
						&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				
				if ( matches != 9 )
				{
					perror("f parsing");
					printf("%d\n", matches);
					return;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		clearBufferData();

		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			unsigned int vertexIndex = vertexIndices[i];
			vec3 vertex = tmpVertices[vertexIndex-1];
			vertex = vertex;
			vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < uvIndices.size(); i++)
		{
			unsigned int uvIndex = uvIndices[i];
			vec2 uv = tmpUvs[uvIndex-1];
			uvs.push_back(uv);
		}
		for (unsigned int i = 0; i < normalIndices.size(); i++)
		{
			unsigned int normalIndex = normalIndices[i];
			vec3 normal = tmpNormals[normalIndex-1];
			normals.push_back(normal);
		}
		
		tmpVertices.clear();
		tmpUvs.clear();
		tmpNormals.clear();
		tmpVertices = vertices;
		tmpUvs = uvs;
		tmpNormals = normals;
		
		
		indexVBO(
			tmpVertices,
			tmpUvs,
			tmpNormals,
			indices,
			vertices,
			uvs,
			normals);

			
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3),
				&vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &uvBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2),
				&uvs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3),
				&normals[0], GL_STATIC_DRAW);
	
		glGenBuffers(1, &elementBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				&indices[0], GL_STATIC_DRAW);
	}


	void drawObj()
	{
		glUseProgram(shaderID);
		*model = translate(*model, position);
		mat4 mvp = *proj * *view * *model;
		mat4 view = *this->view;
		mat4 model = *this->model;

		// TODO : modify mvp for setting position of obj
		
		
		glUniformMatrix4fv(uniformMvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(uniformViewMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(uniformModelMatrixID, 1, GL_FALSE, &model[0][0]);

		glUniform3f(uniformLightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(this->textureID, 0); 

		// vertex buffer
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(
			0,          // attribute 0. vertex
			3,          // size
			GL_FLOAT,   // type
			GL_FALSE,   // nomarlized?
			0,          // stride
			(void*)0    // array buffer offset
		);  

		// uv buffer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(
			1,          // attribute 1. uv
			2,          // size
			GL_FLOAT,   // type
			GL_FALSE,   // nomarlized?
			0,          // stride
			(void*)0    // array buffer offset
		);  

		// normal buffer
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glVertexAttribPointer(
			2,          // attribute 2 normal
			3,          // size
			GL_FLOAT,   // type
			GL_FALSE,   // nomarlized?
			0,          // stride
			(void*)0    // array buffer offset
		);  

		// draw elements
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
		glDrawElements(
			GL_TRIANGLES,
			indices.size(),
			GL_UNSIGNED_SHORT,
			(void*)0
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}


	void clearBufferData()
	{
		vertices.clear();
		uvs.clear();
		normals.clear();
	}

};
