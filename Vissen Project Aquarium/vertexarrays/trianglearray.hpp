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

#ifndef __INCLUDED_TRIANGLEARRAY_HPP__
#define __INCLUDED_TRIANGLEARRAY_HPP__

#include <vector>
#include <boost/static_assert.hpp>
#include <eigen/vector.h>
#include <GLee.h>
#include "gl_type_constants.hpp"
#include "vertexarray.hpp"
#include "texcoordarray.hpp"

template <typename IndexIntegerType, typename VertexCoordType, typename TexCoordType, std::size_t VertexCoordinateCount = 3, std::size_t TexCoordCount = 2>
class TriangleArray
{
    public:
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount>::value_type vertex_type;
        typedef typename TexCoordArray<TexCoordType, TexCoordCount>::value_type texcoord_type;

        void draw() const
        {
            // Bail out if there's nothing to draw
            if (_indices.empty())
                return;

            // Pass all of our triangle data
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            assert(_VertexArray.size() == _TexCoordArray.size());

            // Pass the VertexArray
            _VertexArray.draw();
            _TexCoordArray.draw();

#ifndef NDEBUG
            for (typename std::vector<IndexIntegerType>::const_iterator i = _indices.begin(); i != _indices.end(); ++i)
                assert(*i < _VertexArray.size());
#endif

            // Draw all contained triangles based on the array of indices
            if (GLEE_VERSION_1_2)
                glDrawRangeElements(GL_TRIANGLES, 0, _VertexArray.size() - 1, _indices.size(), OpenGLTypeConstant<IndexIntegerType>::constant, &_indices[0]);
            else if (GLEE_EXT_draw_range_elements)
                glDrawRangeElementsEXT(GL_TRIANGLES, 0, _VertexArray.size() - 1, _indices.size(), OpenGLTypeConstant<IndexIntegerType>::constant, &_indices[0]);
            else
                glDrawElements(GL_TRIANGLES, _indices.size(), OpenGLTypeConstant<IndexIntegerType>::constant, &_indices[0]);

            // Disable the GL_VERTEX_ARRAY client state to prevent strange behaviour
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        std::size_t uniqueVertices() const
        {
            return _VertexArray.size();
        }

        void AddTriangle(const vertex_type* v, const texcoord_type* t)
        {
            for (unsigned int i = 0; i < 3; ++i)
                AddPoint(v[i], t[i]);
        }

        void AddTriangle(const vertex_type* v)
        {
            static texcoord_type t;
            for (unsigned int i = 0; i < static_cast<unsigned int>(t.size()); ++i)
                t[i] = 0;

            for (unsigned int i = 0; i < 3; ++i)
                AddPoint(v[i], t);
        }

    private:
        void AddPoint(const vertex_type& vertex, const texcoord_type& texcoord)
        {
            unsigned int index = 0;

            // Try to find an instance of the given vertex
            for (index = 0; index < _VertexArray.size(); ++index)
            {
                if (_VertexArray[index]   == vertex
                 && _TexCoordArray[index] == texcoord)
                {
                    // If we found a vertex that's the same then add the index
                    // to the index table and bail out
                    _indices.push_back(index);

                    return;
                }
            }

            // If we found no instance of this vertex then we'll obviously need to add it
            _VertexArray.push_back(vertex);
            _TexCoordArray.push_back(texcoord);
            _indices.push_back(index);
        }

    private:
        VertexArray<VertexCoordType, VertexCoordinateCount> _VertexArray;
        TexCoordArray<TexCoordType, TexCoordCount> _TexCoordArray;
        std::vector<IndexIntegerType> _indices;
};

#endif // __INCLUDED_IMD_HPP__
