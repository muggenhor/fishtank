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

#ifndef __INCLUDED_ABSTRACTARRAY_HPP__
#define __INCLUDED_ABSTRACTARRAY_HPP__

#include "abstractarraybase.hpp"
#include "vertexbuffer.hpp"

template <typename CoordType, std::size_t CoordinateCount, bool supportVBO = true>
class AbstractArray;

template <typename CoordType, std::size_t CoordinateCount>
class AbstractArray<CoordType, CoordinateCount, false> : public AbstractArrayBase<CoordType, CoordinateCount, AbstractArray<CoordType, CoordinateCount, false> >
{
    public:
        typedef AbstractArrayBase<CoordType, CoordinateCount,
                AbstractArray<CoordType, CoordinateCount, false> >  base_type;
        typedef typename base_type::value_type                      value_type;
        typedef typename base_type::trans_value_type                trans_value_type;
        typedef typename base_type::matrix_type                     matrix_type;
        typedef typename base_type::trans_matrix_type               trans_matrix_type;

    protected:
        virtual void glPassPointer(value_type const * data) const = 0;

    private:
        friend class AbstractArrayBase<CoordType, CoordinateCount,
               AbstractArray<CoordType, CoordinateCount, false> >;

        /** Passes all of this AbstractArray's data to the OpenGL API.
         */
        void _draw() const
        {
            // This is required to make sure that OpenGL can actually work with our data
            BOOST_STATIC_ASSERT(sizeof(value_type) == sizeof(CoordType[CoordinateCount]));

            glPassPointer(&(*this)[0]);
        }

        void DataChanged() {}

        bool _HasVBO() const
        {
            return false;
        }
};

template <typename CoordType, std::size_t CoordinateCount>
class AbstractArray<CoordType, CoordinateCount, true> : public AbstractArrayBase<CoordType, CoordinateCount, AbstractArray<CoordType, CoordinateCount, true> >
{
    public:
        typedef AbstractArrayBase<CoordType, CoordinateCount,
                AbstractArray<CoordType, CoordinateCount, true> >   base_type;
        typedef typename base_type::value_type                      value_type;
        typedef typename base_type::trans_value_type                trans_value_type;
        typedef typename base_type::matrix_type                     matrix_type;
        typedef typename base_type::trans_matrix_type               trans_matrix_type;

        AbstractArray() :
            _vbo(VertexBufferObject::is_supported() ? new VertexBufferObject : 0),
            _vbo_updated(false)
        {}

        AbstractArray(bool use_vbo) :
            _vbo(use_vbo ? new VertexBufferObject : 0),
            _vbo_updated(false)
        {}

        void UseVBO()
        {
            if (!_vbo)
            {
                _vbo_updated = false;
                _vbo = new VertexBufferObject;
            }
        }

        void UseVA()
        {
            delete _vbo;
            _vbo = 0;
        }

    protected:
        virtual void glPassPointer(value_type const * data) const = 0;

    private:
        friend class AbstractArrayBase<CoordType, CoordinateCount,
               AbstractArray<CoordType, CoordinateCount, true> >;

        /** Passes all of this AbstractArray's data to the OpenGL API.
         */
        void _draw() const
        {
            if (_vbo)
            {
                if (!_vbo_updated)
                {
                    if (sizeof(value_type) == sizeof(CoordType[CoordinateCount]))
                    {
                        _vbo->bufferData(base_type::size() * sizeof(value_type), &(*this)[0]);
                    }
                    else
                    {
                        // First allocate the buffer
                        _vbo->bufferData(base_type::size() * sizeof(CoordType[CoordinateCount]), 0);

                        // Now fill it
                        for (unsigned int vec = 0; vec < base_type::size(); ++vec)
                        {
                            for (unsigned int coord = 0; coord < CoordinateCount; ++coord)
                            {
                                _vbo->bufferSubData(sizeof(CoordType[CoordinateCount]) * vec + sizeof(CoordType) * coord, sizeof(CoordType), &(*this)[vec][coord]);
                            }
                        }
                    }
                }

                _vbo->bind();
                glPassPointer(0);
                _vbo->unbind();
            }
            else
            {
                // This is required to make sure that OpenGL can actually work with our data
                assert(sizeof(value_type) == sizeof(CoordType[CoordinateCount]));

                glPassPointer(&(*this)[0]);
            }
        }

        void DataChanged()
        {
            _vbo_updated = false;
        }

        bool _HasVBO() const
        {
            return _vbo != 0;
        }

        VertexBufferObject*     _vbo;
        mutable bool            _vbo_updated;
};

#endif // __INCLUDED_ABSTRACTARRAY_HPP__
