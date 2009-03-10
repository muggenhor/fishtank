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

#ifndef __INCLUDED_ABSTRACTARRAYBUFFER_HPP__
#define __INCLUDED_ABSTRACTARRAYBUFFER_HPP__

#include <vector>
#include <eigen/vector.h>
#include <cassert>
#include "vertexbuffer.hpp"

template <typename CoordType, std::size_t CoordinateCount>
class AbstractArrayBuffer
{
    public:
        typedef AbstractArrayBuffer<CoordType, CoordinateCount> this_type;
        typedef Eigen::Vector<CoordType, CoordinateCount>       value_type;

        AbstractArrayBuffer() :
            _data_updated(true),
            _vbo_updated(false)
        {
        }

        bool HasVBO() const
        {
            return true;
        }

        void UseVBO() {}
        void UseVA()  {}

        /** Virtual destructor to make sure that all subclasses have a virtual
         *  destructor as well.
         */
        virtual ~AbstractArrayBuffer() {}

        /** Passes all of this AbstractArrayBuffer's data to the OpenGL API.
         */
        void draw() const
        {
            // This is required to make sure that OpenGL can actually work with our data
            BOOST_STATIC_ASSERT(sizeof(value_type) == sizeof(CoordType[CoordinateCount]));

            if (_vbo_updated)
            {
                _vbo.bind();
                glPassPointer(0);
                _vbo.unbind();
            }
            else if (_data_updated)
            {
                glPassPointer(&_data[0]);
            }
            else
                assert(!"We should always have either the data or the VBO to be updated!");
        }

        /** @return the amount of elements in this AbstractArrayBuffer
         */
        std::size_t size() const
        {
            if (_vbo_updated)
                return _vbo_size;
            else if (_data_updated)
                return _data.size();
            else
                assert(!"We should always have either the data or the VBO to be updated!");
        }

        /** Deallocates all client memory (i.e. system RAM as opposed to video
         *  RAM) in use by this AbstractArrayBuffer.
         *  @note that calling any non-const member function and any function
         *        that extracts data from this AbstractArrayBuffer will result
         *        in the client memory being reallocated and refilled again.
         *        So you're probably best of waiting with calling this function
         *        until you're finished filling this AbstractArrayBuffer.
         *        So size() and draw() are the only functions that won't cause
         *        a buffer reallocation and filling.
         */
        void deallocClientMemory()
        {
            // First upload all elements to our VBO if it isn't updated yet
            if (!_vbo_updated)
                updateBuffer();

            // Remove all elements from client memory (system RAM as opposed to
            // video RAM).
            std::vector<value_type> tmp;
            _data.swap(tmp);

            _data_updated = false;
        }

        /** @return the element at the given index
         */
        const value_type& operator[](std::size_t index) const
        {
            // Make sure that we have all our data in client memory.
            if (!_data_updated)
                updateClientMemory();

            return _data[index];
        }

        const value_type& at(std::size_t index) const
        {
            // Make sure that we have all our data in client memory.
            if (!_data_updated)
                updateClientMemory();

            return _data.at(index);
        }

        /** Appends the given element to the end of this AbstractArrayBuffer.
         *  @return the index given to this new element.
         */
        std::size_t push_back(const value_type& element)
        {
            // Make sure that we have all our data in client memory.
            if (!_data_updated)
                updateClientMemory();

            _data.push_back(element);

            // The VBO needs updating
            _vbo_updated = false;

            return _data.size() - 1;
        }

        /** Clear the entire buffer
         */
        void clear()
        {
            std::vector<value_type> tmp;
            _data.swap(tmp);

            _data_updated = true;
            _vbo_updated = false;

            // Update the VBO right away (to relinquish the VRAM memory
            // associated with it).
            updateBuffer();
        }

        /** Updates the VBO held in this AbstractArrayBuffer.
         *  @param usage   @see buffer_usage
         *  @param realloc Reallocate a buffer if it's larger than required.
         *                 This will be slower to update, but will waste less
         *                 video RAM.
         */
        void updateBuffer(const VertexBufferObject::buffer_usage usage = VertexBufferObject::STATIC_DRAW, const bool realloc = false)
        {
            assert(_data_updated || _vbo_updated);

            // Make sure that a stray call to updateBuffer() will not destroy all
            // data if it happens to reside in video RAM only.
            if (!_data_updated)
                return;

            // If the VBO is up to date already there's no need to update it.
            if (_vbo_updated)
                return;

            // Check whether we need to (re)allocate our buffer (by using
            // VertexBufferObject::bufferData instead of
            // VertexBufferObject::bufferSubData).
            const bool allocate_buffer = (_vbo.size() != _data.size() && realloc)
                                      || _vbo.size()  < _data.size();

            if (allocate_buffer)
            {
                _vbo.bufferData(_data.size() * sizeof(value_type), &_data[0], usage);
            }
            else
            {
                _vbo.bufferSubData(0, _data.size() * sizeof(value_type), &_data[0]);
            }

            _vbo_updated = true;
            _vbo_size = _data.size();
        }

        /** Checks whether AbstractArrayBuffer is supported on this OpenGL
         *  implementation.
         *  @return true if it is supported and can safely be constructed, false
         *          otherwise.
         */
        static bool supported()
        {
            return VertexBufferObject::is_supported();
        }

    protected:
        virtual void glPassPointer(value_type const * data) const = 0;

    private:
        void updateClientMemory() const
        {
            assert(_data_updated || _vbo_updated);

            // Check whether the VBO is more up to date than our client memory.
            // If it's the client memory then it don't update it (as it's
            // up-to-date already).
            if (!_vbo_updated)
                return;

            // Make sure all data fits into our client memory buffer
            _data.resize(_vbo_size);

            // Retrieve all elements from video memory and dump it into our own
            // client memory.
            _vbo.getSubData(0, _vbo_size, &_data[0]);

            _data_updated = true;
        }

    private:
        /** Holds all data.
         */
        mutable std::vector<value_type> _data;
        mutable bool                    _data_updated;
        VertexBufferObject              _vbo;
        bool                            _vbo_updated;
        std::size_t                     _vbo_size;
};

#endif // __INCLUDED_ABSTRACTARRAYBUFFER_HPP__
