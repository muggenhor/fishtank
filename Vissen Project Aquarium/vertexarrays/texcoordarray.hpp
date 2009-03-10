#ifndef __INCLUDED_TEXCOORDARRAY_HPP__
#define __INCLUDED_TEXCOORDARRAY_HPP__

#include "abstractarray.hpp"
#include <boost/static_assert.hpp>
#include <GL/gl.h>
#include "gl_type_constants.hpp"

template <typename CoordType, std::size_t CoordinateCount>
class TexCoordArray : public AbstractArray<CoordType, CoordinateCount>
{
    public:
        typedef AbstractArray<CoordType, CoordinateCount>   parent_type;
        typedef typename parent_type::value_type            value_type;

        virtual void draw() const
        {
            // Textures can only have 1, 2, 3 or 4 dimensions to specify coordinates in
            BOOST_STATIC_ASSERT(CoordinateCount == 1 || CoordinateCount == 2 || CoordinateCount == 3 || CoordinateCount == 4);

            // This is required to make sure that OpenGL can actually work with our data
            BOOST_STATIC_ASSERT(sizeof(value_type) == (sizeof(CoordType[CoordinateCount])));

            // Pass all of our texture coordinates as a Texture Coordinates Array
            glTexCoordPointer(CoordinateCount, OpenGLTypeConstant<CoordType>::constant, 0, parent_type::get_data());
        }
};

#endif // __INCLUDED_TEXCOORDARRAY_HPP__
