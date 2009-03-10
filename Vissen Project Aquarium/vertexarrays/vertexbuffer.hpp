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

#ifndef __INCLUDED_VERTEXBUFFER_HPP__
#define __INCLUDED_VERTEXBUFFER_HPP__

#include <GL/gl.h>
#include <stdexcept>

class GLRuntimeError : public std::runtime_error
{
    public:
        explicit GLRuntimeError(const std::string& what);
};

class GLUnsupported : public GLRuntimeError
{
    public:
        explicit GLUnsupported(const std::string& what);
};

class GLLogicError : public std::logic_error
{
    public:
        explicit GLLogicError(const std::string& what);
};

class GLDrawSection : public GLLogicError
{
    public:
        explicit GLDrawSection(const std::string& what);
};

class GLInvalidValue : public GLLogicError
{
    public:
        explicit GLInvalidValue(const std::string& what);
};

class GLInvalidEnum : public GLLogicError
{
    public:
        explicit GLInvalidEnum(const std::string& what);
};
class VertexBufferObject
{
    public:
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
            STATIC_DRAW    = GL_STATIC_DRAW,

            /** The data stored in the buffer object is likely to change
             *  frequently but is likely to be used as a source for drawing
             *  several times between changes. This hint tells the
             *  implementation to put the data somewhere it won't be too painful
             *  to update once in a while.
             */
            DYNAMIC_DRAW   = GL_DYNAMIC_DRAW,

            /** The data stored in the buffer object is likely to change
             *  frequently and will be used only once (or at least very few
             *  times) in between changes. This hint tells the implementation
             *  that you have time-sensitive data such as animated geometry that
             *  will be used once and then replaced. It is crucial that data be
             *  placed somewhere quick to update, even at the expense of faster
             *  rendering.
             */
            STREAMING_DRAW = GL_STREAM_DRAW,
        };

    public:
        static bool is_supported();

        VertexBufferObject();
        ~VertexBufferObject();

        void bind() const;
        static void unbind();

        void bufferData(ptrdiff_t size, void const * const data, buffer_usage method = STATIC_DRAW);
        void bufferSubData(ptrdiff_t offset, ptrdiff_t size, void const * const data);

        std::size_t size() const;

        void clear();

    private:
        GLuint  _vbo;
        size_t  _size;
};

#endif // __INCLUDED_VERTEXBUFFER_HPP__
