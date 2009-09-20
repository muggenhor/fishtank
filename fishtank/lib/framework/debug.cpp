#include "debug.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#define foreach BOOST_FOREACH

using namespace boost::posix_time;
namespace po = boost::program_options;

boost::array<bool, LOG_LAST> DebugStream::enabled_debug =
{{
	true, // LOG_ERROR
#ifndef NDEBUG
	true, // LOG_WARNING
#endif
}};

const boost::array<std::string, LOG_LAST> DebugStream::debug_level_names =
{{
	"error",
	"warning",
	"never",
	"main",
	"media",
	"net",
	"rpc",
	"memory",
	"gui",
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
	else if (s == "rpc")
		v = LOG_RPC;
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

boost::mutex DebugStream::_lastDataMutex;
std::string DebugStream::_lastFunction;
std::string DebugStream::_lastMessage;
unsigned int DebugStream::_lastMessageRepeated = 0;
unsigned int DebugStream::_lastMessageNext = 2;
unsigned int DebugStream::_lastMessagePrev = 0;

DebugStream::DebugStream(boost::shared_ptr<std::ostream> os, const code_part part, const char* const function) :
	std::basic_ios<char, std::char_traits<char> >(initCache()->rdbuf()),
	std::ostream(_osCache->rdbuf()),
	_os(os),
	_part(part),
	_function(function),
	_time(microsec_clock::local_time())
{
}

std::ostringstream* DebugStream::initCache()
{
	return _osCache = new std::ostringstream;
}

DebugStream::DebugStream(const DebugStream& rhs) :
	std::basic_ios<char, std::char_traits<char> >(rhs._osCache->rdbuf()),
	std::ostream(rhs._osCache->rdbuf()),
	_osCache(rhs._osCache),
	_os(rhs._os),
	_part(rhs._part),
	_function(rhs._function),
	_time(rhs._time)
{
}

DebugStream::~DebugStream()
{
	/* If this is the last DebugStream instance of the debug() invocation
	 * terminate the line.
	 */
	if (_os.unique())
	{
		// Finish the message and retrieve it from cache
		*_osCache << std::endl;
		std::string message(_osCache->str());
		delete _osCache;

		// Check for repeated messages
		{
			const boost::lock_guard<boost::mutex> lockLastData(_lastDataMutex);

			if (_function == _lastFunction
			 && message == _lastMessage)
			{
				++_lastMessageRepeated;
				if (_lastMessageRepeated == _lastMessageNext)
				{
					writeRepeatMessage();
					_lastMessagePrev = _lastMessageRepeated;
					_lastMessageNext *= 2;
				}

				return;
			}
			else
			{
				/* Current message didn't got repeated, check if the
				 * previous one did (and spam a message if that was the case).
				 */
				if (_lastMessageRepeated
				 && _lastMessageRepeated != _lastMessagePrev)
					writeRepeatMessage();

				_lastMessageRepeated = 0;
				_lastMessageNext = 2;
				_lastMessagePrev = 0;
				_lastFunction = _function;
				_lastMessage = message;
			}
		}

		*_os << boost::format("%-8s (%8s): [%s] %s") % debug_level_names[_part] % _time % _function % message;
	}
}

void DebugStream::writeRepeatMessage()
{
	if (_lastMessageRepeated > 2)
	{
		*_os << boost::format("last message repeated %u times (total %u repeats)\n") % (_lastMessageRepeated - _lastMessagePrev) % _lastMessageRepeated;
	}
	else
	{
		*_os << boost::format("last message repeated %u times\n") % (_lastMessageRepeated - _lastMessagePrev);
	}
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

	return DebugStream(os, part, function);
}
