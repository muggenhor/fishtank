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
