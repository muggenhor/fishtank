#include "opengl-support.hpp"
#include <Eigen/Core>
#include "../../GL/GLee.h"
#include <luabind/luabind.hpp>

namespace OpenGL
{

static GLvoid glTranslate(const Eigen::Vector3i& v) { return glTranslatef(v.x(), v.y(), v.z()); }
static GLvoid glTranslate(const Eigen::Vector3f& v) { return glTranslatef(v.x(), v.y(), v.z()); }
static GLvoid glTranslate(const Eigen::Vector3d& v) { return glTranslated(v.x(), v.y(), v.z()); }

static GLvoid glRotate(GLint    angle, const Eigen::Vector3i& v) { return glRotatef(angle, v.x(), v.y(), v.z()); }
static GLvoid glRotate(GLfloat  angle, const Eigen::Vector3f& v) { return glRotatef(angle, v.x(), v.y(), v.z()); }
static GLvoid glRotate(GLdouble angle, const Eigen::Vector3d& v) { return glRotated(angle, v.x(), v.y(), v.z()); }

static GLvoid glScale(const Eigen::Vector3i& v) { return glScalef(v.x(), v.y(), v.z()); }
static GLvoid glScale(const Eigen::Vector3f& v) { return glScalef(v.x(), v.y(), v.z()); }
static GLvoid glScale(const Eigen::Vector3d& v) { return glScaled(v.x(), v.y(), v.z()); }

static GLvoid glColor(const Eigen::Vector3i& v) { return glColor3iv(v.data()); }
static GLvoid glColor(const Eigen::Vector3d& v) { return glColor3dv(v.data()); }
static GLvoid glColor(const Eigen::Vector3f& v) { return glColor3fv(v.data()); }
static GLvoid glColor(const Eigen::Vector4i& v) { return glColor4iv(v.data()); }
static GLvoid glColor(const Eigen::Vector4d& v) { return glColor4dv(v.data()); }
static GLvoid glColor(const Eigen::Vector4f& v) { return glColor4fv(v.data()); }

static GLvoid glTexCoord(const Eigen::Vector2i& v) { return glTexCoord2iv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector2d& v) { return glTexCoord2dv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector2f& v) { return glTexCoord2fv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector3i& v) { return glTexCoord3iv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector3d& v) { return glTexCoord3dv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector3f& v) { return glTexCoord3fv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector4i& v) { return glTexCoord4iv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector4d& v) { return glTexCoord4dv(v.data()); }
static GLvoid glTexCoord(const Eigen::Vector4f& v) { return glTexCoord4fv(v.data()); }

static GLvoid glVertex(const Eigen::Vector2i& v) { return glVertex2iv(v.data()); }
static GLvoid glVertex(const Eigen::Vector2d& v) { return glVertex2dv(v.data()); }
static GLvoid glVertex(const Eigen::Vector2f& v) { return glVertex2fv(v.data()); }
static GLvoid glVertex(const Eigen::Vector3i& v) { return glVertex3iv(v.data()); }
static GLvoid glVertex(const Eigen::Vector3d& v) { return glVertex3dv(v.data()); }
static GLvoid glVertex(const Eigen::Vector3f& v) { return glVertex3fv(v.data()); }
static GLvoid glVertex(const Eigen::Vector4i& v) { return glVertex4iv(v.data()); }
static GLvoid glVertex(const Eigen::Vector4d& v) { return glVertex4dv(v.data()); }
static GLvoid glVertex(const Eigen::Vector4f& v) { return glVertex4fv(v.data()); }

void register_with_lua(lua_State* L)
{
	using namespace luabind;

	module(L, "gl")
	[
		register_with_lua()
	];

	globals(L)["gl"]["NORMALIZE"] = GL_NORMALIZE;
	globals(L)["gl"]["QUADS"] = GL_QUADS;
}

luabind::scope register_with_lua()
{
	using namespace luabind;

	return
		namespace_("matrix")
		[
			def("push",       (GLvoid (*)()) &glPushMatrix),
			def("pop",        (GLvoid (*)()) &glPopMatrix),
			def("translate",  (GLvoid (*)(GLfloat, GLfloat, GLfloat)) &glTranslatef),
			def("translate",  (GLvoid (*)(const Eigen::Vector3i&)) &glTranslate),
			def("translate",  (GLvoid (*)(const Eigen::Vector3f&)) &glTranslate),
			def("translate",  (GLvoid (*)(const Eigen::Vector3d&)) &glTranslate),
			def("rotate",     (GLvoid (*)(GLfloat, GLfloat, GLfloat, GLfloat)) &glRotatef),
			def("rotate",     (GLvoid (*)(GLint,    const Eigen::Vector3i&)) &glRotate),
			def("rotate",     (GLvoid (*)(GLfloat,  const Eigen::Vector3f&)) &glRotate),
			def("rotate",     (GLvoid (*)(GLdouble, const Eigen::Vector3d&)) &glRotate),
			def("scale",      (GLvoid (*)(GLfloat, GLfloat, GLfloat)) &glScalef),
			def("scale",      (GLvoid (*)(const Eigen::Vector3i&)) &glScale),
			def("scale",      (GLvoid (*)(const Eigen::Vector3f&)) &glScale),
			def("scale",      (GLvoid (*)(const Eigen::Vector3d&)) &glScale)
		],
		def("enable",     (GLvoid (*)(GLenum)) &glEnable),
		def("disable",    (GLvoid (*)(GLenum)) &glDisable),
		def("Begin",      (GLvoid (*)(GLenum)) &glBegin),
		def("End",        (GLvoid (*)())       &glEnd),
		def("color",      (GLvoid (*)(GLfloat, GLfloat, GLfloat)) &glColor3f),
		def("color",      (GLvoid (*)(GLfloat, GLfloat, GLfloat, GLfloat)) &glColor4f),
		def("color",      (GLvoid (*)(const Eigen::Vector3i&)) &glColor),
		def("color",      (GLvoid (*)(const Eigen::Vector3f&)) &glColor),
		def("color",      (GLvoid (*)(const Eigen::Vector3d&)) &glColor),
		def("color",      (GLvoid (*)(const Eigen::Vector4i&)) &glColor),
		def("color",      (GLvoid (*)(const Eigen::Vector4f&)) &glColor),
		def("color",      (GLvoid (*)(const Eigen::Vector4d&)) &glColor),
		def("texCoord",   (GLvoid (*)(GLfloat, GLfloat)) &glTexCoord2f),
		def("texCoord",   (GLvoid (*)(GLfloat, GLfloat, GLfloat)) &glTexCoord3f),
		def("texCoord",   (GLvoid (*)(GLfloat, GLfloat, GLfloat, GLfloat)) &glTexCoord4f),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector2i&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector2f&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector2d&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector3i&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector3f&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector3d&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector4i&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector4f&)) &glTexCoord),
		def("texCoord",   (GLvoid (*)(const Eigen::Vector4d&)) &glTexCoord),
		def("vertex",     (GLvoid (*)(GLfloat, GLfloat)) &glVertex2f),
		def("vertex",     (GLvoid (*)(GLfloat, GLfloat, GLfloat)) &glVertex3f),
		def("vertex",     (GLvoid (*)(GLfloat, GLfloat, GLfloat, GLfloat)) &glVertex4f),
		def("vertex",     (GLvoid (*)(const Eigen::Vector2i&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector2f&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector2d&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector3i&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector3f&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector3d&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector4i&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector4f&)) &glVertex),
		def("vertex",     (GLvoid (*)(const Eigen::Vector4d&)) &glVertex);
}

}
