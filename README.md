[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](../master/LICENSE)

# ctstring
Compile-Time String

## Strings at compile-time
Strings can be used for anything at compile-time, e.g. for hash calculation or matching:
```cpp
auto sample1 = "Hello"_chars;
auto sample2 = "Hello"_ichars;
char constexpr csz[] = "hello world";
static_assert(!sample1.match(csz, csz+sizeof(csz)), "");
static_assert(sample2.match(csz, csz+sizeof(csz)), "");
```

## Obfuscated strings
Obfuscate strings in compiled binary to hide sensitive data:
```cpp
auto const xsHidden = "Farewell: this shouldn't be seen in binary!"_xchars;
std::cout << "hidden: " << xsHidden1.str().c_str() << std::endl;
```

