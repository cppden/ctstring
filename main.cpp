#include <cstdio>
#include <iostream>

#include "ctstring.hpp"

int main(int argc, char **argv)
{
	using namespace cts::literals;

	auto sample1 = "Hello"_chars;
	auto sample2 = "Hello"_ichars;

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

	auto const xsHidden1 = "XOR: this shouldn't be seen in binary!"_xchars;
	std::cout << "xored 1: " << xsHidden1.str().c_str() << std::endl;

	return 0;
}
