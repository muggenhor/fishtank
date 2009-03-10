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

#ifndef __INCLUDED_GL_TYPE_CONSTANTS_HPP__
#define __INCLUDED_GL_TYPE_CONSTANTS_HPP__

#include <GL/gl.h>

template <typename T>
struct OpenGLTypeConstant;

template <>
struct OpenGLTypeConstant<GLbyte>
{
    enum { constant = GL_BYTE };
};

template <>
struct OpenGLTypeConstant<GLubyte>
{
    enum { constant = GL_UNSIGNED_BYTE };
};

template <>
struct OpenGLTypeConstant<GLshort>
{
    enum { constant = GL_SHORT };
};

template <>
struct OpenGLTypeConstant<GLushort>
{
    enum { constant = GL_UNSIGNED_SHORT };
};

template <>
struct OpenGLTypeConstant<GLint>
{
    enum { constant = GL_INT };
};

template <>
struct OpenGLTypeConstant<GLuint>
{
    enum { constant = GL_UNSIGNED_INT };
};

template <>
struct OpenGLTypeConstant<GLfloat>
{
    enum { constant = GL_FLOAT };
};

template <>
struct OpenGLTypeConstant<GLdouble>
{
    enum { constant = GL_DOUBLE };
};

#endif // __INCLUDED_GL_TYPE_CONSTANTS_HPP__
