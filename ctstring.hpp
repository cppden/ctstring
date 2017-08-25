/**
@file
Compile-time string

@copyright Denis Priyomov 2017
Distributed under the MIT License
(See accompanying file LICENSE or visit https://github.com/cppden/ctstring)
*/

#include <cstdint>
#include <utility>
#include <type_traits>

#include "string.hpp"

namespace cts {

constexpr char to_upper(char const c)
{
	return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

constexpr char to_lower(char const c)
{
	return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

constexpr bool is_equal(char const c1, char const c2)
{
	return c1 == c2;
}

constexpr bool is_iequal(char const c1, char const c2)
{
	return c1 == c2 || to_upper(c1) == c2;
}


namespace {

template <char... CHARS>
using chars = std::integer_sequence<char, CHARS...>;
template <std::size_t... INDEXES>
using indexes = std::index_sequence<INDEXES...>;

template <class... ICs>
struct sequence {};

template <std::size_t I, std::size_t C>
struct ic //index + char
{
	static constexpr std::size_t my_index = I;
	static constexpr std::size_t my_char = C;
};

template <class Map, std::size_t ...Is>
struct zip
{
	template <char ...Cs>
	struct with
	{
		using type = sequence<ic<Is, Map::apply(Cs, Is)>...>;
	};
};

struct plain
{
	static constexpr std::size_t apply(std::size_t c, std::size_t)
	{
		return c;
	}
};

template <class Map, std::size_t... Is, char... Cs>
constexpr auto map_index(indexes<Is...>, chars<Cs...>) -> typename zip<Map, Is...>::template with<Cs...>::type;

template <char... Cs>
using indexed_chars = decltype( map_index<plain>(std::make_index_sequence<sizeof...(Cs)>{}, chars<Cs...>{}) );

//TODO: replace with fold expression in c++17
template <class FUNC>
constexpr bool apply_impl(FUNC&& func, char const* p) { return true; }

template <class FUNC, class IC, class... ICs>
constexpr bool apply_impl(FUNC&& func, char const* p)
{
	return func(p[IC::my_index], IC::my_char) && apply_impl<FUNC, ICs...>(std::forward<FUNC>(func), p);
}

template <class FUNC, class... ICs>
constexpr bool apply(FUNC&& func, char const* p, sequence<ICs...>)
{
	return apply_impl<FUNC, ICs...>(std::forward<FUNC>(func), p);
}

template <class FUNC>
constexpr bool apply_impl(FUNC&& func, char const* p, std::size_t offset) { return true; }

template <class FUNC, class IC, class... ICs>
constexpr bool apply_impl(FUNC&& func, char const* p, std::size_t offset)
{
	if (offset)
	{
		return apply_impl<FUNC, ICs...>(std::forward<FUNC>(func), p, offset - 1);
	}
	else
	{
		return func(p[IC::my_index], IC::my_char) && apply_impl<FUNC, ICs...>(std::forward<FUNC>(func), p);
	}
}

template <class FUNC, class... ICs>
constexpr bool apply(FUNC&& func, char const* p, sequence<ICs...>, std::size_t offset)
{
	return apply_impl<FUNC, ICs...>(std::forward<FUNC>(func), p, offset);
}

} //end: namespace


template <char... CHARS>
class Chars
{
public:
	static constexpr std::size_t length() { return sizeof...(CHARS); }
	using type = indexed_chars<CHARS...>;

	constexpr bool match(char const* begin, char const* end) const
	{
		return (begin + length() <= end)
			? apply(is_equal, begin, type{})
			: false;
	}

	constexpr bool match(char const* begin, char const* end, std::size_t offset) const
	{
		return (begin + length() <= end && offset < length())
			? apply(is_equal, begin, type{}, offset)
			: false;
	}

	static constexpr char const* c_str()
	{
		return m_string;
	}

private:
	static constexpr char m_string[] = {CHARS..., 0};
};

template <char... CHARS>
constexpr char Chars<CHARS...>::m_string[];

template <char... CHARS>
class CaseChars
{
public:
	static constexpr std::size_t length() { return sizeof...(CHARS); }
	using type = indexed_chars<CHARS...>;

	constexpr bool match(char const* begin, char const* end) const
	{
		return (begin + length() <= end)
			? apply(is_iequal, begin, type{})
			: false;
	}

	constexpr bool match(char const* begin, char const* end, std::size_t offset) const
	{
		return (begin + length() <= end && offset < length())
			? apply(is_iequal, begin, type{}, offset)
			: false;
	}

	static constexpr char const* c_str()
	{
		return m_string;
	}

private:
	static constexpr char m_string[] = {to_upper(CHARS)..., 0};
};

template <char... CHARS>
constexpr char CaseChars<CHARS...>::m_string[];


namespace {

#ifndef CTS_RND_SEED
#define CTS_TIME(index) ((__TIME__[index]-'0')*10 + (__TIME__[index+1]-'0'))
#define CTS_RND_SEED uint32_t(CTS_TIME(0)*3600 + CTS_TIME(3)*60 + CTS_TIME(6) + 13709*(31+ __COUNTER__))
#endif

constexpr std::size_t STATE_OFFSET = 137;
constexpr std::size_t CHAR_SHIFT_MOD = 7;

//https://en.wikipedia.org/wiki/Linear_congruential_generator
//LCG: X(n + 1) = (A * X(n) + C) % m
constexpr std::size_t rand(std::size_t n)
{
	//NOTE: cast makes the mod 2^32
	return static_cast<uint32_t>(1664525u * (n ? rand(n - 1) : CTS_RND_SEED) + 1013904223u);
}

constexpr std::size_t rand_next(std::size_t rand_curr)
{
	//NOTE: cast makes the mod 2^32
	return static_cast<uint32_t>(1664525u * rand_curr + 1013904223u);
}
/*
	a+b = (a ^ b) + 2*(a | b)
	a-b = (a ^ b) - 2*(~a | b)
	a*a(a+1)^2 % 4 = 0
	(a*a*a -3) % 3 = 0
	a + b >= a ^ b
	7*a*a - 1 != b*b
*/

struct xmap
{
	static constexpr std::size_t apply(std::size_t c, std::size_t index)
	{
		return ((c + index) << (index % CHAR_SHIFT_MOD)) ^ rand(index + STATE_OFFSET);
	}
};

//__attribute__((noinline))
#ifdef __clang__
__attribute__((clang:optnone))
#else
__attribute__((optimize(0)))
#endif
inline std::size_t decode(std::size_t c, std::size_t index, std::size_t state)
{
	return ((c ^ state) >> (index % CHAR_SHIFT_MOD)) - index;
}

template <char... Cs>
using xchars = decltype( map_index<xmap>(std::make_index_sequence<sizeof...(Cs)>{}, chars<Cs...>{}) );

//[[gnu::visibility("hidden")]]

template <std::size_t STATE>
constexpr void ximpl(char* p) { }

template <std::size_t STATE, class IC, class... ICs>
__attribute__((always_inline, visibility("internal")))
inline void ximpl(char* p)
{
	p[IC::my_index] = static_cast<char>(decode(IC::my_char, IC::my_index, STATE));
	ximpl<rand_next(STATE), ICs...>(p);
}

template <class... ICs>
__attribute__((always_inline, visibility("internal")))
inline void xapply(char* p, sequence<ICs...>)
{
	return ximpl<rand(STATE_OFFSET), ICs...>(p);
}

} //end: namespace


//obfuscated string via XOR
template <char... CHARS>
class XChars
{
	using type = xchars<CHARS...>;

public:
	static constexpr std::size_t size()                  { return sizeof...(CHARS); }
	static constexpr std::size_t length()                { return size(); }

	//de-obfuscate at run-time into string on heap which will clean up its data in dtor
	__attribute__((always_inline, visibility("internal")))
	string str() const
	{
		string result{ this->size() };
		xapply(result.data(), type{});
		return result;
	}

	//de-obfuscate at run-time avoiding intermediate copy
	//but you should clean-up buffer once it's used and no longer needed to erase protected data from memory
	__attribute__((always_inline, visibility("internal")))
	char* str(void* buffer, std::size_t buf_size) const
	{
		if (buf_size >= size())
		{
			auto* p = static_cast<char*>(buffer);
			xapply(p, type{});
			if (buf_size > size()) //NULL terminate if space allows
			{
				p[size()] = '\0';
			}
			return p;
		}
		return nullptr;
	}

	template <typename T, std::size_t QTY>
	__attribute__((always_inline, visibility("internal")))
	char* str(T (&buffer)[QTY]) const
	{
		static_assert(sizeof(buffer) >= size(), "buffer is too small to fit the string");
		return str(buffer, sizeof(buffer));
	}
};


namespace literals {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template <typename T, T... CHARS>
constexpr cts::Chars<CHARS...> operator""_chars() { return { }; }

template <typename T, T... CHARS>
constexpr cts::CaseChars<CHARS...> operator""_ichars() { return { }; }

template <typename T, T... CHARS>
constexpr cts::XChars<CHARS...> operator""_xchars() { return { }; }

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} //end: namespace literals

} //end: namespace cts

