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

#ifndef __INCLUDED_NORMALARRAY_HPP__
#define __INCLUDED_NORMALARRAY_HPP__

#include "abstractarray.hpp"
#include <boost/serialization/base_object.hpp>
#include <GL/gl.h>
#include "gl_type_constants.hpp"

template <typename CoordType, bool supportVBO = true>
class NormalArray : public AbstractArray<CoordType, 3, supportVBO, NormalArray<CoordType, supportVBO> >
{
    public:
        typedef typename AbstractArray<CoordType, 3, supportVBO, NormalArray<CoordType, supportVBO> >::value_type value_type;
        typedef typename AbstractArray<CoordType, 3, supportVBO, NormalArray<CoordType, supportVBO> >::transform_type transform_type;

    private:
	friend class AbstractArray<CoordType, 3, supportVBO,
	       NormalArray<CoordType, supportVBO> >;

        void glPassPointer(value_type const * const data) const
        {
            // Pass all of our texture coordinates as a Texture Coordinates Array
            glNormalPointer(OpenGLTypeConstant<CoordType>::constant, 0, data);
        }

        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & boost::serialization::base_object< AbstractArray<CoordType, 3, supportVBO, NormalArray<CoordType, supportVBO> > >(*this);
        }
};

#endif // __INCLUDED_NORMALARRAY_HPP__
