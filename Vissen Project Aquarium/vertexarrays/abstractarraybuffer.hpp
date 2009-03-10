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
#include <stdexcept>
#include <GLee.h>
#include <GL/gl.h>

template <typename CoordType, std::size_t CoordinateCount>
class AbstractArrayBuffer
{
    public:
        typedef AbstractArrayBuffer<CoordType, CoordinateCount> this_type;
        typedef Eigen::Vector<CoordType, CoordinateCount>       value_type;

        /** Used to indicate how often this VBO will be updated. This is a
         *  performance hint.
         */
        enum buffer_usage
        {
            /** The data stored in the buffer object is unlikely to change and
             *  will be used possibly many times as a source for drawing. This
             *  hint tells the implementation to put the data somewhere it's
             *  quick to draw from, but probably not quick to update.
             */
            static_draw    = GL_STATIC_DRAW,

            /** The data stored in the buffer object is likely to change
             *  frequently but is likely to be used as a source for drawing
             *  several times between changes. This hint tells the
             *  implementation to put the data somewhere it won't be too painful
             *  to update once in a while.
             */
            dynamic_draw   = GL_DYNAMIC_DRAW,

            /** The data stored in the buffer object is likely to change
             *  frequently and will be used only once (or at least very few
             *  times) in between changes. This hint tells the implementation
             *  that you have time-sensitive data such as animated geometry that
             *  will be used once and then replaced. It is crucial that data be
             *  placed somewhere quick to update, even at the expense of faster
             *  rendering.
             */
            streaming_draw = GL_STREAM_DRAW,
        };

        AbstractArrayBuffer() :
            _data_updated(true),
            _vbo_created(false),
            _vbo_updated(false)
        {
            assert(supported() && "This version of OpenGL doesn't support VBOs!");
            if (!supported())
                throw std::domain_error("No OpenGL VBO extensions available! AbstractArrayBuffers shouldn't be used!");

            // Reset the current error code
            glGetError();

            // Generate our vertex buffer object
            if (GLEE_VERSION_1_5)
                glGenBuffers(1, &_vbo);
            else if (GLEE_ARB_vertex_buffer_object)
                glGenBuffersARB(1, &_vbo);

            // Check to see whether buffer generation was succesful
            const GLenum error = glGetError();
            switch (error)
            {
                case GL_NO_ERROR:
                    // No error occurred
                    break;

                case GL_INVALID_VALUE:
                    // Should only happen if the first parameter to glGenBuffers
                    // is negative.
                    assert(!"Negative buffer count provided for glGenBuffers!?");
                    break;

                case GL_INVALID_OPERATION:
                    // This error is generated if glGenBuffers is executed
                    // between the execution of glBegin and the corresponding
                    // execution of glEnd.
                    // So this would be a programming error by the client
                    // programmer, so assert() and throw.
                    assert(!"Construction of AbstractArrayBuffer must not occur between a glBegin and glEnd call!");
                    throw std::domain_error("Construction of AbstractArrayBuffer occurred between a glBegin and glEnd call, this should _not_ be done!");
                    break;
            }
        }

        /** Virtual destructor to make sure that all subclasses have a virtual
         *  destructor as well.
         */
        virtual ~AbstractArrayBuffer()
        {
            // Reset the current error code
            glGetError();

            // Delete our vertex buffer object
            if (GLEE_VERSION_1_5)
                glDeleteBuffers(1, &_vbo);
            else if (GLEE_ARB_vertex_buffer_object)
                glDeleteBuffersARB(1, &_vbo);

            // Check to see whether texture destruction was successful
            GLenum error = glGetError();
            switch (error)
            {
                case GL_NO_ERROR:
                    // No error occurred
                    break;

                case GL_INVALID_VALUE:
                    // Should only happen if the first parameter to
                    // glDeleteBuffers is negative.
                    assert(!"Negative texture count provided for glDeleteBuffers!?");
                    break;

                case GL_INVALID_OPERATION:
                    // This error is generated if glDeleteBuffers is executed
                    // between the execution of glBegin and the corresponding
                    // execution of glEnd.
                    // So this would be a programming error by the client
                    // programmer, so assert() and throw.
                    assert(!"Destruction of AbstractArrayBuffer must not occur between a glBegin and glEnd call! (So make sure to call glEnd _before_ leaving the scope of this AbstractArrayBuffer).");
                    throw std::domain_error("Destruction of AbstractArrayBuffer occurred between a glBegin and glEnd call, this should _not_ be done!");
                    break;
            }
        }

        /** Passes all of this AbstractArrayBuffer's data to the OpenGL API.
         */
        void draw() const
        {
            if (_vbo_updated)
            {
                bind();
                glPassPointer(0);
                unbind();
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
        void updateBuffer(const buffer_usage usage = static_draw, const bool realloc = false)
        {
            assert(_data_updated || _vbo_updated);

            // Make sure that a stray call to updateBuffer() will not destroy all
            // data if it happens to reside in video RAM only.
            if (!_data_updated)
                return;

            // If the VBO is up to date already there's no need to update it.
            if (_vbo_updated)
                return;

            bind();

            // Check whether we need to (re)allocate our buffer (by using
            // glBufferData instead of glBufferSubData).
            const bool allocate_buffer = !_vbo_created
                                      || (_vbo_size != _data.size() && realloc)
                                      || _vbo_size  < _data.size();

            if (GLEE_VERSION_1_5)
            {
                if (allocate_buffer)
                {
                    glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(value_type), &_data[0], usage);
                    _vbo_size = _data.size();
                }
                else
                {
                    glBufferSubData(GL_ARRAY_BUFFER, 0, _data.size(), &_data[0]);
                }
            }
            else if (GLEE_ARB_vertex_buffer_object)
            {
                if (allocate_buffer)
                {
                    glBufferDataARB(GL_ARRAY_BUFFER, _data.size() * sizeof(value_type), &_data[0], usage);
                    _vbo_size = _data.size();
                }
                else
                {
                    glBufferSubDataARB(GL_ARRAY_BUFFER, 0, _data.size(), &_data[0]);
                }
            }
            else
                assert(!"No OpenGL VBO extensions available ? The constructor should have thrown already!");

            unbind();

            _vbo_created = true;
            _vbo_updated = true;
        }

        /** Will unbind the currently bound (if any) Vertex Buffer Object.
         */
        static void unbind()
        {
            if (GLEE_VERSION_1_5)
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            else if (GLEE_ARB_vertex_buffer_object)
                glBindBufferARB(GL_ARRAY_BUFFER, 0);
        }

        /** Checks whether AbstractArrayBuffer is supported on this OpenGL
         *  implementation.
         *  @return true if it is supported and can safely be constructed, false
         *          otherwise.
         */
        static bool supported()
        {
            return GLEE_VERSION_1_5 || GLEE_ARB_vertex_buffer_object;
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

            // Bind our buffer so we can read from it
            bind();

            // Retrieve all elements from video memory and dump it into our own
            // client memory.
            if (GLEE_VERSION_1_5)
                glGetBufferSubData(GL_ARRAY_BUFFER, 0, _vbo_size, &_data[0]);
            else if (GLEE_ARB_vertex_buffer_object)
                glGetBufferSubDataARB(GL_ARRAY_BUFFER, 0, _vbo_size, &_data[0]);
            else
                assert(!"No OpenGL VBO extensions available ? The constructor should have thrown already!");

            unbind();

            _data_updated = true;
        }

        void bind() const
        {
            if (GLEE_VERSION_1_5)
                glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            else if (GLEE_ARB_vertex_buffer_object)
                glBindBufferARB(GL_ARRAY_BUFFER, _vbo);
            else
                assert(!"No OpenGL VBO extensions available ? The constructor should have thrown already!");
        }

    private:
        /** Holds all data.
         */
        mutable std::vector<value_type> _data;
        mutable bool                    _data_updated;
        GLuint                          _vbo;
        bool                            _vbo_created;
        bool                            _vbo_updated;
        std::size_t                     _vbo_size;
};

#endif // __INCLUDED_ABSTRACTARRAYBUFFER_HPP__
