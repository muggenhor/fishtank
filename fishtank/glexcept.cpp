#include "glexcept.hpp"

namespace OpenGL
{
	missing_capabilities::missing_capabilities(const std::string& desc) :
		std::runtime_error(desc)
	{
	}

	object_creation_error::object_creation_error(const std::string& desc) :
		std::domain_error(desc)
	{
	}

	shader_error::shader_error(const std::string& desc) :
		std::domain_error(desc)
	{
	}

	shader_source_error::shader_source_error(const std::string& desc, const std::string& infoLog_) :
		shader_error(desc),
		_infoLog(infoLog_)
	{
	}

	const std::string& shader_source_error::infoLog() const throw()
	{
		return _infoLog;
	}

	shader_uniform_location_error::shader_uniform_location_error(const std::string& desc, const std::string& uniform_name_) :
		shader_error(desc),
		_uniform_name(uniform_name_)
	{
	}

	const std::string& shader_uniform_location_error::uniform_name() const throw()
	{
		return _uniform_name;
	}
}
