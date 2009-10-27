#ifndef __INCLUDED_LIB_FRAMEWORK_TYPE_TRAITS_HPP__
#define __INCLUDED_LIB_FRAMEWORK_TYPE_TRAITS_HPP__

#include <boost/utility/enable_if.hpp>

// Forward declarations
namespace std
{
	template<class charT> class char_traits;
	template<class T> class allocator;

	template <class charT, class traits> class basic_ios;
	template <class charT, class traits> class basic_istream;
	template <class charT, class traits> class basic_ostream;
	template <class charT, class traits> class basic_iostream;
	template <class charT, class traits, class Allocator> class basic_stringbuf;
	template <class charT, class traits, class Allocator> class basic_istringstream;
	template <class charT, class traits, class Allocator> class basic_ostringstream;
	template <class charT, class traits, class Allocator> class basic_stringstream;
	template <class charT, class traits> class basic_filebuf;
	template <class charT, class traits> class basic_ifstream;
	template <class charT, class traits> class basic_ofstream;
	template <class charT, class traits> class basic_fstream;
}

namespace boost
{
	namespace filesystem
	{
		template <class charT, class traits> class basic_ifstream;
		template <class charT, class traits> class basic_ofstream;
		template <class charT, class traits> class basic_fstream;
	}
}

template<class Fstream> struct is_std_fstream
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_std_fstream< std::basic_ifstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };
template<class charT, class traits> struct is_std_fstream< std::basic_ofstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };
template<class charT, class traits> struct is_std_fstream< std::basic_fstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };

template<class Fstream> struct is_fs_fstream
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_fs_fstream< boost::filesystem::basic_ifstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };
template<class charT, class traits> struct is_fs_fstream< boost::filesystem::basic_ofstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };
template<class charT, class traits> struct is_fs_fstream< boost::filesystem::basic_fstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };

template<class Stream> struct is_fstream
  { BOOST_STATIC_CONSTANT( bool, value = is_std_fstream<Stream>::value || is_fs_fstream<Stream>::value ); };

template<class Stream> struct is_stream
  { BOOST_STATIC_CONSTANT( bool, value = is_fstream<Stream>::value ); };
template<class charT, class traits> struct is_stream< std::basic_ios<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };
template<class charT, class traits> struct is_stream< std::basic_istream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };
template<class charT, class traits> struct is_stream< std::basic_ostream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };
template<class charT, class traits> struct is_stream< std::basic_iostream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = true ); };

template<class Stream> struct is_istream
  { BOOST_STATIC_CONSTANT( bool, value = is_stream<Stream>::value ); };
template<class charT, class traits> struct is_istream< std::basic_ios<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_istream< std::basic_ostream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_istream< boost::filesystem::basic_ofstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_istream< std::basic_ofstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };

template<class Stream> struct is_ostream
  { BOOST_STATIC_CONSTANT( bool, value = is_stream<Stream>::value ); };
template<class charT, class traits> struct is_ostream< std::basic_ios<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_ostream< std::basic_istream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_ostream< boost::filesystem::basic_ifstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };
template<class charT, class traits> struct is_ostream< std::basic_ifstream<charT, traits> >
  { BOOST_STATIC_CONSTANT( bool, value = false ); };

template<class Stream> struct is_iostream
  { BOOST_STATIC_CONSTANT( bool, value = is_istream<Stream>::value || is_ostream<Stream>::value ); };

#endif // __INCLUDED_LIB_FRAMEWORK_TYPE_TRAITS_HPP__
