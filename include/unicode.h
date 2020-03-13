#pragma once
#include <sstream>
#include "unicode_helper.h"

namespace LANG_NS
{
    namespace Unicode
    {
        static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);
        enum class FormatType
        {
            Auto = 0,
            ANSI,
            Utf8,
            Utf16LE,
            Utf16BE,
            Utf32LE,
            Utf32BE,
        };

        static StringT __WstringToUcs4(const std::wstring& src);
        static std::wstring __Ucs4ToWstring(const StringT& src);

        static StringT __ANSIToUcs4(const BytesT& src)
        {
            Helper::SetLocale();
            const char* src_str = src.data();
            std::mbstate_t state;
            memset(&state, 0, sizeof(std::mbstate_t));
            size_t len = std::mbsrtowcs(nullptr, &src_str, 0, &state);
            std::wstring dst;
            dst.resize(len);
            std::mbsrtowcs(dst.data(), &src_str, len, &state);
            return __WstringToUcs4(dst);
        }

        static StringT __Utf8ToUcs4(const BytesT& src, bool without_bom)
        {
            if (without_bom)
            {
                auto& converter = Helper::ConverterUtf8WithoutBOM();
                return converter.from_bytes(src.c_str(), src.c_str() + src.size());
            }
            auto& converter = Helper::ConverterUtf8();
            return converter.from_bytes(src.c_str(), src.c_str() + src.size());
        }

        static StringT __Utf16LEToUcs4(const BytesT& src, bool without_bom)
        {
            if (without_bom)
            {
                auto& converter = Helper::ConverterUtf16LEWithoutBOM();
                return converter.from_bytes(src.c_str(), src.c_str() + src.size());
            }
            auto& converter = Helper::ConverterUtf16LE();
            return converter.from_bytes(src.c_str(), src.c_str() + src.size());
        }

        static StringT __Utf16BEToUcs4(const BytesT& src, bool without_bom)
        {
            if (without_bom)
            {
                auto& converter = Helper::ConverterUtf16BEWithoutBOM();
                return converter.from_bytes(src.c_str(), src.c_str() + src.size());
            }
            auto& converter = Helper::ConverterUtf16BE();
            return converter.from_bytes(src.c_str(), src.c_str() + src.size());
        }

        static StringT __Utf32LEToUcs4(const BytesT& src, bool without_bom)
        {
            assert(src.size() % 4 == 0);
            if (Helper::IsLittleEndian())
            {
                return StringT((CharT*)src.data(), src.size() / 4);
            }
            StringT dst = StringT((CharT*)src.data(), src.size() / 4);
            for (size_t i = 0; i < dst.size(); ++i)
            {
                dst[i] = Helper::SwapEndian(dst[i]);
            }
            return dst;
        }

        static StringT __Utf32BEToUcs4(const BytesT& src, bool without_bom)
        {
            assert(src.size() % 4 == 0);
            if (!Helper::IsLittleEndian())
            {
                return StringT((CharT*)src.data(), src.size() / 4);
            }
            StringT dst = StringT((CharT*)src.data(), src.size() / 4);
            for (size_t i = 0; i < dst.size(); ++i)
            {
                dst[i] = Helper::SwapEndian(dst[i]);
            }
            return dst;
        }

        static StringT __WstringToUcs4(const std::wstring& src)
        {
            if (sizeof(wchar_t) == 4)
            {
                return StringT((CharT*)src.data(), src.size());
            }
            else
            {
                if (Helper::IsLittleEndian())
                {
                    return __Utf16LEToUcs4(BytesT((ByteT*)src.data(), src.size() * sizeof(wchar_t)), false);
                }
                else
                {
                    return __Utf16BEToUcs4(BytesT((ByteT*)src.data(), src.size() * sizeof(wchar_t)), false);
                }
            }
            return StringT();
        }

        static BytesT __Ucs4ToANSI(const StringT& src)
        {
            Helper::SetLocale();
            std::wstring src_wstr = __Ucs4ToWstring(src);
            size_t len = std::wcstombs(nullptr, src_wstr.data(), 0);
            if (len == static_cast<size_t>(-1))
            {
                throw(Exception(U"Invalid character, cannot convert to ansi"));
                return "";
            }
            BytesT dst;
            dst.resize(len);
            std::wcstombs((char*)dst.data(), src_wstr.data(), dst.size());
            return dst;
        }

        static BytesT __Ucs4ToUtf8(const StringT& src, bool generate_bom)
        {
            if (!generate_bom)
            {
                auto& converter = Helper::ConverterUtf8WithoutBOM();
                return converter.to_bytes(src);
            }
            auto& converter = Helper::ConverterUtf8();
            return converter.to_bytes(src);
        }

        static BytesT __Ucs4ToUtf16LE(const StringT& src, bool generate_bom)
        {
            if (!generate_bom)
            {
                auto& converter = Helper::ConverterUtf16LEWithoutBOM();
                return converter.to_bytes(src);
            }
            auto& converter = Helper::ConverterUtf16LE();
            return converter.to_bytes(src);
        }

        static BytesT __Ucs4ToUtf16BE(const StringT& src, bool generate_bom)
        {
            if (!generate_bom)
            {
                auto& converter = Helper::ConverterUtf16BEWithoutBOM();
                return converter.to_bytes(src);
            }
            auto& converter = Helper::ConverterUtf16BE();
            return converter.to_bytes(src);
        }

        static BytesT __Ucs4ToUtf32LE(const StringT& src, bool generate_bom)
        {
            if (Helper::IsLittleEndian())
            {
                return BytesT((ByteT*)src.data(), src.size() * 4);
            }
            StringT dst = src;
            for (size_t i = 0; i < dst.size(); ++i)
            {
                dst[i] = Helper::SwapEndian(dst[i]);
            }
            return BytesT((ByteT*)dst.data(), dst.size() * 4);
        }

        static BytesT __Ucs4ToUtf32BE(const StringT& src, bool generate_bom)
        {
            if (!Helper::IsLittleEndian())
            {
                return BytesT((ByteT*)src.data(), src.size() * 4);
            }
            StringT dst = src;
            for (size_t i = 0; i < dst.size(); ++i)
            {
                dst[i] = Helper::SwapEndian(dst[i]);
            }
            return BytesT((ByteT*)dst.data(), dst.size() * 4);
        }

        static std::wstring __Ucs4ToWstring(const StringT& src)
        {
            if (sizeof(wchar_t) == 4)
            {
                return std::wstring((wchar_t*)src.data(), src.size());
            }
            else if (sizeof(wchar_t) == 2)
            {
                if (Helper::IsLittleEndian())
                {
                    BytesT dst = __Ucs4ToUtf16LE(src, false);
                    assert(dst.size() % 2 == 0);
                    return std::wstring((wchar_t*)dst.data(), dst.size() / 2);
                }
                else
                {
                    BytesT dst = __Ucs4ToUtf16BE(src, false);
                    assert(dst.size() % 2 == 0);
                    return std::wstring((wchar_t*)dst.data(), dst.size() / 2);
                }
            }
            return std::wstring();
        }

        static StringT Decode(const BytesT& src, FormatType src_format_type = FormatType::Auto, bool src_without_bom = true)
        {
            if (src.empty())
            {
                return StringT();
            }
            do
            {
                if (src_format_type == FormatType::Auto)
                {
                    src_without_bom = false;
                    if (src.size() % 4 == 0)
                    {
                        if (src[0] == '\xFF' && src[1] == '\xFE' && src[2] == '\0' && src[3] == '\0')
                        {
                            src_format_type = FormatType::Utf32LE;
                            break;
                        }
                        else if (src[0] == '\0' && src[1] == '\0' && src[2] == '\xFE' && src[3] == '\xFF')
                        {
                            src_format_type = FormatType::Utf32BE;
                            break;
                        }
                    }
                    if (src.size() % 2 == 0)
                    {
                        if (src[0] == '\xFF' && src[1] == '\xFE')
                        {
                            src_format_type = FormatType::Utf16LE;
                            break;
                        }
                        else if (src[0] == '\xFE' && src[1] == '\xFF')
                        {
                            src_format_type = FormatType::Utf16BE;
                            break;
                        }
                    }
                    if (src.size() >= 3)
                    {
                        if (
                            src[0] == '\xEF'
                            && src[1] == '\xBB'
                            && src[2] == '\xBF'
                            )
                        {
                            src_format_type = FormatType::Utf8;
                            break;
                        }
                    }
                    src_format_type = FormatType::ANSI;
                    src_without_bom = true;
                }
            } while (false);
            if (src_format_type == FormatType::ANSI)
            {
                return __ANSIToUcs4(src);
            }
            else if (src_format_type == FormatType::Utf8)
            {
                return __Utf8ToUcs4(src, src_without_bom);
            }
            else if (src_format_type == FormatType::Utf16LE)
            {
                return __Utf16LEToUcs4(src, src_without_bom);
            }
            else if (src_format_type == FormatType::Utf16BE)
            {
                return __Utf16BEToUcs4(src, src_without_bom);
            }
            else if (src_format_type == FormatType::Utf32LE)
            {
                return __Utf32LEToUcs4(src, src_without_bom);
            }
            else if (src_format_type == FormatType::Utf32BE)
            {
                return __Utf32BEToUcs4(src, src_without_bom);
            }
            return StringT();
        }

        static BytesT Encode(const StringT& src, FormatType dst_format_type, bool dst_generate_bom = false)
        {
            if (src.empty())
            {
                return BytesT();
            }
            if (dst_format_type == FormatType::ANSI)
            {
                return __Ucs4ToANSI(src);
            }
            else if (dst_format_type == FormatType::Utf8)
            {
                return __Ucs4ToUtf8(src, dst_generate_bom);
            }
            else if (dst_format_type == FormatType::Utf16LE)
            {
                return __Ucs4ToUtf16LE(src, dst_generate_bom);
            }
            else if (dst_format_type == FormatType::Utf16BE)
            {
                return __Ucs4ToUtf16BE(src, dst_generate_bom);
            }
            else if (dst_format_type == FormatType::Utf32LE)
            {
                return __Ucs4ToUtf32LE(src, dst_generate_bom);
            }
            else if (dst_format_type == FormatType::Utf32BE)
            {
                return __Ucs4ToUtf32BE(src, dst_generate_bom);
            }
            return BytesT();
        }

        static bool IsDigit(CharT c)
        {
            if (
                c == '0' || c == '1' || c == '2' || c == '3' || c == '4'
                || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'
                )
            {
                return true;
            }
            return Helper::InCharCheckList(Helper::DigitTable(), c);
        }

        static bool IsLetter(CharT c)
        {
            if (
                (c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z')
                )
            {
                return true;
            }
            return Helper::InCharCheckList(Helper::LetterTable(), c);
        }
    }
    template < typename T >
    StringT ToString(const T& v)
    {
        std::stringstream ss;
        ss << v;
        std::string temp;
        ss >> temp;
        return Unicode::Decode(temp, Unicode::FormatType::ANSI);
    }
    static IntT StringToInt(const StringT& s)
    {
        return std::stoll(Unicode::Encode(s, Unicode::FormatType::ANSI));
    }
    static FloatT StringToFloat(const StringT& s)
    {
        return std::stof(Unicode::Encode(s, Unicode::FormatType::ANSI));
    }
    static Ostream& operator<<(Ostream& os, const StringT& str)
    {
        return (os << Unicode::Encode(str, Unicode::FormatType::ANSI));
    }
}
