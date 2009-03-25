#ifndef __INCLUDED_MATH_HELPERS_HPP__
#define __INCLUDED_MATH_HELPERS_HPP__

template <typename T>
T clip(T const& value, T const& min, T const& max)
{
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

#endif // __INCLUDED_MATH_HELPERS_HPP__
