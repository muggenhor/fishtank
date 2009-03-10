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

#include <vector>
#include <eigen/vector.h>
#include "vertexbuffer.hpp"

template <typename CoordType, std::size_t CoordinateCount, bool supportVBO = true>
class AbstractArray;

template <typename CoordType, std::size_t CoordinateCount>
class AbstractArray<CoordType, CoordinateCount, false>
{
    public:
        typedef Eigen::Vector<CoordType, CoordinateCount> value_type;

        /** Virtual destructor to make sure that all subclasses have a virtual
         *  destructor as well.
         */
        virtual ~AbstractArray() {}

        bool HasVBO() const
        {
            return false;
        }

        /** Passes all of this AbstractArray's data to the OpenGL API.
         */
        void draw() const
        {
            // This is required to make sure that OpenGL can actually work with our data
            BOOST_STATIC_ASSERT(sizeof(value_type) == sizeof(CoordType[CoordinateCount]));

            glPassPointer(&_data[0]);
        }

        /** @return the amount of elements in this AbstractArray
         */
        std::size_t size() const
        {
            return _data.size();
        }

        /** @return the element at the given index
         */
        const value_type& operator[](std::size_t index) const
        {
            return _data[index];
        }

        /** Appends the given element to the end of this AbstractArray.
         *  @return the index given to this new element.
         */
        std::size_t push_back(const value_type& element)
        {
            _data.push_back(element);
            return _data.size() - 1;
        }

        /** Removes all elements from this AbstractArray
         */
        void clear()
        {
            _data.clear();
        }

    protected:
        virtual void glPassPointer(value_type const * data) const = 0;

    private:
        /** Holds all data.
         */
        std::vector<value_type> _data;
};

template <typename CoordType, std::size_t CoordinateCount>
class AbstractArray<CoordType, CoordinateCount, true>
{
    public:
        typedef Eigen::Vector<CoordType, CoordinateCount> value_type;

        AbstractArray() :
            _vbo(VertexBufferObject::is_supported() ? new VertexBufferObject : 0),
            _vbo_updated(false)
        {}

        AbstractArray(bool use_vbo) :
            _vbo(use_vbo ? new VertexBufferObject : 0),
            _vbo_updated(false)
        {}

        /** Virtual destructor to make sure that all subclasses have a virtual
         *  destructor as well.
         */
        virtual ~AbstractArray() {}

        bool HasVBO() const
        {
            return _vbo != 0;
        }

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

        /** Passes all of this AbstractArray's data to the OpenGL API.
         */
        void draw() const
        {
            if (_vbo)
            {
                if (!_vbo_updated)
                {
                    if (sizeof(value_type) == sizeof(CoordType[CoordinateCount]))
                    {
                        _vbo->bufferData(_data.size() * sizeof(value_type), &_data[0]);
                    }
                    else
                    {
                        // First allocate the buffer
                        _vbo->bufferData(_data.size() * sizeof(CoordType[CoordinateCount]), 0);

                        // Now fill it
                        for (unsigned int vec = 0; vec < _data.size(); ++vec)
                        {
                            for (unsigned int coord = 0; coord < CoordinateCount; ++coord)
                            {
                                _vbo->bufferSubData(sizeof(CoordType[CoordinateCount]) * vec + sizeof(CoordType) * coord, sizeof(CoordType), &_data[vec][coord]);
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

                glPassPointer(&_data[0]);
            }
        }

        /** @return the amount of elements in this AbstractArray
         */
        std::size_t size() const
        {
            return _data.size();
        }

        /** @return the element at the given index
         */
        const value_type& operator[](std::size_t index) const
        {
            return _data[index];
        }

        /** Appends the given element to the end of this AbstractArray.
         *  @return the index given to this new element.
         */
        std::size_t push_back(const value_type& element)
        {
            _vbo_updated = false;
            _data.push_back(element);
            return _data.size() - 1;
        }

        /** Removes all elements from this AbstractArray
         */
        void clear()
        {
            _data.clear();
            if (_vbo)
                _vbo->clear();
        }

    protected:
        virtual void glPassPointer(value_type const * data) const = 0;

    private:
        /** Holds all data.
         */
        std::vector<value_type> _data;
        VertexBufferObject*     _vbo;
        mutable bool            _vbo_updated;
};

#endif // __INCLUDED_ABSTRACTARRAY_HPP__
