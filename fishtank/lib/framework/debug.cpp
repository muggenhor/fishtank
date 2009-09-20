#include "debug.hpp"
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#define foreach BOOST_FOREACH

using namespace boost::posix_time;
using namespace std;
namespace po = boost::program_options;

boost::array<bool, LOG_LAST> DebugStream::enabled_debug =
{{
	true, // LOG_ERROR
#ifndef NDEBUG
	true, // LOG_WARNING
#endif
}};

// NOTE: Order and contents of this array *must* be the same as code_part up until LOG_LAST
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

	if (s == "all")
	{
		v = LOG_ALL;
		return;
	}

	const boost::array<std::string, LOG_LAST>::const_iterator
	  log_part = find(
	    DebugStream::debug_level_names.begin(),
	    DebugStream::debug_level_names.end(),
	    s
	  );

	if (log_part == DebugStream::debug_level_names.end())
		throw po::validation_error("invalid value");

	v = static_cast<code_part>(distance(
	  DebugStream::debug_level_names.begin(),
	  log_part
	));
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

std::ostringstream* DebugStream::initCache()
{
	return _osSafeCacheInit = new std::ostringstream;
}

DebugStream::DebugStream(const DebugStream& rhs) :
	std::basic_ios<char, std::char_traits<char> >(rhs._osCache->rdbuf()),
	std::ostream(rhs._osCache->rdbuf()),
	_osCache(rhs._osCache),
	_streams(rhs._streams),
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
	if (_osCache.unique())
	{
		// Finish the message and retrieve it from cache
		std::string message(_osCache->str());

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

		foreach (const boost::shared_ptr<std::ostream>& stream, _streams)
			*stream << boost::format("%-8s (%8s): [%s] %s") % debug_level_names[_part] % _time % _function % message << endl;
	}
}

void DebugStream::writeRepeatMessage()
{
	string repeatMessage;

	if (_lastMessageRepeated > 2)
	{
		repeatMessage = (boost::format("last message repeated %u times (total %u repeats)") % (_lastMessageRepeated - _lastMessagePrev) % _lastMessageRepeated).str();
	}
	else
	{
		repeatMessage = (boost::format("last message repeated %u times") % (_lastMessageRepeated - _lastMessagePrev)).str();
	}

	foreach (const boost::shared_ptr<std::ostream>& stream, _streams)
		*stream << repeatMessage << endl;
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

	std::vector< boost::shared_ptr<std::ostream> > os;
	if (DebugStream::enabled_debug[part])
		os.push_back(boost::shared_ptr<std::ostream>(new DebugStream::stderr_wrapper));

	return DebugStream(os.begin(), os.end(), part, function);
}
