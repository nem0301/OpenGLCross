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

	GLuint loadBmp(string path)
	{
		const char* cPath = path.c_str();
		unsigned int header[54];
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

		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x0A]);
		width = *(int*)&(header[0x0A]);
		height = *(int*)&(header[0x0A]);

		if ( imageSize == 0 )
		{
			imageSize = width * height * 3;
		}

		if ( dataPos == 0 )
		{
			dataPos = 54;
		}

		data = new unsigned char [imageSize];

		fread(data, 1, imageSize);

		fclose(fp);
	}
};
