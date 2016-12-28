#include "Common.h"
#include "GLCommon.h"

class Model
{
public:
	Model()
	{
	}

	void loadObj (string path, 
			vector<vec3>& outVertices,
			vector<vec2>& outUvs,
			vector<vec3>& outNormals)
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

		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			unsigned int vertexIndex = vertexIndices[i];
			vec3 vertex = tmpVertices[vertexIndex-1];
			outVertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < uvIndices.size(); i++)
		{
			unsigned int uvIndex = uvIndices[i];
			vec2 uv = tmpUvs[uvIndex-1];
			outUvs.push_back(uv);
		}
		for (unsigned int i = 0; i < normalIndices.size(); i++)
		{
			unsigned int normalIndex = normalIndices[i];
			vec3 normal = tmpNormals[normalIndex-1];
			outNormals.push_back(normal);
		}
	}

};
