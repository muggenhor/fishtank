#ifndef __INCLUDED_GLEXCEPT_HPP__
#define __INCLUDED_GLEXCEPT_HPP__

#include <stdexcept>
#include <string>

namespace OpenGL
{
	class missing_capabilities : public std::runtime_error
	{
		public:
			missing_capabilities(const std::string& desc);
			virtual ~missing_capabilities() throw() {}
	};

	class object_creation_error : public std::domain_error
	{
		public:
			object_creation_error(const std::string& desc);
			virtual ~object_creation_error() throw() {}
	};

	class shader_error : public std::domain_error
	{
		public:
			shader_error(const std::string& desc);
			virtual ~shader_error() throw() {}
	};

	class shader_source_error : public shader_error
	{
		public:
			shader_source_error(const std::string& desc, const std::string& infoLog_);
			virtual ~shader_source_error() throw() {}

			virtual const std::string& infoLog() const throw();

		private:
			std::string _infoLog;
	};

	class shader_uniform_location_error : public shader_error
	{
		public:
			shader_uniform_location_error(const std::string& desc, const std::string& uniform_name_);
			virtual ~shader_uniform_location_error() throw() {}

			virtual const std::string& uniform_name() const throw();

		private:
			std::string _uniform_name;
	};
}

#endif // __INCLUDED_GLEXCEPT_HPP__
