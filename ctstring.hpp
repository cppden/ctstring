#include <cstdint>
#include <utility>
#include <type_traits>
#include <tuple>

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
			? detail::apply(is_equal, begin, typename base_t::type{}, offset)
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

//template <char... CHARS>
//void show(detail::chars<CHARS...>)
//{
//	using swallow = int[];
//	(void)swallow{0, (std::printf("%c", CHARS), 0)...};
//}

namespace literals {

template <typename T, T... CHARS>
constexpr cts::Chars<CHARS...> operator""_chars() { return { }; }

template <typename T, T... CHARS>
constexpr cts::CaseChars<CHARS...> operator""_ichars() { return { }; }

} //end: namespace

} //end: namespace cts

