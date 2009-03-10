/*
 *  Vertex Array abstraction for OpenGL
 *  Copyright (C) 2007-2008  Giel van Schijndel
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
