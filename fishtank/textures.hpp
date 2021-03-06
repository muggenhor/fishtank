#ifndef __INCLUDED_TEXTURES_HPP__
#define __INCLUDED_TEXTURES_HPP__

#include "GL/GLee.h"
#include <boost/gil/algorithm.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/serialization/split_member.hpp>

class Texture
{
	public:
		Texture();

		template <typename View>
		Texture(const View& img) :
			_img(new boost::gil::rgb8_image_t(img.dimensions()))
		{
			boost::gil::copy_and_convert_pixels(img, view(*_img));
			gen_texture();
			upload_texture();
		}

		~Texture();

		template <typename View>
		Texture& operator=(const View& rhs)
		{
			if (!_img
			 || _img->dimensions() != rhs.dimensions())
			{
				clear();
				_img = new boost::gil::rgb8_image_t(rhs.dimensions());
				gen_texture();
			}

			boost::gil::copy_and_convert_pixels(rhs, view(*_img));
			upload_texture();

			return *this;
		}

		Texture(const Texture& rhs);
		Texture& operator=(const Texture& rhs);

		void clear();
		bool empty() const;

		void bind() const;

	private:
		void gen_texture();

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
			ar & _img;
			upload_texture();
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER()

	private:
		boost::gil::rgb8_image_t*       _img;
		GLuint                          _texture;

		friend class Camera;
};

#endif // __INCLUDED_TEXTURES_HPP__
