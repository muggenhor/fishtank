#include "JPEG.h"
#include "Main.h"
#include <iostream>

#ifdef WIN32
extern "C"
{
# include "include/jpeglib.h"
}
#else
# include <jpeglib.h>
#endif

void DecodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData, bool flipY)
{
	jpeg_read_header(cinfo, TRUE);

	jpeg_start_decompress(cinfo);

	pImageData->rowSpan = cinfo->image_width * cinfo->num_components;
	pImageData->sizeX   = cinfo->image_width;
	pImageData->sizeY   = cinfo->image_height;

	pImageData->data = new unsigned char[pImageData->rowSpan * pImageData->sizeY];

	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];
	if(flipY){
		for (int i = 0; i < pImageData->sizeY; i++){
			rowPtr[i] = &(pImageData->data[(pImageData->sizeY-i-1)*pImageData->rowSpan]);
		}
	}else{
		for (int i = 0; i < pImageData->sizeY; i++){
			rowPtr[i] = &(pImageData->data[i*pImageData->rowSpan]);
		}
	}
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height)
	{
		rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], cinfo->output_height-rowsRead);
	}

	delete [] rowPtr;

	jpeg_finish_decompress(cinfo);
}

tImageJPG *LoadJPG(const char *filename, bool flipY)
{
	struct jpeg_decompress_struct cinfo;

	FILE* const file = fopen(filename, "rb");
	if (file == NULL)
	{
		//MessageBox(hWnd, "Unable to load JPG File!", "Error", MB_OK);
		std::cerr << "Unable to load JPG File '" << filename << "'" << std::endl;
		return NULL;
	}

	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, file);

	tImageJPG* const pImageData = (tImageJPG*)malloc(sizeof(*pImageData));

	DecodeJPG(&cinfo, pImageData, flipY);

	jpeg_destroy_decompress(&cinfo);

	fclose(file);

	return pImageData;
}

void JPEG_Texture(UINT textureArray[], tImageJPG *pImage, int textureID)
{
	if (pImage == NULL)
		exit(EXIT_FAILURE);

	glGenTextures(1, &textureArray[textureID]);
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	if (pImage)
	{
		free(pImage->data);
	}

	free(pImage);
}

void JPEG_Texture(UINT textureArray[], const std::string &strFileName, int textureID)
{
	if(strFileName.empty())return;

	tImageJPG *pImage = LoadJPG(strFileName.c_str(), true);

	JPEG_Texture(textureArray, pImage, textureID);
}
