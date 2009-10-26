#include "vfs.hpp"
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <cerrno>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
namespace fs = boost::filesystem;
using boost::format;
using boost::system::error_code;
using boost::system::error_condition;
using boost::system::get_system_category;
using boost::system::system_error;

#define too_many_symbolic_link_levels too_many_synbolic_link_levels
static const error_condition loop_condition(
	boost::system::errc::too_many_symbolic_link_levels,
	get_system_category());
#undef too_many_symbolic_link_levels

boost::filesystem::path normalized_path(const boost::filesystem::path& path)
{
	if (path.empty())
		return path;

	fs::path temp;
	fs::path::iterator start( path.begin() );
	fs::path::iterator last( path.end() );
	fs::path::iterator stop( last-- );
	for ( fs::path::iterator itr( start ); itr != stop; ++itr )
	{
		if (*itr == "."
		 && itr != start
		 && itr != last)
			continue;

		// ignore a name and following ".."
		if (!temp.empty()
		 && *itr == "..")
		{
			string lf( temp.filename() );  
			if (!lf.empty()
			 && lf != "."
			 && lf != "/"
#             ifdef BOOST_WINDOWS_PATH
			 && !(lf.size() == 2 
			   && lf[1] == ':')
#             endif
			 && lf != "..")
			{
				temp.remove_filename();
				continue;
			}
		}

		temp /= *itr;
		if (itr != last
		 && temp.has_root_path()
		 && fs::is_symlink(temp))
		{
			temp = normalized_path(readlink(temp));
			if (temp.filename() == ".")
				temp = temp.parent_path();
		}
	}

	if (temp.empty())
		temp /= ".";

	return temp;
}

bool dir_contains_path(boost::filesystem::path dir, boost::filesystem::path path)
{
	dir  = normalized_path(fs::system_complete(dir));
	path = normalized_path(fs::system_complete(path));

	for (fs::path::const_iterator diri(dir.begin()), pathi(path.begin());
	     diri != dir.end();
	     ++diri, ++pathi)
		if (pathi == path.end()
		 || *diri != *pathi)
			return false;

	return true;
}

boost::filesystem::path readlink(const boost::filesystem::path&
#ifndef BOOST_WINDOWS_API
		path
#endif
		)
{
#ifdef BOOST_WINDOWS_API
	throw std::runtime_error("readlink() not implemented on Windows");
#else
	string link(path.external_file_string());
	vector<char> buf(512);

	for (;;)
	{
		const ssize_t len = readlink(link.c_str(), &buf[0], buf.size());
		if (len == -1)
		{
			if (errno == ENAMETOOLONG)
			{
				buf.resize(2 * buf.size());
				continue;
			}

			throw system_error(error_code(errno, get_system_category()));
		}

		return path.parent_path() / string(&buf[0], len);
	}
#endif
}

namespace vfs
{

vector<boost::filesystem::path> allowed_read_paths, allowed_readwrite_paths;

static bool allow_readwrite(const boost::filesystem::path& path, const unsigned int recursion)
{
	if (recursion > SYMLOOP_MAX)
		throw system_error(error_code(ELOOP, get_system_category()));

	const bool symlink = fs::is_symlink(path);

	for (vector<fs::path>::const_iterator
	     dir  = allowed_readwrite_paths.begin();
	     dir != allowed_readwrite_paths.end();
	     ++dir)
		if (dir_contains_path(*dir, path))
			return symlink ?
				allow_readwrite(readlink(path), recursion + 1) :
				true;

	for (vector<fs::path>::const_iterator
	     dir  = allowed_read_paths.begin();
	     dir != allowed_read_paths.end();
	     ++dir)
		if (dir_contains_path(*dir, path))
			return symlink ?
				allow_readwrite(readlink(path), recursion + 1) :
				true;

	return false;
}

bool allow_readwrite(const boost::filesystem::path& path)
{
	try
	{
		return allow_readwrite(path, 0);
	}
	catch (const system_error& e)
	{
		throw fs::filesystem_error(e.std::runtime_error::what(), path, e.code());
	}
}

static bool allow_read(const boost::filesystem::path& path, const unsigned int recursion)
{
	if (recursion > SYMLOOP_MAX)
		throw system_error(error_code(ELOOP, get_system_category()));

	const bool symlink = fs::is_symlink(path);

	for (vector<fs::path>::const_iterator
	     dir  = allowed_read_paths.begin();
	     dir != allowed_read_paths.end();
	     ++dir)
		if (dir_contains_path(*dir, path))
			return symlink ?
				allow_read(readlink(path), recursion + 1) :
				true;

	return false;
}

bool allow_read(const boost::filesystem::path& path)
{
	try
	{
		return allow_read(path, 0);
	}
	catch (const system_error& e)
	{
		throw fs::filesystem_error(e.std::runtime_error::what(), path, e.code());
	}
}

boost::shared_ptr<boost::filesystem::ifstream> open_read(const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
	boost::shared_ptr<fs::ifstream> is(new fs::ifstream);
	open(*is, path, mode);
	return is;
}

boost::shared_ptr<boost::filesystem::ofstream> open_write(const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
	boost::shared_ptr<fs::ofstream> os(new fs::ofstream);
	open(*os, path, mode);
	return os;
}

boost::shared_ptr<boost::filesystem::fstream> open_readwrite(const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
	boost::shared_ptr<fs::fstream> os(new fs::fstream);
	open(*os, path, mode);
	return os;
}

void open(std::ifstream& is, const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
	try
	{
		if (!allow_read(path))
			throw system_error(error_code(EACCES, get_system_category()));

		string file(path.external_file_string());
		is.open(file.c_str(), mode);
		if (!is.is_open())
			throw system_error(error_code(errno, get_system_category()));
	}
	catch (const system_error& e)
	{
		string what(e.std::runtime_error::what());
		if (!what.empty())
			what = ": " + what;
		what = "Cannot open file for reading" + what;

		throw fs::filesystem_error(what, path, e.code());
	}
}

void open(std::ofstream& os, const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
	try
	{
		if (!allow_readwrite(path))
			throw system_error(error_code(EACCES, get_system_category()));

		string file(path.external_file_string());
		os.open(file.c_str(), mode);
		if (!os.is_open())
			throw system_error(error_code(errno, get_system_category()));
	}
	catch (const system_error& e)
	{
		string what(e.std::runtime_error::what());
		if (!what.empty())
			what = ": " + what;
		what = "Cannot open file for writing" + what;

		throw fs::filesystem_error(what, path, e.code());
	}
}

void open(std::fstream& ios, const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
	try
	{
		if (!allow_readwrite(path))
			throw system_error(error_code(EACCES, get_system_category()));

		string file(path.external_file_string());
		ios.open(file.c_str(), mode);
		if (!ios.is_open())
			throw system_error(error_code(errno, get_system_category()));
	}
	catch (const system_error& e)
	{
		string what(e.std::runtime_error::what());
		if (!what.empty())
			what = ": " + what;
		what = "Cannot open file for updating" + what;

		throw fs::filesystem_error(what, path, e.code());
	}
}

}
