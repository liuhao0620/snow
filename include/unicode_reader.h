#pragma once
#include <fstream>
#include "unicode.h"

namespace LANG_NS
{
    namespace Unicode
    {
        class ReaderBase
        {
        public:
            Option<CharT> Read()
            {
                if (_pos >= _str.size())
                {
                    return Option<CharT>();
                }
                return _str[_pos++];
            }

            const StringT ModuleName() const
            {
                return _module_name;
            }
        protected:
            StringT _str;
            StringT _module_name;
            SizeT _pos = 0;
        };

        class StringReader : public ReaderBase
        {
        public:
            explicit StringReader(const BytesT& str, Unicode::FormatType format_type = Unicode::FormatType::Auto, bool without_bom = true)
            {
                _str = Unicode::Decode(str, format_type, without_bom);
                _module_name = U"<string>";
            }

            explicit StringReader(const StringT& str)
            {
                _str = str;
                _module_name = U"<string>";
            }
        };

        class FileReader : public ReaderBase
        {
        public:
            explicit FileReader(const StringT& file_name, Unicode::FormatType format_type = Unicode::FormatType::Auto, bool without_bom = true)
            {
                std::ifstream ifs(Unicode::Encode(file_name, Unicode::FormatType::ANSI), std::ios_base::in | std::ios_base::binary);
                BytesT str(
                    (std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>()
                );
                if (0 == str.find(Utf8CodingTag))
                {
                    format_type = FormatType::Utf8;
                }
                _str = Unicode::Decode(str, format_type, without_bom);
                _module_name = file_name;
            }
        };
    }
}
