#include "JPEG.h"
#include <iostream>
#include "GL/GLee.h"
#include <GL/glu.h>
#include <boost/gil/gil_all.hpp>
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

/* Needs to be included after jpeglib.h to ensure that the platform workarounds
 * in "include/jpeglib.h" will be used before <jpeglib.h> gets included by
 * jpeg_io.h.
 */
#include <boost/gil/extension/io/jpeg_io.hpp>

Image::Image(const unsigned int num_components, const unsigned int width_, const unsigned int height_) :
	data(boost::extents[width_][height_], boost::fortran_storage_order())
{
	assert(num_components == 3);
}

Image Image::LoadJPG(const char* const filename, FLIP_IMAGE_PIXELS flip)
{
	using namespace boost::gil;

	// Allocate image memory
	const point2<std::ptrdiff_t> dimensions = jpeg_read_dimensions(filename);
	Image img(3, dimensions.x, dimensions.y);

	// Load image
	rgb8_view_t imgView = interleaved_view(img.width(), img.height(), (rgb8_pixel_t*)img.data.origin(), img.width() * 3);
	if (flip == FLIP_Y)
		jpeg_read_view(filename, flipped_up_down_view(imgView));
	else
		jpeg_read_view(filename, imgView);

	return img;
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
