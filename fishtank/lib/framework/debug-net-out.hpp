#ifndef __INCLUDED_DEBUG_NET_OUT_HPP__
#define __INCLUDED_DEBUG_NET_OUT_HPP__

#include "debug.hpp"

namespace boost {
	namespace program_options {
	class options_description;
	class variables_map;
	}
	namespace asio {
	class io_service;
	}
}

namespace boost { }

class DebugNetOutputStream
{
	public:
		static void addCommandLineOptions(boost::program_options::options_description& desc);

		static void processOptions(boost::asio::io_service& io_svc, const boost::program_options::variables_map& options);
};

#endif // __INCLUDED_DEBUG_NET_OUT_HPP__
