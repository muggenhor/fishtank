#ifndef JPEG_H
#define JPEG_H

#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>

class Texture;

class Image
{
	public:
		static Image LoadJPG(const char* filename, bool flipY = false);

	private:
		Image(unsigned int rowSpan_, unsigned int width_, unsigned int height_);

	public:
		unsigned int rowSpan;
		unsigned int width, height;
		std::vector<unsigned char> data;
};

class Texture
{
	public:
		Texture();
		Texture(const Image& img);
		~Texture();

		Texture(const Texture& rhs);
		Texture& operator=(const Texture& rhs);

		void bind() const;
		bool is_null_texture() const { return !_img; }

	private:
		void upload_texture() const;

	private:
		Image*  _img;
		GLuint  _texture;
};

#endif
