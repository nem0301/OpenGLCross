#include "Common.h" 
using namespace std;

class Texture
{
public:
	Texture()
	{
	}

	~Texture()
	{
	}

	static GLuint loadBmp(string path)
	{
		const char* cPath = path.c_str();
		unsigned char header[54];
		unsigned int dataPos;
		unsigned int width, height;
		unsigned int imageSize;
		unsigned char *data;

		FILE* fp = fopen(cPath, "rb");
		if ( !fp ) 
		{
			cout << "image could not opened" << endl;
			return -1;
		}

		if ( fread(header, 1, 54, fp) != 54)
		{
			cout << "not correct BMP file" << endl;
			return -2;
		}

		if ( header[0] != 'B' || header[1] != 'M' )
		{
			cout << "not correct BMP file " << header[0] << ", " <<  header[1] <<endl;
		}


		// Make sure this is a 24bpp file
		if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    return 0;}
		if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    return 0;}


		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);

		if ( imageSize == 0 )
		{
			imageSize = width * height * 3;
		}

		if ( dataPos == 0 )
		{
			dataPos = 54;
		}

		data = new unsigned char [imageSize];

		fread(data, 1, imageSize, fp);

		fclose(fp);

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

		delete [] data;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		return textureID;
	}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
	GLuint loadDdss(string path)
	{
		const char* cPath = path.c_str();
		unsigned char header[124];
		FILE* fp;

		fp = fopen(cPath, "rb");
		if ( !fp ) 
		{
			cout << "image could not opened" << endl;
			return -1;
		}

		char fileCode[4];
		fread(fileCode, 1, 4, fp);
		if ( strncmp(fileCode, "DDS ", 4) != 0 )
		{
			fclose(fp);
			return -2;
		}

		fread(&header, 124, 1, fp);

		unsigned int height = *(unsigned int*)&(header[8]);
		unsigned int width = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);

		unsigned char* buffer;
		unsigned int bufSize;

		bufSize = mipMapCount > 1 ? linearSize * 2 : linearSize;
		buffer = (unsigned char*) malloc (bufSize * sizeof(unsigned char));
		fread(buffer, 1, bufSize, fp);
		fclose(fp);

		unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
		unsigned int format;

		switch (fourCC)
		{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		}

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width+3)/4) * ((height+3)/4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
					0, size, buffer + offset);
			offset += size;
			width /= 2;
			height /= 2;
		}
		free(buffer);
		return textureID;

	}


};
