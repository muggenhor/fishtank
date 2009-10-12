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
#include "../glexcept.hpp"
#include "gl_type_constants.hpp"
#include "normalarray.hpp"
#include "vertexarray.hpp"
#include "texcoordarray.hpp"
#include <cassert>

template <typename IndexIntegerType, typename VertexCoordType, typename TexCoordType, typename NormalCoordType, std::size_t textureCount = 1, bool supportVertexVBOs = true, bool supportTexVBOs = true, bool supportNormalVBOs = true, std::size_t VertexCoordinateCount = 3, std::size_t TexCoordCount = 2>
class TriangleArray
{
    public:
        typedef TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs>                                          texcoord_array_type;
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs>::value_type         vertex_type;
        typedef typename texcoord_array_type::value_type                                                            texcoord_type;
        typedef typename NormalArray<NormalCoordType, supportNormalVBOs>::value_type                                normal_type;
        typedef typename VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs>::transform_type     vertex_transform_type;
        typedef typename texcoord_array_type::transform_type                                                        texcoord_transform_type;

        TriangleArray(const bool use_indices = true) :
#ifndef NDEBUG
            _indices_modified(false),
#endif
            _indices(use_indices ? new std::vector<IndexIntegerType> : 0)
        {
            if (textureCount > 1
             && !GLEE_VERSION_1_3
             && !GLEE_ARB_multitexture)
                throw OpenGL::missing_capabilities("The GL_ARB_multitexture extension is required to perform multi-layered texturing.");
        }

        ~TriangleArray()
        {
            delete _indices;
        }

	TriangleArray(const TriangleArray& rhs) :
		_VertexArray(rhs._VertexArray),
		_NormalArray(rhs._NormalArray),
		_TexCoordArrays(rhs._TexCoordArrays),
#ifndef NDEBUG
		_indices_modified(rhs._indices_modified),
#endif
		_indices(rhs._indices ? new std::vector<IndexIntegerType>(*rhs._indices) : 0)
	{
	}

	TriangleArray& operator=(const TriangleArray& rhs)
	{
		_VertexArray = rhs._VertexArray;
		_NormalArray = rhs._NormalArray;
		for (std::size_t i = 0; i < textureCount; ++i)
			_TexCoordArrays[i] = rhs._TexCoordArrays[i];
#ifndef NDEBUG
		_indices_modified = true;
#endif
		std::vector<IndexIntegerType>* new_indices = 0;
		if (rhs._indices)
			new_indices = new std::vector<IndexIntegerType>(*rhs._indices);
		delete _indices;
		_indices = new_indices;

		return *this;
	}

        void draw() const
        {
            // Bail out if there's nothing to draw
            if (empty())
                return;

            // Pass all of our triangle data
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);

            assert((textureCount == 0
                 || _VertexArray.size() == _TexCoordArrays[0].size())
                && _VertexArray.size() == _NormalArray.size());

            // Pass the VertexArray
            _VertexArray.draw();
            _NormalArray.draw();

            // Pass texture data for all texture units
            /* NOTE: Although the single loop would suffice, to allow the
             *       compiler to optimise this code (by removing unreachable
             *       code in case of <= 1 texture) this switch statement is
             *       required.
             */
            switch (textureCount)
            {
                case 0:
                    break;

                case 1:
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    _TexCoordArrays[0].draw();
                    break;

                default:
                {
                    GLenum unit = GL_TEXTURE0;
                    for (size_t i = 0; i < textureCount; ++i)
                    {
                        if (GLEE_VERSION_1_3)
                            glClientActiveTexture(unit++);
                        else if (GLEE_ARB_multitexture)
                            glClientActiveTextureARB(unit++);
                        else
                            assert(!"Shouldn't ever get here!");

                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                        _TexCoordArrays[i].draw();
                    }
                    break;
                }
            }

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

            /* NOTE: Although the single loop would suffice, to allow the
             *       compiler to optimise this code (by removing unreachable
             *       code in case of <= 1 texture) this switch statement is
             *       required.
             */
            switch (textureCount)
            {
                case 0:
                    break;

                case 1:
                    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    break;

                default:
                {
                    GLenum unit = GL_TEXTURE0;
                    for (size_t i = 0; i < textureCount; ++i)
                    {
                        if (GLEE_VERSION_1_3)
                            glClientActiveTexture(unit++);
                        else if (GLEE_ARB_multitexture)
                            glClientActiveTextureARB(unit++);
                        else
                            assert(!"Shouldn't ever get here!");
                        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    }
                    if (GLEE_VERSION_1_3)
                        glClientActiveTexture(GL_TEXTURE0);
                    else if (GLEE_ARB_multitexture)
                        glClientActiveTextureARB(GL_TEXTURE0);
                    else
                        assert(!"Shouldn't ever get here!");
                    break;
                }
            }
        }

        bool empty() const
        {
            return _VertexArray.empty();
        }

        void clear()
        {
            _VertexArray.clear();
            for (size_t i = 0; i < textureCount; ++i)
                _TexCoordArrays[i].clear();
            _NormalArray.clear();
            if (_indices)
                _indices->clear();

#ifndef NDEBUG
            _indices_modified = true;
#endif
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
            for (size_t i = 0; i < textureCount; ++i)
                _TexCoordArrays[i].leftmultiply(m);
        }

        void TextureMult(texcoord_transform_type const& m)
        {
            for (size_t i = 0; i < textureCount; ++i)
                _TexCoordArrays[i].multiply(m);
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
            for (size_t i = 0; i < textureCount; ++i)
                return _TexCoordArrays[i].HasVBO();

            return false;
        }

        void UseTexVBO(bool const vbo = false)
        {
            if (vbo)
            {
                for (size_t i = 0; i < textureCount; ++i)
                    _TexCoordArrays[i].UseVBO();
            }
            else
            {
                for (size_t i = 0; i < textureCount; ++i)
                    _TexCoordArrays[i].UseVA();
            }
        }

        bool HasNormalVBO() const
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

        void UseVBOs(bool const vbo = true)
        {
            UseVertexVBO(vbo);
            UseTexVBO(vbo);
            UseNormalVBO(vbo);
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

        void AddPoint(const vertex_type& vertex, const texcoord_type& texcoord, const normal_type& normal)
        {
            return AddPoint(vertex, &texcoord, &texcoord + 1, normal);
        }

        template <typename TexForwardIterator>
        void AddPoint(const vertex_type& vertex, TexForwardIterator firstTexcoord, const TexForwardIterator lastTexcoord, const normal_type& normal)
        {
            assert(std::distance(firstTexcoord, lastTexcoord) == textureCount);

#ifndef NDEBUG
            _indices_modified = true;
#endif
            unsigned int index = 0;

            if (_indices)
            {
                // Try to find an instance of the given vertex
                for (index = 0; index < _VertexArray.size(); ++index)
                {
                    if (_VertexArray[index]   == vertex
                     && _NormalArray[index]   == normal)
                    {
                        TexForwardIterator compare = firstTexcoord;
                        for (size_t i = 0; i < textureCount; ++i)
                        {
                            assert(compare != lastTexcoord);

                            if (*compare++ != _TexCoordArrays[i][index])
                                goto not_found;
                        }
                        // If we found a vertex that's the same then add the index
                        // to the index table and bail out
                        _indices->push_back(index);

                        return;
                    }
                }
            }

not_found:
            // If we found no instance of this vertex then we'll obviously need to add it
            index = _VertexArray.push_back(vertex);
            _NormalArray.push_back(normal);

            // Add all texture coordinates
            TexForwardIterator insert = firstTexcoord;
            for (size_t i = 0; i < textureCount; ++i)
            {
                assert(insert != lastTexcoord);

                _TexCoordArrays[i].push_back(*insert++);
            }

            assert(index == _VertexArray.size()   - 1
                && (textureCount == 0
                 || index == _TexCoordArrays[0].size() - 1)
                && index == _NormalArray.size()   - 1);
            if (_indices)
                _indices->push_back(index);
        }

    private:
        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive & ar, const unsigned int /* version */)
        {
            ar & _VertexArray;
            ar & _TexCoordArrays;
            ar & _NormalArray;
            ar & _indices;
#ifndef NDEBUG
            _indices_modified = true;
#endif
        }

    private:
        VertexArray<VertexCoordType, VertexCoordinateCount, supportVertexVBOs> _VertexArray;
        NormalArray<NormalCoordType, supportNormalVBOs> _NormalArray;
        TexCoordArray<TexCoordType, TexCoordCount, supportTexVBOs> _TexCoordArrays[textureCount];
#ifndef NDEBUG
        mutable bool _indices_modified;
#endif
        std::vector<IndexIntegerType>* _indices;
};

#endif // __INCLUDED_IMD_HPP__
