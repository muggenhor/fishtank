#ifndef __INCLUDED_MAIN_HPP__
#define __INCLUDED_MAIN_HPP__

/* Exception class providing exit() like support with the added advantage that
 * destructors are run. This unlike exit() where destructors will <em>not</em>
 * be called.
 */
class exit_exception
{
	public:
		inline exit_exception(int code_) :
			_code(code_)
		{
		}

		inline int code() const { return _code; }

	private:
		const int _code;
};

extern bool use_vbos;

#endif // __INCLUDED_MAIN_HPP__
