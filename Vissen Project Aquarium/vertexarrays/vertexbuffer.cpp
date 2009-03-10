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

#include <GLee.h>
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

bool VertexBufferObject::is_supported()
{
    return GLEE_VERSION_1_5
        || GLEE_ARB_vertex_buffer_object;
}

VertexBufferObject::VertexBufferObject()
{
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
         * unbind a VBO is not a problem */;
}

void VertexBufferObject::bufferData(size_t size, void const * const data, buffer_usage method)
{
    // First bind the buffer
    bind();

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

    // Unbind the buffer again
    unbind();
}
