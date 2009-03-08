#ifndef __INCLUDED_SHADERS_HPP__
#define __INCLUDED_SHADERS_HPP__

#include <boost/noncopyable.hpp>
#include <Eigen/Core>
#include <istream>
#include "GL/GLee.h"

namespace OpenGL
{
	class Shader : public boost::noncopyable
	{
		public:
			virtual ~Shader();

			void loadSource(const char* source);
			void loadSource(const std::string& source);
			void loadSource(std::istream& source);

			void compile();

		protected:
			Shader(GLenum type);

		private:
			GLuint _shader;

			friend class Program;
	};

	class VertexShader : public Shader
	{
		public:
			VertexShader();
	};

	class FragmentShader : public Shader
	{
		public:
			FragmentShader();
	};

	class Program
	{
		public:
			Program();
			virtual ~Program();

			void attachShader(const Shader& shader);
			void detachShader(const Shader& shader);

			void link();

			bool validate();

			void use();

			static void noUse();

		protected:
			GLint getUniformLocation(const char* name);

			void getUniform(GLint location, GLfloat* params);
			void getUniform(GLint location, GLint* params);

			// For single values
			void uniform(GLint location, GLfloat v0);
			void uniform(GLint location, const Eigen::Vector2f& value);
			void uniform(GLint location, const Eigen::Vector3f& value);
			void uniform(GLint location, const Eigen::Vector4f& value);
			void uniform(GLint location, GLint v0);
			void uniform(GLint location, GLint v0, GLint v1);
			void uniform(GLint location, GLint v0, GLint v1, GLint v2);
			void uniform(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

			// For arrays
			void uniform1v(GLint location, GLsizei count, const GLfloat * value);
			void uniform2v(GLint location, GLsizei count, const GLfloat * value);
			void uniform3v(GLint location, GLsizei count, const GLfloat * value);
			void uniform4v(GLint location, GLsizei count, const GLfloat * value);
			void uniform1v(GLint location, GLsizei count, const GLint * value);
			void uniform2v(GLint location, GLsizei count, const GLint * value);
			void uniform3v(GLint location, GLsizei count, const GLint * value);
			void uniform4v(GLint location, GLsizei count, const GLint * value);

			// For matrices
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 2, 2>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 3, 3>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 4, 4>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 2, 3>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 3, 2>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 2, 4>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 4, 2>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 3, 4>& value);
			void uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 4, 3>& value);

		private:
			GLuint _program;
	};

	class use_scoped_program : boost::noncopyable
	{
		public:
			use_scoped_program(Program& program);
			~use_scoped_program();

		private:
			static GLuint currentProgram();

		private:
			const GLuint _old_program;
	};
}

#endif // __INCLUDED_SHADERS_HPP__
