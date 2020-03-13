#pragma once
#include <assert.h>
#include <codecvt>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string.h>
#include <vector>

#define LANG_NAME SnowLang
#define LANG_NS Snow
#define LANG_EXT_NAME ".sno"
#define LANG_EXT_NAME_U U".sno"

namespace LANG_NS
{
    // type define
    using NullptrT = std::nullptr_t;
    using BoolT = bool;
    using IntT = long long;
    using FloatT = double;
    using SizeT = size_t;
    using StringT = std::u32string;
    using CharT = StringT::value_type;
    using BytesT = std::string;
    using ByteT = BytesT::value_type;
    using Ostream = std::ostream;
    template < typename KeyType, typename ValueType >
    using TMap = std::map<KeyType, ValueType>;
    template < typename ValueType >
    using TVector = std::vector<ValueType>;
    template < typename ValueType >
    using Option = std::optional<ValueType>;

    // const define
    static const SizeT Utf8MaxSize = 4;
    static const BytesT Utf8CodingTag = "//@coding=utf-8";

    // tools define
    class Exception : public std::exception
    {
    public:
        Exception(const StringT& err) : _what(err) {}
        StringT Info() const noexcept
        {
            return _what;
        }
    private:
        StringT _what;
    };
    class NoCopyable
    {
    protected:
        NoCopyable() = default;
    private:
        NoCopyable(const NoCopyable&) = delete;
        NoCopyable(const NoCopyable&&) = delete;
        NoCopyable& operator=(const NoCopyable&) = delete;
        NoCopyable& operator=(const NoCopyable&&) = delete;
    };
    template < typename T >
    using SharedPtr = std::shared_ptr<T>;
    template < typename T >
    using WeakPtr = std::weak_ptr<T>;
    #define MakeShared std::make_shared
    #define StaticAssert(cond, msg) static_assert(cond, msg)
    #define Assert(cond) assert(cond)
    #define DebugTrace(msg) std::cout << msg << " in " << __FILE__ << " " << __LINE__ << std::endl
}
