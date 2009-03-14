#include "JPEG.h"
#include <iostream>
#include "GL/GLee.h"
#include <GL/glu.h>
#include <boost/scoped_array.hpp>
#include <cassert>
#include "glexcept.hpp"

#ifdef WIN32
extern "C"
{
# include "include/jpeglib.h"
}
#else
# include <jpeglib.h>
#endif

Image::Image(const unsigned int num_components, const unsigned int width_, const unsigned int height_) :
	data(boost::extents[width_][height_], boost::fortran_storage_order())
{
	assert(num_components == 3);
}

Image Image::LoadJPG(const char* const filename, bool flipY)
{
	FILE* const file = fopen(filename, "rb");
	if (file == NULL)
	{
		/// @TODO throw exception instead
		std::cerr << "Unable to load JPG File '" << filename << "'" << std::endl;
		exit(EXIT_FAILURE);
	}

	try
	{
		// Create a JPEG decompressor
		struct jpeg_decompress_struct cinfo;
		jpeg_error_mgr jerr;
		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);

		// Decompress the file
		jpeg_stdio_src(&cinfo, file);
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		Image img(cinfo.num_components, cinfo.image_width, cinfo.image_height);

		boost::scoped_array<unsigned char*> rowPtr(new unsigned char*[img.height()]);
		for (unsigned int i = 0; i < img.height(); ++i)
		{
			const unsigned int rowIndex = flipY
			                             ? img.height() - 1 - i
			                             : i;

			rowPtr[i] = img.data[0][rowIndex].data();
		}

		int rowsRead = 0;
		while (cinfo.output_scanline < cinfo.output_height)
		{
			rowsRead += jpeg_read_scanlines(&cinfo, &rowPtr[rowsRead], cinfo.output_height-rowsRead);
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		fclose(file);

		return img;
	}
	catch (...)
	{
		fclose(file);
		throw;
	}
}

Texture::Texture(const Image& img) :
	_img(new Image(img))
{
	// Create a texture ID
	glGenTextures(1, &_texture);

	upload_texture();
}

Texture::~Texture()
{
	if (_img)
	{
		glDeleteTextures(1, &_texture);
	}
}

Texture::Texture(const Texture& rhs) :
	_img(rhs._img ? new Image(*rhs._img) : NULL)
{
	if (_img)
	{
		// Create a texture ID
		glGenTextures(1, &_texture);

		upload_texture();
	}
}

Texture& Texture::operator=(const Texture& rhs)
{
	if (rhs._img)
	{
		if (!_img)
			glGenTextures(1, &_texture);
		_img.reset(new Image(*rhs._img));

		upload_texture();
	}
	else
	{
		if (_img)
			glDeleteTextures(1, &_texture);
		_img.reset();
	}

	return *this;
}

void Texture::bind() const
{
	if (is_null_texture())
	{
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _texture);
	}
}

void Texture::upload_texture() const
{
	if (!_img)
		return;

	if (!GLEE_EXT_texture_object)
		throw OpenGL::missing_capabilities("The GL_EXT_texture_object extension is required to find out what the current texture binding is.");

	GLint previous_texture;
	glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &previous_texture);

	// Upload the texture
	glBindTexture(GL_TEXTURE_2D, _texture);
	try
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, _img->width(), _img->height(), GL_RGB, GL_UNSIGNED_BYTE, _img->data.origin()->data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glBindTexture(GL_TEXTURE_2D, previous_texture);
	}
	catch (...)
	{
		glBindTexture(GL_TEXTURE_2D, previous_texture);
		throw;
	}
}
