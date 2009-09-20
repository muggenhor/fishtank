#ifndef __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__
#define __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <ostream>

enum code_part
{
	LOG_ERROR, /**< special; on by default. _must_ be first! */
	LOG_WARNING, /**< special; on in debug mode. _must_ be second! */
	LOG_NEVER, /**< if too verbose for anything but dedicated debugging... */
	LOG_MAIN,
	LOG_MEDIA,
	LOG_NET,
	LOG_MEMORY,
	LOG_GUI,

	LOG_LAST /**< _must_ be last! */
};

class DebugStream : public std::ostream
{
	public:
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
		 * Construction is only allowed through the @c debug(code_part)
		 * function.
		 */
		DebugStream(boost::shared_ptr<std::ostream> os);

		friend DebugStream debug(code_part);

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
		static bool enabled_debug[LOG_LAST];

	private:
		boost::shared_ptr<std::ostream> _os;
};

DebugStream debug(code_part part);

#endif // __INCLUDED_LIB_FRAMEWORK_DEBUG_HPP__
