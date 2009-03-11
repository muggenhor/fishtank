#ifndef JPEG_H
#define JPEG_H

#include "GL/GLee.h"
#include <boost/scoped_ptr.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <vector>

class Texture;

class Image
{
	public:
		static Image LoadJPG(const char* filename, bool flipY = false);

	private:
		Image() {}
		Image(unsigned int rowSpan_, unsigned int width_, unsigned int height_);

		friend class boost::serialization::access;

		template <class Archive>
		void serialize(Archive & ar, const unsigned int /* version */)
		{
			ar & rowSpan;
			ar & width;
			ar & height;
			ar & data;
		}

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

		friend class boost::serialization::access;

		template <class Archive>
		void save(Archive & ar, const unsigned int /* version */) const
		{
			ar & _img;
		}

		template <class Archive>
		void load(Archive & ar, const unsigned int /* version */)
		{
			boost::scoped_ptr<Image> img;
			ar & img;

			*this = Texture(*img);
		}

                BOOST_SERIALIZATION_SPLIT_MEMBER()


	private:
		boost::scoped_ptr<Image>        _img;
		GLuint                          _texture;
};

#endif
