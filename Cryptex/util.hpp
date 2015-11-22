#ifndef UTIL_H
#define UTIL_H

#include <type_traits>

/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4380.html
template<typename T>
inline typename std::add_const<T>::type& as_const(T &t) noexcept
{
	return t;
}

#endif // UTIL_H
