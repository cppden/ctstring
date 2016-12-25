#include <cstdio>
#include <iostream>

#include "ctstring.hpp"

int main(int argc, char **argv)
{
	//using namespace cts;
	auto sample1 = "Hello"_chars;
	auto sample2 = "Hello"_casechars;

	std::printf("1: %s\n", sample1.c_str());
	std::printf("2: %s\n", sample2.c_str());

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

	return 0;
}
