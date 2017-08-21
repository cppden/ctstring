/**
@file
Run-time string

@copyright Denis Priyomov 2017
Distributed under the MIT License
(See accompanying file LICENSE or visit https://github.com/cppden/ctstring)
*/

#include <vector>
#include <ostream>


namespace cts {

//run-time string which cleans up its memory in dtor
class string
{
public:
	explicit string(std::size_t n): m_str(n + 1)         { }
	~string();

	string(string const&) = delete;
	string& operator=(string const&) = delete;

	string(string&& rhs): m_str{ std::move(rhs.m_str) }  { }
	string& operator=(string&& rhs)                      { m_str = std::move(rhs.m_str); return *this; }

	char const* c_str() const                            { return m_str.data(); }
	char* data()                                         { return m_str.data(); }
	std::size_t size() const                             { return m_str.size() - 1; }
	std::size_t length() const                           { return size(); }

private:
	std::vector<char> m_str;
};

std::ostream& operator<< (std::ostream& out, string const& s);

} //end: namespace cts
