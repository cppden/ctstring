#include <cstdio>
#include <iostream>

#include "ctstring.hpp"

int main(int argc, char **argv)
{
	using namespace cts::literals;

	auto sample1 = "Hello"_chars;
	auto sample2 = "Hello"_ichars;

	//std::printf("%s = %u:%u:%u\n", __TIME__, CTS_TIME(0), CTS_TIME(3), CTS_TIME(6));
	std::printf("plain 1: %s\n", sample1.c_str());
	std::printf("plain 2: %s\n", sample2.c_str());

	{
		char constexpr csz[] = "Hello World";
		static_assert(sample1.match(csz, csz+sizeof(csz)), "");
		static_assert(sample2.match(csz, csz+sizeof(csz)), "");
	}
	{
		char constexpr csz[] = "hello world";
		static_assert(!sample1.match(csz, csz+sizeof(csz)), "");
		static_assert(sample2.match(csz, csz+sizeof(csz)), "");
	}

	auto sample3 = "HELLO"_chars;
	{
		char constexpr csz[] = "helLO worLD";
		static_assert(!sample3.match(csz, csz+sizeof(csz)), "");
		static_assert(sample3.match(csz, csz+sizeof(csz), 3), "");
		static_assert(!sample3.match(csz, csz+sizeof(csz), 5), "");
	}

	auto const xsHidden = "FAREWELL: THIS STRING SHOULD NOT BE SEEN IN BINARY EITHER FULL OR IN PIECES!"_xchars;
	std::cout << "xored: " << xsHidden.str() << std::endl;

	std::size_t buf[xsHidden.size()+1];
	std::printf("in-buf(xored): %s\n", xsHidden.str(buf));

	return 0;
}
