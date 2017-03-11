/**
@file
Run-time string

@copyright Denis Priyomov 2017
Distributed under the MIT License
(See accompanying file LICENSE or visit https://github.com/cppden/ctstring)
*/

#include <cstring>
#include "string.hpp"

namespace cts {

/*
 * explicit_bzero is from public domain. Written by Matthew Dempsky.
 */
__attribute__((weak)) void __explicit_bzero_hook(void* buf, std::size_t len)
{
}

void explicit_bzero(void* buf, std::size_t len)
{
	std::memset(buf, 0, len);
	__explicit_bzero_hook(buf, len);
}

string::~string()
{
	if (!m_str.empty())
	{
		explicit_bzero(m_str.data(), m_str.size());
	}
}

} //end: namespace cts
