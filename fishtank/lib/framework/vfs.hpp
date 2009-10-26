#ifndef __INCLUDED_LIB_FRAMEWORK_VFS_HPP__
#define __INCLUDED_LIB_FRAMEWORK_VFS_HPP__

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

static const unsigned int SYMLOOP_MAX = 8;

boost::filesystem::path normalized_path(const boost::filesystem::path& path);
bool dir_contains_path(boost::filesystem::path dir, boost::filesystem::path path);
boost::filesystem::path readlink(const boost::filesystem::path& path);

namespace vfs
{
	extern std::vector<boost::filesystem::path> allowed_read_paths, allowed_readwrite_paths;;

	bool allow_readwrite(const boost::filesystem::path& path);
	bool allow_read(const boost::filesystem::path& path);

	boost::shared_ptr<boost::filesystem::ifstream> open_read(const boost::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::in);
	boost::shared_ptr<boost::filesystem::ofstream> open_write(const boost::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::out);
	boost::shared_ptr<boost::filesystem::fstream> open_readwrite(const boost::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::in|std::ios_base::out);

	void open(std::ifstream& is, const boost::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::in);
	void open(std::ofstream& os, const boost::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::out);
	void open(std::fstream& ios, const boost::filesystem::path& path, std::ios_base::openmode mode = std::ios_base::in|std::ios_base::out);
}

#endif // __INCLUDED_LIB_FRAMEWORK_VFS_HPP__
