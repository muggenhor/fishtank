#include "textures.hpp"
#include "GL/GLee.h"
#include <GL/glu.h>
#include "glexcept.hpp"

Texture::Texture() :
	_img(0)
{
}

Texture::~Texture()
{
	clear();
}

Texture::Texture(const Texture& rhs) :
	_img(rhs._img ? new boost::gil::rgb8_image_t(*rhs._img) : 0)
{
	if (_img)
	{
		gen_texture();
		upload_texture();
	}
}

Texture& Texture::operator=(const Texture& rhs)
{
	if (rhs._img)
	{
		*this = const_view(*rhs._img);
	}
	else
	{
		clear();
	}

	return *this;
}

void Texture::clear()
{
	if (_img)
	{
		delete _img;
		glDeleteTextures(1, &_texture);
		_img = 0;
	}
}

bool Texture::empty() const
{
	return !_img;
}

void Texture::bind() const
{
	if (_img)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _texture);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void Texture::gen_texture()
{
	glGenTextures(1, &_texture);
}

void Texture::upload_texture() const
{
	if (!GLEE_EXT_texture_object)
		throw OpenGL::missing_capabilities("The GL_EXT_texture_object extension is required to find out what the current texture binding is.");

	GLint previous_texture;
	glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &previous_texture);

	// Upload the texture
	glBindTexture(GL_TEXTURE_2D, _texture);
	try
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, _img->width(), _img->height(), GL_RGB, GL_UNSIGNED_BYTE, &const_view(*_img)(0, 0)[0]);

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
