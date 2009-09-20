#include "debug.hpp"
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <cassert>
#include <fstream>
#include <iostream>

#define foreach BOOST_FOREACH

namespace po = boost::program_options;

boost::array<bool, LOG_LAST> DebugStream::enabled_debug =
{{
	true, // LOG_ERROR
#ifndef NDEBUG
	true, // LOG_WARNING
#endif
}};

static void validate(boost::any& v, const std::vector<std::string>& values, code_part*, int)
{
	// Make sure no previous assignment to 'a' was made.
	po::validators::check_first_occurrence(v);
	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const std::string& s = po::validators::get_single_string(values);

	if      (s == "error")
		v = LOG_ERROR;
	else if (s == "warning")
		v = LOG_WARNING;
	else if (s == "never")
		v = LOG_NEVER;
	else if (s == "main")
		v = LOG_MAIN;
	else if (s == "media")
		v = LOG_MEDIA;
	else if (s == "net")
		v = LOG_NET;
	else if (s == "memory")
		v = LOG_MEMORY;
	else if (s == "gui")
		v = LOG_GUI;
	else if (s == "all")
		v = LOG_ALL;
	else
		throw po::validation_error("invalid value");
}

void DebugStream::addCommandLineOptions(boost::program_options::options_description& desc)
{
	desc.add_options()
		("debug", po::value< std::vector<code_part> >(), "Enable debug messages for given level")
	;
}

void DebugStream::processOptions(const boost::program_options::variables_map& options)
{
	if (options.count("debug") == 0)
		return;

	foreach (const code_part& part, options["debug"].as< std::vector<code_part> >())
	{
		if (part == LOG_ALL)
		{
			foreach (bool& log, enabled_debug)
				log = true;
			continue;
		}

		enabled_debug[part] = true;
	}
}

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

DebugStream _debug(const code_part part, const char* const function)
{
	assert(part < LOG_LAST && "debug part out of range");

	boost::shared_ptr<std::ostream> os;
	if (DebugStream::enabled_debug[part])
		os.reset(new DebugStream::stderr_wrapper);
	else
		os.reset(new std::ofstream("/dev/null"));

	DebugStream ds(os);

	ds << "[" << function << "] ";

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

		case LOG_RPC:
			ds << "rpc   : ";
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
