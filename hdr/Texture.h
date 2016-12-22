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
};
