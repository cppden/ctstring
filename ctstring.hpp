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


namespace detail {

template <char... CHARS>
using chars = std::integer_sequence<char, CHARS...>;
template <std::size_t... INDEXES>
using indexes = std::index_sequence<INDEXES...>;

template <class... ICs>
struct sequence {};

template <std::size_t I, char C>
struct index_char
{
	static constexpr std::size_t my_index = I;
	static constexpr char my_char = C;
};

template <std::size_t ...Is>
struct zip
{
	template <char ...Cs>
	struct with
	{
		using type = sequence<index_char<Is, Cs>...>;
	};
};

template <std::size_t... Is, char... Cs>
constexpr auto add_index(indexes<Is...>, chars<Cs...>) -> typename zip<Is...>::template with<Cs...>::type;

template <char... Cs>
using indexed_chars = decltype(
	add_index(std::make_index_sequence<sizeof...(Cs)>{}, chars<Cs...>{})
);

template <char... CHARS>
class Chars
{
public:
	static constexpr std::size_t length() { return sizeof...(CHARS); }
	using type = detail::indexed_chars<CHARS...>;
};


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

} //end: namespace detail


template <char... CHARS>
class Chars : public detail::Chars<CHARS...>
{
	using base_t = detail::Chars<CHARS...>;

public:
	constexpr bool match(char const* begin, char const* end) const
	{
		return (begin + base_t::length() <= end)
			? detail::apply(is_equal, begin, typename base_t::type{})
			: false;
	}

	constexpr bool match(char const* begin, char const* end, std::size_t offset) const
	{
		return (begin + base_t::length() <= end && offset < base_t::length())
			? detail::apply(is_equal, begin, typename base_t::type{}, offset)
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
class CaseChars : public detail::Chars<CHARS...>
{
	using base_t = detail::Chars<CHARS...>;

public:
	constexpr bool match(char const* begin, char const* end) const
	{
		return (begin + base_t::length() <= end)
			? detail::apply(is_iequal, begin, typename base_t::type{})
			: false;
	}

	constexpr bool match(char const* begin, char const* end, std::size_t offset) const
	{
		return (begin + base_t::length() <= end && offset < base_t::length())
			? detail::apply(is_iequal, begin, typename base_t::type{}, offset)
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


namespace detail {

#ifndef CTS_RND_SEED
#define CTS_TIME(index) ((__TIME__[index]-'0')*10 + (__TIME__[index+1]-'0'))
#define CTS_RND_SEED uint32_t(CTS_TIME(0)*3600 + CTS_TIME(3)*60 + CTS_TIME(6) + 13709*(31+ __COUNTER__))
#endif

//https://en.wikipedia.org/wiki/Linear_congruential_generator
//LCG: X(n + 1) = (A * X(n) + C) % m
constexpr std::size_t rand(std::size_t n)
{
	//NOTE: cast makes the mod 2^32
	return static_cast<uint32_t>(1664525u * (n ? rand(n - 1) : CTS_RND_SEED) + 1013904223u);
}

constexpr std::size_t rand_next(std::size_t rand_curr = CTS_RND_SEED)
{
	//NOTE: cast makes the mod 2^32
	return static_cast<uint32_t>(1664525u * rand_curr + 1013904223u);
}

constexpr char xor_char(char const c, std::size_t const index)
{
	return c ^ static_cast<char>(rand(index));
}

template <std::size_t ...Is>
struct xor_zip
{
	template <char ...Cs>
	struct with
	{
		using type = sequence<index_char<Is, xor_char(Cs, Is)>...>;
	};
};

template <std::size_t... Is, char... Cs>
constexpr auto xor_index(indexes<Is...>, chars<Cs...>) -> typename xor_zip<Is...>::template with<Cs...>::type;

template <char... Cs>
using xored_chars = decltype(
	xor_index(std::make_index_sequence<sizeof...(Cs)>{}, chars<Cs...>{})
);

template <std::size_t STATE>
constexpr void xor_impl(char* p) { }

template <std::size_t STATE, class IC, class... ICs>
constexpr void xor_impl(char* p)
{
	p[IC::my_index] = IC::my_char ^ static_cast<char>(STATE);
	xor_impl<rand_next(STATE), ICs...>(p);
}

template <class... ICs>
constexpr void apply_xor(char* p, sequence<ICs...>)
{
	return xor_impl<rand(0), ICs...>(p);
}

} //end: namespace detail


//obfuscated string via XOR
template <char... CHARS>
class XorChars
{
	using type = detail::xored_chars<CHARS...>;

public:
	static constexpr std::size_t size()                  { return sizeof...(CHARS); }
	static constexpr std::size_t length()                { return size(); }

	//de-obfuscate at run-time into string on heap which will clean up its data in dtor
	string str() const
	{
		string result{ this->size() };
		detail::apply_xor(result.data(), type{});
		return result;
	}

	//de-obfuscate at run-time avoiding intermediate copy
	//but you should clean-up buffer once it's used/no longer needed
	char* str(void* buffer, std::size_t buf_size) const
	{
		if (buf_size >= size())
		{
			auto* p = static_cast<char*>(buffer);
			detail::apply_xor(p, type{});
			if (buf_size > size()) //NULL terminate if space allows
			{
				p[size()] = '\0';
			}
			return p;
		}
		return nullptr;
	}

	template <typename T, std::size_t QTY>
	char* str(T (&buffer)[QTY]) const
	{
		static_assert(sizeof(buffer) >= size(), "buffer is too small to fit the string");
		return str(buffer, sizeof(buffer));
	}
};


namespace literals {

template <typename T, T... CHARS>
constexpr cts::Chars<CHARS...> operator""_chars() { return { }; }

template <typename T, T... CHARS>
constexpr cts::CaseChars<CHARS...> operator""_ichars() { return { }; }

template <typename T, T... CHARS>
constexpr cts::XorChars<CHARS...> operator""_xchars() { return { }; }

} //end: namespace

} //end: namespace cts

