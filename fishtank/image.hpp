#ifndef __INCLUDED_IMAGE_HPP__
#define __INCLUDED_IMAGE_HPP__

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
#include <boost/gil/extension/io/png_io.hpp>

template <typename Image>
inline void read_image(const char* filename, Image& im)
{
	// Try filetypes until one is succesful (i.e. doesn't throw an error).

	try
	{
		return boost::gil::jpeg_read_image(filename, im);
	}
	catch (std::ios_base::failure& e)
	{
		return boost::gil::png_read_image(filename, im);
	}
}

template <typename Image>
inline void read_image(const std::string& filename, Image& im)
{
	return read_image(filename.c_str(), im);
}

#endif // __INCLUDED_IMAGE_HPP__
