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
#include <boost/serialization/vector.hpp>
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
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs>::transform_type     vertex_transform_type;
        typedef typename TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs>::transform_type                 texcoord_transform_type;

        TriangleArray(const bool use_indices = true) :
#ifndef NDEBUG
            _indices_modified(false),
#endif
            _indices(use_indices ? new std::vector<IndexIntegerType> : 0)
        {
        }

        ~TriangleArray()
        {
            delete _indices;
        }

        void draw() const
        {
            // Bail out if there's nothing to draw
            if (empty())
                return;

            // Pass all of our triangle data
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);

            assert(_VertexArray.size() == _TexCoordArray.size()
                && _VertexArray.size() == _NormalArray.size());

            // Pass the VertexArray
            _VertexArray.draw();
            _TexCoordArray.draw();
            _NormalArray.draw();

            if (_indices)
            {
#ifndef NDEBUG
                if (_indices_modified)
                {
                    for (typename std::vector<IndexIntegerType>::const_iterator i = _indices->begin(); i != _indices->end(); ++i)
                        assert(*i < _VertexArray.size());
                    _indices_modified = false;
                }
#endif

                // Draw all contained triangles based on the array of indices
                if (GLEE_VERSION_1_2)
                    glDrawRangeElements(GL_TRIANGLES, 0, _VertexArray.size() - 1, _indices->size(), OpenGLTypeConstant<IndexIntegerType>::constant, &(*_indices)[0]);
                else if (GLEE_EXT_draw_range_elements)
                    glDrawRangeElementsEXT(GL_TRIANGLES, 0, _VertexArray.size() - 1, _indices->size(), OpenGLTypeConstant<IndexIntegerType>::constant, &(*_indices)[0]);
                else
                    glDrawElements(GL_TRIANGLES, _indices->size(), OpenGLTypeConstant<IndexIntegerType>::constant, &(*_indices)[0]);
            }
            else
            {
                glDrawArrays(GL_TRIANGLES, 0, _VertexArray.size());
            }

            // Disable the GL_VERTEX_ARRAY client state to prevent strange behaviour
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        bool empty() const
        {
            return _VertexArray.empty();
        }

        void clear()
        {
            _VertexArray.clear();
            _TexCoordArray.clear();
            _NormalArray.clear();
            if (_indices)
                _indices->clear();

            _indices_modified = true;
        }

        void ModelViewLeftMult(vertex_transform_type const& m)
        {
            _VertexArray.leftmultiply(m);
            // TODO: Transpose the inversed matrix
            _NormalArray.leftmultiply(m.inverse());
        }

        void ModelViewMult(vertex_transform_type const& m)
        {
            _VertexArray.multiply(m);
            // TODO: Transpose the inversed matrix
            _NormalArray.multiply(m.inverse());
        }

        void TextureLeftMult(texcoord_transform_type const& m)
        {
            _TexCoordArray.leftmultiply(m);
        }

        void TextureMult(texcoord_transform_type const& m)
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

        void HasNormalVBO() const
        {
            return _NormalArray.HasVBO();
        }

        void UseNormalVBO(bool const vbo = true)
        {
            if (vbo)
                _NormalArray.UseVBO();
            else
                _NormalArray.UseVA();
        }

        std::size_t uniqueVertices() const
        {
            return _VertexArray.size();
        }

        std::size_t drawnVertices() const
        {
            return _indices ? _indices->size() : _VertexArray.size();
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
            _indices_modified = true;
            unsigned int index = 0;

            if (_indices)
            {
                // Try to find an instance of the given vertex
                for (index = 0; index < _VertexArray.size(); ++index)
                {
                    if (_VertexArray[index]   == vertex
                     && _TexCoordArray[index] == texcoord
                     && _NormalArray[index]   == normal)
                    {
                        // If we found a vertex that's the same then add the index
                        // to the index table and bail out
                        _indices->push_back(index);

                        return;
                    }
                }
            }

            // If we found no instance of this vertex then we'll obviously need to add it
            index = _VertexArray.push_back(vertex);
            _TexCoordArray.push_back(texcoord);
            _NormalArray.push_back(normal);
            assert(index == _VertexArray.size()   - 1
                && index == _TexCoordArray.size() - 1
                && index == _NormalArray.size()   - 1);
            if (_indices)
                _indices->push_back(index);
        }

        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _VertexArray;
            ar & _TexCoordArray;
            ar & _NormalArray;
            ar & _indices;
            _indices_modified = true;
        }

    private:
        VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs> _VertexArray;
        NormalArray<NormalCoordType, supportNormalVBOs> _NormalArray;
        TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs> _TexCoordArray;
#ifndef NDEBUG
        mutable bool _indices_modified;
#endif
        std::vector<IndexIntegerType>* const _indices;
};

#endif // __INCLUDED_IMD_HPP__
