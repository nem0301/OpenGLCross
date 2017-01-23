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

void computeTangentsBasis(
	// in
	vector<vec3> &vertices, 
	vector<vec2> &uvs,
	vector<vec3> &normals,
	// out
	vector<vec3> &tangents,
	vector<vec3> &bitangents)
{
	for (unsigned int i = 0; i < vertices.size(); i+=3)
	{
		vec3 &v0 = vertices[i+0];
		vec3 &v1 = vertices[i+1];
		vec3 &v2 = vertices[i+2];

		vec2 &uv0 = uvs[i+0];
		vec2 &uv1 = uvs[i+1];
		vec2 &uv2 = uvs[i+2];

		vec3 deltaPos1 = v1 - v0;
		vec3 deltaPos2 = v2 - v0;

		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}

	for (unsigned int i = 0; i < vertices.size(); i+=1)
	{
		vec3 &n = normals[i];
		vec3 &t = tangents[i];
		vec3 &b = bitangents[i];

		t = normalize(t - n * dot(n, t));

		if ( dot(cross(n, t), b) < 0.0f)
		{
			t = t * -1.0f;
		}
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

void indexVBOTBN(
	vector<vec3> &inVertices,
	vector<vec2> &inUvs,
	vector<vec3> &inNormals,
	vector<vec3> &inTangents,
	vector<vec3> &inBitangents,
	vector<unsigned short> &outIndices,
	vector<vec3> &outVertices,
	vector<vec2> &outUvs,
	vector<vec3> &outNormals,
	vector<vec3> &outTangents,
	vector<vec3> &outBitangents
	)
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

			outTangents[index] += inTangents[i];
			outBitangents[index] += inBitangents[i];
		}
		else
		{
			outVertices.push_back(inVertices[i]);
			outUvs.push_back(inUvs[i]);
			outNormals.push_back(inNormals[i]);

			outTangents.push_back(inTangents[i]);
			outBitangents.push_back(inBitangents[i]);

			unsigned short newIndex = (unsigned short)outVertices.size() - 1;
			outIndices.push_back(newIndex);
			vertexToOutIndex[packed] = newIndex;
		}
	}
}

void loadOBJ(string path,
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
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		vec3 vertex = tmpVertices[vertexIndex-1];
		vec2 uv = tmpUvs[uvIndex-1];
		vec3 normal = tmpNormals[normalIndex-1];

		outVertices.push_back(vertex);
		outUvs.push_back(uv);
		outNormals.push_back(normal);
	}
}

