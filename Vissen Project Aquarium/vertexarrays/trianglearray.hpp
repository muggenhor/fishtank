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
#include "../GL/GLee.h"
#include "gl_type_constants.hpp"
#include "normalarray.hpp"
#include "vertexarray.hpp"
#include "texcoordarray.hpp"
#include <cassert>

template <typename IndexIntegerType, typename VertexCoordType, typename TexCoordType, typename NormalCoordType, bool supportVertexVBOs = true, bool supportTexVBOs = true, bool supportNormalVBOs = true, std::size_t VertexCoordinateCount = 3, std::size_t TexCoordCount = 2>
class TriangleArray
{
    public:
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs>::value_type         vertex_type;
        typedef typename TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs>::value_type                     texcoord_type;
        typedef typename NormalArray<NormalCoordType, supportNormalVBOs>::value_type                                normal_type;
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs>::matrix_type        vertex_matrix_type;
        typedef typename TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs>::matrix_type                    texcoord_matrix_type;
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs>::trans_matrix_type  vertex_trans_matrix_type;
        typedef typename TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs>::trans_matrix_type              texcoord_trans_matrix_type;

        void draw() const
        {
            // Bail out if there's nothing to draw
            if (empty())
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

        bool empty() const
        {
            return _indices.empty();
        }

        void clear()
        {
            _VertexArray.clear();
            _TexCoordArray.clear();
        }

        void ModelViewLeftMult(vertex_matrix_type const& m)
        {
            _VertexArray.leftmultiply(m);

            vertex_matrix_type normalMatrix(m.inverse());
            normalMatrix.replaceWithAdjoint();
            _NormalArray.leftmultiply(normalMatrix);
        }

        void ModelViewLeftMult(vertex_trans_matrix_type const& m)
        {
            _VertexArray.leftmultiply(m);

            vertex_trans_matrix_type normalMatrix(m.inverse());
            normalMatrix.replaceWithAdjoint();
            _NormalArray.leftmultiply(normalMatrix);
        }

        void ModelViewMult(vertex_matrix_type const& m)
        {
            _VertexArray.multiply(m);

            vertex_matrix_type normalMatrix(m.inverse());
            normalMatrix.replaceWithAdjoint();
            _NormalArray.multiply(normalMatrix);
        }

        void ModelViewMult(vertex_trans_matrix_type const& m)
        {
            _VertexArray.multiply(m);

            vertex_trans_matrix_type normalMatrix(m.inverse());
            normalMatrix.replaceWithAdjoint();
            _NormalArray.multiply(normalMatrix);
        }

        void TextureLeftMult(texcoord_matrix_type const& m)
        {
            _TexCoordArray.leftmultiply(m);
        }

        void TextureLeftMult(texcoord_trans_matrix_type const& m)
        {
            _TexCoordArray.leftmultiply(m);
        }

        void TextureMult(texcoord_matrix_type const& m)
        {
            _TexCoordArray.multiply(m);
        }

        void TextureMult(texcoord_trans_matrix_type const& m)
        {
            _TexCoordArray.multiply(m);
        }

        bool HasVertexVBO() const
        {
            return _VertexArray.HasVBO();
        }

        void UseVertexVBO(bool const vbo = true)
        {
            if (vbo)
                _VertexArray.UseVBO();
            else
                _VertexArray.UseVA();
        }

        bool HasTexVBO() const
        {
            return _TexCoordArray.HasVBO();
        }

        void UseTexVBO(bool const vbo = false)
        {
            if (vbo)
                _TexCoordArray.UseVBO();
            else
                _TexCoordArray.UseVA();
        }

        std::size_t uniqueVertices() const
        {
            return _VertexArray.size();
        }

        std::size_t drawnVertices() const
        {
            return _indices.size();
        }

        void AddTriangle(const vertex_type* v, const texcoord_type* t, const normal_type* n)
        {
            for (unsigned int i = 0; i < 3; ++i)
                AddPoint(v[i], t[i], n[i]);
        }

        void AddTriangle(const vertex_type* v, const normal_type* n)
        {
            static texcoord_type t;
            for (unsigned int i = 0; i < static_cast<unsigned int>(t.size()); ++i)
                t[i] = 0;

            for (unsigned int i = 0; i < 3; ++i)
                AddPoint(v[i], t, n[i]);
        }

    private:
        void AddPoint(const vertex_type& vertex, const texcoord_type& texcoord, const normal_type& normal)
        {
            unsigned int index = 0;

            // Try to find an instance of the given vertex
            for (index = 0; index < _VertexArray.size(); ++index)
            {
                if (_VertexArray[index]   == vertex
                 && _TexCoordArray[index] == texcoord
                 && _NormalArray[index]   == normal)
                {
                    // If we found a vertex that's the same then add the index
                    // to the index table and bail out
                    _indices.push_back(index);

                    return;
                }
            }

            // If we found no instance of this vertex then we'll obviously need to add it
            index = _VertexArray.push_back(vertex);
            _TexCoordArray.push_back(texcoord);
            _NormalArray.push_back(normal);
            assert(index == _VertexArray.size()   - 1
                && index == _TexCoordArray.size() - 1
                && index == _NormalArray.size()   - 1);
            _indices.push_back(index);
        }

    private:
        VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs> _VertexArray;
        NormalArray<NormalCoordType, supportNormalVBOs> _NormalArray;
        TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs> _TexCoordArray;
        std::vector<IndexIntegerType> _indices;
};

#endif // __INCLUDED_IMD_HPP__
