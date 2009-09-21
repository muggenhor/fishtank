#include "threadpool.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

void create_thread_pool(boost::asio::io_service& io_service, boost::thread_group& threads, std::size_t count)
{
	for (; count; --count)
		threads.create_thread(boost::bind(&boost::asio::io_service::run, boost::ref(io_service)));
}
