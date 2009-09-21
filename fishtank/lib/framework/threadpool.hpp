#ifndef __INCLUDED_THREADPOOL_HPP__
#define __INCLUDED_THREADPOOL_HPP__

#include <cstddef> // for size_t

namespace boost {
class thread_group;

namespace asio {
class io_service;
}}

/**
 * Create a pool of @c count threads to handle events on the given @c
 * io_service.
 *
 * @param io_service The asynchronous I/O service to handle events for with the
 *                   threads to create.
 * @param threads    Thread group to put the newly created threads in.
 * @param count      The amount of threads to create.
 *
 * @note The given threads will terminate immediately when io_service.run()
 *       returns. It is thus the duty of the calling code to make sure that
 *       io_service.run() only returns when the threads aren't required
 *       anymore. E.g.  the caller could use boost::asio::io_service::work for
 *       this purpose.
 */
void create_thread_pool(boost::asio::io_service& io_service, boost::thread_group& threads, std::size_t count);

#endif // __INCLUDED_THREADPOOL_HPP__
