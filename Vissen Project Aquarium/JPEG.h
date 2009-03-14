#ifndef JPEG_H
#define JPEG_H

#include "GL/GLee.h"
#include <boost/multi_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/serialization/scoped_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <Eigen/Core>

namespace Eigen
{
	template<> struct NumTraits<unsigned char>
	{
		typedef unsigned char Real;
		typedef float FloatingPoint;
		enum {
			IsComplex = 0,
			HasFloatingPoint = 0,
			ReadCost = 1,
			AddCost = 1,
			MulCost = 1
		};
	};
}

class Image
{
	public:
		static Image LoadJPG(const char* filename, bool flipY = false);

		inline unsigned int width() const  { return data.shape()[0]; }
		inline unsigned int height() const { return data.shape()[1]; };

	private:
		Image() {}
		Image(unsigned int num_components, unsigned int width_, unsigned int height_);

		friend class boost::serialization::access;

		template <class Archive>
		void serialize(Archive & ar, const unsigned int /* version */)
		{
			ar & data;
		}

	public:
		boost::multi_array<Eigen::Matrix<unsigned char, 3, 1>, 2> data;
};

class Texture
{
	public:
		Texture() {}
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
