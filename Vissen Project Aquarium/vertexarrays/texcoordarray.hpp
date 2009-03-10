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

#ifndef __INCLUDED_TEXCOORDARRAY_HPP__
#define __INCLUDED_TEXCOORDARRAY_HPP__

#include "abstractarray.hpp"
#include <boost/static_assert.hpp>
#include <GL/gl.h>
#include "gl_type_constants.hpp"

template <typename CoordType, std::size_t CoordinateCount, bool supportVBO = true>
class TexCoordArray : public AbstractArray<CoordType, CoordinateCount, supportVBO>
{
    public:
        typedef typename AbstractArray<CoordType, CoordinateCount, supportVBO>::value_type value_type;
        typedef typename AbstractArray<CoordType, CoordinateCount, supportVBO>::matrix_type matrix_type;

    protected:
        virtual void glPassPointer(value_type const * const data) const
        {
            // Textures can only have 1, 2, 3 or 4 dimensions to specify coordinates in
            BOOST_STATIC_ASSERT(CoordinateCount == 1 || CoordinateCount == 2 || CoordinateCount == 3 || CoordinateCount == 4);

            // Pass all of our texture coordinates as a Texture Coordinates Array
            glTexCoordPointer(CoordinateCount, OpenGLTypeConstant<CoordType>::constant, 0, data);
        }
};

#endif // __INCLUDED_TEXCOORDARRAY_HPP__
