#ifndef JPEG_H
#define JPEG_H

#include <string.h>
#include <stdio.h>

#include <string>


// This structure below was added for GameTutorials.com purposes.  This is in
// no way connected to the jpeg library and can be deleted or changed.

// This stores the important jpeg data
struct tImageJPG
{
	int rowSpan;
	int sizeX;
	int sizeY;
	unsigned char *data;
};

tImageJPG *LoadJPG(const char *filename, bool flipY=false);

typedef unsigned int UINT;

/// Note: frees image data.
void JPEG_Texture(UINT textureArray[], tImageJPG *pImageData, int textureID);
void JPEG_Texture(UINT textureArray[], const std::string &strFileName, int textureID);




#endif
