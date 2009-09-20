#ifndef __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__
#define __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <ostream>

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
			*_os << printable;
			return *this;
		}

	private:
		/**
		 * Construction is only allowed through the @c _debug(code_part, const char*)
		 * function.
		 */
		DebugStream(boost::shared_ptr<std::ostream> os);

		friend DebugStream _debug(code_part, const char*);

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

	private:
		boost::shared_ptr<std::ostream> _os;
};

/**
 * Output iostream style str.
 *
 * Only outputs if debugging of part was formerly enabled in DebugStream::enabled_debug.
 */
#define debug(part) _debug(part, __FUNCTION__)
DebugStream _debug(code_part part, const char* function);

#endif // __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__
