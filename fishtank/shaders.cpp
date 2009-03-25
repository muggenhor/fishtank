#include "shaders.hpp"
#include <boost/scoped_array.hpp>
#include <cassert>
#include "glexcept.hpp"

namespace OpenGL
{
	Shader::Shader(GLenum type) :
		_shader(0)
	{
		assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);

		if (GLEE_VERSION_2_0)
		{
			_shader = glCreateShader(type);

			if (_shader == 0)
				throw object_creation_error("Failed to create a shader object");
		}
		else
		{
			throw missing_capabilities("OpenGL 2.0 or newer required for shader operation. NOTE: At this time the GL_ARB_shader_objects extension isn't supported.");
		}
	}

	Shader::~Shader()
	{
		if (GLEE_VERSION_2_0
		 && _shader)
			glDeleteShader(_shader);
	}

	void Shader::loadSource(const char* source)
	{
		glShaderSource(_shader, 1, &source, NULL);
	}

	void Shader::loadSource(const std::string& source)
	{
		const GLint length = source.length();
		const GLchar* pSourceText = source.c_str();

		glShaderSource(_shader, 1, &pSourceText, &length);
	}

	void Shader::loadSource(std::istream& source)
	{
		source.seekg(0, std::ios_base::end);

		const GLint length = source.tellg();
		source.seekg(0, std::ios_base::beg);

		boost::scoped_array<char> sourceText(new char[length]);
		source.read(sourceText.get(), length);

		const GLchar* pSourceText = sourceText.get();
		glShaderSource(_shader, 1, &pSourceText, &length);
	}

	void Shader::compile()
	{
		glCompileShader(_shader);

		GLint success;
		glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLint infoLogSize;
			glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &infoLogSize);
			boost::scoped_array<GLchar> infoLog(new GLchar[infoLogSize]);
			glGetShaderInfoLog(_shader, infoLogSize, NULL, infoLog.get());

			throw shader_source_error("Failed to compile shader.", infoLog.get());
		}
	}

	VertexShader::VertexShader() :
		Shader(GL_VERTEX_SHADER)
	{
	}

	FragmentShader::FragmentShader() :
		Shader(GL_FRAGMENT_SHADER)
	{
	}

	Program::Program() :
		_program(0)
	{
		if (GLEE_VERSION_2_0)
		{
			_program = glCreateProgram();

			if (_program == 0)
				throw object_creation_error("Failed to create a shader program object.");
		}
		else
		{
			throw missing_capabilities("OpenGL 2.0 or newer required for shader operation. NOTE: At this time the GL_ARB_shader_objects extension isn't supported.");
		}
	}

	Program::~Program()
	{
		if (GLEE_VERSION_2_0
		 && _program)
			glDeleteProgram(_program);
	}

	void Program::attachShader(const Shader& shader)
	{
		glAttachShader(_program, shader._shader);
	}

	void Program::detachShader(const Shader& shader)
	{
		glDetachShader(_program, shader._shader);
	}

	void Program::link()
	{
		glLinkProgram(_program);

		GLint success;
		glGetProgramiv(_program, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLint infoLogSize;
			glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogSize);
			boost::scoped_array<GLchar> infoLog(new GLchar[infoLogSize]);
			glGetProgramInfoLog(_program, infoLogSize, NULL, infoLog.get());

			throw shader_source_error("Failed to link shader.", infoLog.get());
		}
	}

	bool Program::validate()
	{
		glValidateProgram(_program);

		GLint success;
		glGetProgramiv(_program, GL_VALIDATE_STATUS, &success);
		if (!success)
		{
			GLint infoLogSize;
			glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogSize);
			boost::scoped_array<GLchar> infoLog(new GLchar[infoLogSize]);
			glGetProgramInfoLog(_program, infoLogSize, NULL, infoLog.get());

			throw shader_source_error("Failed to validate shader.", infoLog.get());
		}

		return success;
	}

	void Program::use()
	{
		glUseProgram(_program);
	}

	void Program::noUse()
	{
		glUseProgram(0);
	}

	GLint Program::getUniformLocation(const char* name)
	{
		const GLint location = glGetUniformLocation(_program, name);

		if (location == -1)
			throw shader_uniform_location_error("Failed to get the location of uniform " + std::string(name), name);

		return location;
	}

	void Program::getUniform(GLint location, GLfloat* params)
	{
		glGetUniformfv(_program, location, params);
	}

	void Program::getUniform(GLint location, GLint* params)
	{
		glGetUniformiv(_program, location, params);
	}

	void Program::uniform(GLint location, GLfloat v0)
	{
		glUniform1f(location, v0);
	}

	void Program::uniform(GLint location, const Eigen::Vector2f& value)
	{
		glUniform2f(location, value[0], value[1]);
	}

	void Program::uniform(GLint location, const Eigen::Vector3f& value)
	{
		glUniform3f(location, value[0], value[1], value[2]);
	}

	void Program::uniform(GLint location, const Eigen::Vector4f& value)
	{
		glUniform4f(location, value[0], value[1], value[2], value[3]);
	}

	void Program::uniform(GLint location, GLint v0)
	{
		glUniform1i(location, v0);
	}

	void Program::uniform(GLint location, GLint v0, GLint v1)
	{
		glUniform2i(location, v0, v1);
	}

	void Program::uniform(GLint location, GLint v0, GLint v1, GLint v2)
	{
		glUniform3i(location, v0, v1, v2);
	}

	void Program::uniform(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
	{
		glUniform4i(location, v0, v1, v2, v3);
	}

	void Program::uniform1v(GLint location, GLsizei count, const GLfloat * value)
	{
		glUniform1fv(location, count, value);
	}

	void Program::uniform2v(GLint location, GLsizei count, const GLfloat * value)
	{
		glUniform2fv(location, count, value);
	}

	void Program::uniform3v(GLint location, GLsizei count, const GLfloat * value)
	{
		glUniform3fv(location, count, value);
	}

	void Program::uniform4v(GLint location, GLsizei count, const GLfloat * value)
	{
		glUniform4fv(location, count, value);
	}

	void Program::uniform1v(GLint location, GLsizei count, const GLint * value)
	{
		glUniform1iv(location, count, value);
	}

	void Program::uniform2v(GLint location, GLsizei count, const GLint * value)
	{
		glUniform2iv(location, count, value);
	}

	void Program::uniform3v(GLint location, GLsizei count, const GLint * value)
	{
		glUniform3iv(location, count, value);
	}

	void Program::uniform4v(GLint location, GLsizei count, const GLint * value)
	{
		glUniform4iv(location, count, value);
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 2, 2>& value)
	{
		glUniformMatrix2fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 3, 3>& value)
	{
		glUniformMatrix3fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 4, 4>& value)
	{
		glUniformMatrix4fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 2, 3>& value)
	{
		glUniformMatrix2x3fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 3, 2>& value)
	{
		glUniformMatrix3x2fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 2, 4>& value)
	{
		glUniformMatrix2x4fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 4, 2>& value)
	{
		glUniformMatrix4x2fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 3, 4>& value)
	{
		glUniformMatrix3x4fv(location, count, transpose, value.data());
	}

	void Program::uniform(GLint location, GLsizei count, GLboolean transpose, const Eigen::Matrix<float, 4, 3>& value)
	{
		glUniformMatrix4x3fv(location, count, transpose, value.data());
	}

	use_scoped_program::use_scoped_program(Program& program) :
		_old_program(currentProgram())
	{
		program.use();
	}

	use_scoped_program::~use_scoped_program()
	{
		glUseProgram(_old_program);
	}

	GLuint use_scoped_program::currentProgram()
	{
		GLint current_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

		return current_program;
	}
}
