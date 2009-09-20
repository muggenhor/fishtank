#include "debug-net-out.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using boost::asio::ip::tcp;
namespace po = boost::program_options;

void DebugNetOutputStream::addCommandLineOptions(boost::program_options::options_description& desc)
{
	po::options_description debugOps("Net Debug Options");
	debugOps.add_options()
		("debug-port", po::value<unsigned short>(), "Listen for incoming telnet connections on this port. Then replicate all debug output on those connections.")
	;

	desc.add(debugOps);
}

namespace
{
void acceptConnection(shared_ptr<tcp::acceptor> acceptor, shared_ptr<tcp::iostream> socket, const boost::system::error_code& error);

void startAccept(shared_ptr<tcp::acceptor> acceptor)
{
	shared_ptr<tcp::iostream> socket(new tcp::iostream);

	acceptor->async_accept(*socket->rdbuf(),
		boost::bind(&acceptConnection, acceptor, socket, boost::asio::placeholders::error));
}

void acceptConnection(shared_ptr<tcp::acceptor> acceptor, shared_ptr<tcp::iostream> socket, const boost::system::error_code& error)
{
	if (error)
	{
		debug(LOG_ERROR) << error;
		return;
	}

	startAccept(acceptor);
	DebugStream::registerDebugOutput(socket);
}
}

void DebugNetOutputStream::processOptions(boost::asio::io_service& io_svc, const boost::program_options::variables_map& options)
{
	if (options.count("debug-port"))
	{
		const unsigned short port = options["debug-port"].as<unsigned short>();

		shared_ptr<tcp::acceptor> acceptor(new tcp::acceptor(io_svc, tcp::endpoint(tcp::v4(), port)));
		startAccept(acceptor);
	}
}
