#ifndef __INCLUDED_VERTEXARRAY_HPP__
#define __INCLUDED_VERTEXARRAY_HPP__

#include "abstractarray.hpp"
#include <boost/static_assert.hpp>
#include <GL/gl.h>
#include "gl_type_constants.hpp"

template <typename CoordType, std::size_t CoordinateCount>
class VertexArray : public AbstractArray<CoordType, CoordinateCount>
{
    public:
        typedef typename AbstractArray<CoordType, CoordinateCount>::value_type value_type;

    protected:
        virtual void glPassPointer(value_type const * const data) const
        {
            // Vertices can only have coordinates in 2, 3 or 4 dimensions
            BOOST_STATIC_ASSERT(CoordinateCount == 2 || CoordinateCount == 3 || CoordinateCount == 4);

            // This is required to make sure that OpenGL can actually work with our data
            BOOST_STATIC_ASSERT(sizeof(value_type) == sizeof(CoordType[CoordinateCount]));

            // Pass all of our vertices as a Vertex Array
            glVertexPointer(CoordinateCount, OpenGLTypeConstant<CoordType>::constant, 0, data);
        }
};

#endif // __INCLUDED_VERTEXARRAY_HPP__
