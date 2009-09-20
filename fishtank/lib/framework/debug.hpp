#ifndef __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__
#define __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ostream>
#include <sstream>
#include <string>

namespace boost { namespace program_options {
class options_description;
class variables_map;
}}

enum code_part
{
	LOG_ERROR, /**< special; on by default. _must_ be first! */
	LOG_WARNING, /**< special; on in debug mode. _must_ be second! */
	LOG_NEVER, /**< if too verbose for anything but dedicated debugging... */
	LOG_MAIN,
	LOG_MEDIA,
	LOG_NET,
	LOG_RPC,
	LOG_MEMORY,
	LOG_GUI,

	LOG_LAST, /**< _must_ be the last valid log level! */

	LOG_ALL, /**< special; used to enable all debug levels */
};

class DebugStream : public std::ostream
{
	public:
		static void addCommandLineOptions(boost::program_options::options_description& desc);

		static void processOptions(const boost::program_options::variables_map& options);

		DebugStream(const DebugStream& rhs);
		virtual ~DebugStream();

		template <typename Printable>
		DebugStream operator<<(const Printable& printable)
		{
			*_osCache << printable;
			return *this;
		}

	private:
		/**
		 * Construction is only allowed through the @c _debug(code_part, const char*)
		 * function.
		 */
		DebugStream(boost::shared_ptr<std::ostream> os, code_part part, const char* function);

		friend DebugStream _debug(code_part, const char*);

	private:
		std::ostringstream* initCache();
		void writeRepeatMessage();

	private:
		/** 
		 * This class implements a C++ ostream object wrapping stderr.
		 */
		class stderr_wrapper : public std::ostream
		{
			public:
				stderr_wrapper();

			private:
				static boost::mutex                     _global_mutex;
				const boost::lock_guard<boost::mutex>   _lock;
		};

	private:
		static boost::array<bool, LOG_LAST> enabled_debug;
		static const boost::array<std::string, LOG_LAST> debug_level_names;

	private:
		std::ostringstream* _osCache;
		boost::shared_ptr<std::ostream> _os;

		const code_part _part;
		const std::string _function;
		boost::posix_time::ptime _time;

		static boost::mutex _lastDataMutex;
		static std::string _lastFunction;
		static std::string _lastMessage;
		static unsigned int _lastMessageRepeated;
		static unsigned int _lastMessageNext;
		static unsigned int _lastMessagePrev;
};

/**
 * Output iostream style str.
 *
 * Only outputs if debugging of part was formerly enabled in DebugStream::enabled_debug.
 */
#define debug(part) _debug(part, __FUNCTION__)
DebugStream _debug(code_part part, const char* function);

#endif // __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__
