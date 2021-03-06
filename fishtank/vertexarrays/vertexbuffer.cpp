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

#include "../GL/GLee.h"
#include "vertexbuffer.hpp"
#include <cassert>

GLRuntimeError::GLRuntimeError(const std::string& what) :
    std::runtime_error(what)
{
}

GLUnsupported::GLUnsupported(const std::string& what) :
    GLRuntimeError(what)
{
}

GLLogicError::GLLogicError(const std::string& what) :
    std::logic_error(what)
{
}

GLDrawSection::GLDrawSection(const std::string& what) :
    GLLogicError(what)
{
}

GLInvalidValue::GLInvalidValue(const std::string& what) :
    GLLogicError(what)
{
}

GLInvalidEnum::GLInvalidEnum(const std::string& what) :
    GLLogicError(what)
{
}

bool VertexBufferObject::is_supported()
{
    return GLEE_VERSION_1_5
        || GLEE_ARB_vertex_buffer_object;
}

VertexBufferObject::VertexBufferObject() :
    _size(0)
{
    assert(is_supported() && "This version of OpenGL doesn't support VBOs!");

    // Reset the current error code
    glGetError();

    if (GLEE_VERSION_1_5)
        glGenBuffers(1, &_vbo);
    else if (GLEE_ARB_vertex_buffer_object)
        glGenBuffersARB(1, &_vbo);
    else
        throw GLUnsupported("Neither OpenGL 1.5 nor the ARB_vertex_buffer_object extension are supported.");

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
            assert(!"Construction of VertexBufferObject must not occur between a glBegin and glEnd call!");
            throw GLDrawSection("Construction of VertexBufferObject occurred between a glBegin and glEnd call, this should _not_ be done!");
            break;
    }
}

VertexBufferObject::~VertexBufferObject()
{
    // Reset the current error code
    glGetError();

    if (GLEE_VERSION_1_5)
        glDeleteBuffers(1, &_vbo);
    else if (GLEE_ARB_vertex_buffer_object)
        glDeleteBuffersARB(1, &_vbo);
    else
        /* We simply shouldn't ever get here. As we shouldn't have been
         * able to create a VBO in the first place, when there are no
         * VBO functions available. */
        assert(!"VBO created without VBO functions");

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
            assert(!"Destruction of VertexBufferObject must not occur between a glBegin and glEnd call! (So make sure to call glEnd _before_ leaving the scope of this VertexBufferObject).");
            throw GLDrawSection("Destruction of VertexBufferObject occurred between a glBegin and glEnd call, this should _not_ be done!");
            break;
    }
}

void VertexBufferObject::bind() const
{
    if (GLEE_VERSION_1_5)
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    else if (GLEE_ARB_vertex_buffer_object)
        glBindBufferARB(GL_ARRAY_BUFFER, _vbo);
    else
        /* We simply shouldn't ever get here. As we shouldn't have been
         * able to create a VBO in the first place, when there are no
         * VBO functions available. */
        assert(!"VBO created without VBO functions");
}

void VertexBufferObject::unbind()
{
    if (GLEE_VERSION_1_5)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    else if (GLEE_ARB_vertex_buffer_object)
        glBindBufferARB(GL_ARRAY_BUFFER, 0);
    else
        /* If the VBO functions are unavailable, then we cannot
         * possibly have bound a VBO object. So not being able to
         * unbind a VBO is not a problem */
        (void)0;
}

void VertexBufferObject::bufferData(ptrdiff_t const size, void const * const data, buffer_usage const method)
{
    // First bind the buffer
    bind();

    // Reset the current error code
    glGetError();

    // Fill it
    if (GLEE_VERSION_1_5)
        glBufferData(GL_ARRAY_BUFFER, size, data, method);
    else if (GLEE_ARB_vertex_buffer_object)
        glBufferDataARB(GL_ARRAY_BUFFER, size, data, method);
    else
        /* We simply shouldn't ever get here. As we shouldn't have been
         * able to create a VBO in the first place, when there are no
         * VBO functions available. */
        assert(!"VBO created without VBO functions");

    // Check to see whether buffer generation was succesful
    const GLenum error = glGetError();
    switch (error)
    {
        case GL_NO_ERROR:
            // No error occurred
            break;

        case GL_INVALID_ENUM:
            if (method != GL_STREAM_DRAW
             && method != GL_STREAM_READ
             && method != GL_STREAM_COPY
             && method != GL_STATIC_DRAW
             && method != GL_STATIC_READ
             && method != GL_STATIC_COPY
             && method != GL_DYNAMIC_DRAW
             && method != GL_DYNAMIC_READ
             && method != GL_DYNAMIC_COPY)
            {
                assert(!"`method' is none of GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ or GL_DYNAMIC_COPY!");
                throw GLInvalidEnum("Parameter `method` passed to function VertexBufferObject::bufferData is none of STREAM_DRAW, STREAM_READ, STREAM_COPY, STATIC_DRAW, STATIC_READ, STATIC_COPY, DYNAMIC_DRAW, DYNAMIC_READ or DYNAMIC_COPY");
            }
            else
            {
                if (GLEE_VERSION_2_1)
                    // Targets GL_PIXEL_PACK_BUFFER and GL_PIXEL_UNPACK_BUFFER
                    // are available if the GL version is 2.1 or greater.
                    assert(!"`target' for glBufferData is none of GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER or GL_PIXEL_UNPACK_BUFFER!");
                else
                    assert(!"`target' for glBufferData is none of GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER!");
            }
            break;

        case GL_INVALID_VALUE:
            // Should only happen if the size parameter is negative. This would
            // be a programming error by the client programmer, so assert() and
            // throw as well.
            assert(!"Negative buffer size provided for glBufferData!?");
            throw GLInvalidValue("Negative `size' parameter passed to function VertexBufferObject::bufferData");
            break;

        case GL_INVALID_OPERATION:
            // This error is generated if glGenBuffers is executed
            // between the execution of glBegin and the corresponding
            // execution of glEnd.
            // So this would be a programming error by the client
            // programmer, so assert() and throw.
            // @NOTE This error can occur also when the reserved buffer object
            //       `0' is bound to `target`, which would mean something went
            //       wrong during bind().
            assert(!"Calling of VertexBufferObject::bufferData must not occur between a glBegin and glEnd call!");
            throw GLDrawSection("Call to VertexBufferObject::bufferData occurred between a glBegin and glEnd call, this should _not_ be done!");
            break;

        case GL_OUT_OF_MEMORY:
            // When this error occurs it'll mean that the OpenGL implementation
            // couldn't allocate memory on the video device, nor in main
            // memory. Thus throw a bad_alloc.
            throw std::bad_alloc();
            break;
    }

    _size = size;

    // Unbind the buffer again
    unbind();
}

void VertexBufferObject::bufferSubData(ptrdiff_t const offset, ptrdiff_t const size, void const * const data)
{
    // First bind the buffer
    bind();

    // Reset the current error code
    glGetError();

    if (GLEE_VERSION_1_5)
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    else if (GLEE_ARB_vertex_buffer_object)
        glBufferSubDataARB(GL_ARRAY_BUFFER, offset, size, data);
    else
        /* We simply shouldn't ever get here. As we shouldn't have been
         * able to create a VBO in the first place, when there are no
         * VBO functions available. */
        assert(!"VBO created without VBO functions");

    // Check to see whether copying of data into the buffer was succesful
    const GLenum error = glGetError();
    switch (error)
    {
        case GL_NO_ERROR:
            // No error occurred
            break;

        case GL_INVALID_ENUM:
            if (GLEE_VERSION_2_1)
                // Targets GL_PIXEL_PACK_BUFFER and GL_PIXEL_UNPACK_BUFFER
                // are available if the GL version is 2.1 or greater.
                assert(!"`target' for glBufferSubData is none of GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER or GL_PIXEL_UNPACK_BUFFER!");
            else
                assert(!"`target' for glBufferSubData is none of GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER!");
            break;

        case GL_INVALID_VALUE:
            // Should only happen if the offset or size parameter is negative,
            // or if together they define a region of memory that extends
            // beyond the buffer object's allocated data store. This would be a
            // programming error by the client programmer, so assert() and
            // throw as well.
            if (offset < 0)
            {
                assert(!"Negative buffer offset provided for glBufferSubData");
                throw GLInvalidValue("Negative `offset' parameter passed to function VertexBufferObject::bufferSubData");
            }
            else if (size < 0)
            {
                assert(!"Negative data size provided for glBufferSubData");
                throw GLInvalidValue("Negative `size' parameter passed to function VertexBufferObject::bufferSubData");
            }
            else
            {
                assert(!"offset and size define a region of memory that extends beyond the buffer object's data store!");
                throw GLInvalidValue("`offset` and `size` parameters passed to function VertexBufferObject::bufferSubData define a region that extends beyond the VertexBufferObject's buffer size");
            }
            break;

        case GL_INVALID_OPERATION:
            // This error is generated if glGenBuffers is executed
            // between the execution of glBegin and the corresponding
            // execution of glEnd.
            // So this would be a programming error by the client
            // programmer, so assert() and throw.
            // @NOTE This error can occur also when the reserved buffer object
            //       `0' is bound to `target`, which would mean something went
            //       wrong during bind().
            assert(!"Calling of VertexBufferObject::bufferSubData must not occur between a glBegin and glEnd call!");
            throw GLDrawSection("Call to VertexBufferObject::bufferSubData occurred between a glBegin and glEnd call, this should _not_ be done!");
            break;
    }

    // Unbind the buffer again
    unbind();
}

void VertexBufferObject::getSubData(ptrdiff_t const offset, ptrdiff_t const size, void * const data) const
{
    // First bind the buffer
    bind();

    // Reset the current error code
    glGetError();

    // Retrieve the data from the buffer
    if (GLEE_VERSION_1_5)
        glGetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    else if (GLEE_ARB_vertex_buffer_object)
        glGetBufferSubDataARB(GL_ARRAY_BUFFER, offset, size, data);
    else
        /* We simply shouldn't ever get here. As we shouldn't have been
         * able to create a VBO in the first place, when there are no
         * VBO functions available. */
        assert(!"VBO created without VBO functions");

    // Check to see whether copying of data from the buffer was succesful
    const GLenum error = glGetError();
    switch (error)
    {
        case GL_NO_ERROR:
            // No error occurred
            break;

        case GL_INVALID_ENUM:
            if (GLEE_VERSION_2_1)
                // Targets GL_PIXEL_PACK_BUFFER and GL_PIXEL_UNPACK_BUFFER
                // are available if the GL version is 2.1 or greater.
                assert(!"`target' for glGetBufferSubData is none of GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER or GL_PIXEL_UNPACK_BUFFER!");
            else
                assert(!"`target' for glGetBufferSubData is none of GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER!");
            break;

        case GL_INVALID_VALUE:
            // Should only happen if the offset or size parameter is negative,
            // or if together they define a region of memory that extends
            // beyond the buffer object's allocated data store. This would be a
            // programming error by the client programmer, so assert() and
            // throw as well.
            if (offset < 0)
            {
                assert(!"Negative buffer offset provided for glGetBufferSubData");
                throw GLInvalidValue("Negative `offset' parameter passed to function VertexBufferObject::getSubData");
            }
            else if (size < 0)
            {
                assert(!"Negative data size provided for glGetBufferSubData");
                throw GLInvalidValue("Negative `size' parameter passed to function VertexBufferObject::getSubData");
            }
            else
            {
                assert(!"offset and size define a region of memory that extends beyond the buffer object's data store!");
                throw GLInvalidValue("`offset` and `size` parameters passed to function VertexBufferObject::bufferSubData define a region that extends beyond the VertexBufferObject's buffer size");
            }
            break;

        case GL_INVALID_OPERATION:
            // This error is generated if glGenBuffers is executed
            // between the execution of glBegin and the corresponding
            // execution of glEnd.
            // So this would be a programming error by the client
            // programmer, so assert() and throw.
            // @NOTE This error can occur also when the reserved buffer object
            //       `0' is bound to `target`, which would mean something went
            //       wrong during bind().
            assert(!"Calling of VertexBufferObject::getSubData must not occur between a glBegin and glEnd call!");
            throw GLDrawSection("Call to VertexBufferObject::getSubData occurred between a glBegin and glEnd call, this should _not_ be done!");
            break;
    }

    // Unbind the buffer again
    unbind();
}

std::size_t VertexBufferObject::size() const
{
    return _size;
}

void VertexBufferObject::clear()
{
    bufferData(0, 0);
}
