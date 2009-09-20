#include "debug.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

bool DebugStream::enabled_debug[LOG_LAST] =
{
	true, // LOG_ERROR
#ifndef NDEBUG
	true, // LOG_WARNING
#endif
	// TODO: Remove this, as it enables *all* debug levels
	true,
	true,
	true,
	true,
	true,
	true,
};

DebugStream::DebugStream(boost::shared_ptr<std::ostream> os) :
	std::basic_ios<char, std::char_traits<char> >(os->rdbuf()),
	std::ostream(os->rdbuf()),
	_os(os)
{
}

DebugStream::DebugStream(const DebugStream& rhs) :
	std::basic_ios<char, std::char_traits<char> >(rhs._os->rdbuf()),
	std::ostream(rhs._os->rdbuf()),
	_os(rhs._os)
{
}

DebugStream::~DebugStream()
{
	/* If this is the last DebugStream instance of the debug() invocation
	 * terminate the line.
	 */
	if (_os.unique())
		*_os << std::endl;
}

boost::mutex DebugStream::stderr_wrapper::_global_mutex;

DebugStream::stderr_wrapper::stderr_wrapper() :
	std::basic_ios<char, std::char_traits<char> >(std::cerr.rdbuf()),
	std::ostream(std::cerr.rdbuf()),
	_lock(_global_mutex)
{
}

DebugStream debug(const code_part part)
{
	assert(part < LOG_LAST && "debug part out of range");

	boost::shared_ptr<std::ostream> os;
	if (DebugStream::enabled_debug[part])
		os.reset(new DebugStream::stderr_wrapper);
	else
		os.reset(new std::ofstream("/dev/null"));

	DebugStream ds(os);

	switch (part)
	{
		case LOG_ERROR:
			ds << "error  : ";
			break;

		case LOG_WARNING:
			ds << "warning: ";
			break;

		case LOG_NEVER:
			ds << "never  : ";
			break;

		case LOG_MAIN:
			ds << "main   : ";
			break;

		case LOG_MEDIA:
			ds << "media : ";
			break;

		case LOG_NET:
			ds << "net   : ";
			break;

		case LOG_MEMORY:
			ds << "memory: ";
			break;

		case LOG_GUI:
			ds << "gui   : ";
			break;

		default:
			assert(!"debug part out of range");
	}

	return ds;
}
