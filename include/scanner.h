#pragma once
#include "pre_define.h"
#include "token.h"
#include "unicode_reader.h"

namespace LANG_NS
{
    class Scanner
    {
    public:
        using ReaderType = Unicode::ReaderBase;
    public:
        Scanner(const SharedPtr<ReaderType> reader)
            : _reader(reader)
            , _module_name(_reader ? _reader->ModuleName() : U"<unknown>")
        {

        }

        SharedPtr<TokenT> Scan()
        {
            if (!_current)
            {
                _current = Next();
            }
            while(_current)
            {
                if (
                    *_current == ' '
                    || *_current == '\t'
                    || *_current == '\v'
                    || *_current == '\f'
                )
                {
                    _current = Next();
                }
                else if (*_current == '\r' || *_current == '\n')
                {
                    NewLine();
                }
                else if (*_current == '&')
                {
                    auto c = Next();
                    if (c == '&')
                    {
                        _current = Next();
                        return NormalToken(ETokenType::And);
                    }
                    else
                    {
                        _current = c;
                        return NormalToken(ETokenType::BitwiseAnd);
                    }
                }
                else if (*_current == '|')
                {
                    auto c = Next();
                    if (c == '|')
                    {
                        _current = Next();
                        return NormalToken(ETokenType::Or);
                    }
                    else
                    {
                        _current = c;
                        return NormalToken(ETokenType::BitwiseOr);
                    }
                }
                else if (
                    *_current == '~'
                    || *_current == '^'
                    || *_current == '+'
                    || *_current == '-'
                    || *_current == '*'
                    || *_current == '%'
                    || *_current == '('
                    || *_current == ')'
                    || *_current == '['
                    || *_current == ']'
                    || *_current == '{'
                    || *_current == '}'
                    || *_current == ','
                )
                {
                    auto token_type = TokenT::CheckKeyword(*_current);
                    Assert(token_type);
                    _current = Next();
                    return NormalToken(*token_type);
                }
                else if (*_current == '/')
                {
                    auto c = Next();
                    if (*c == '/')
                    {
                        Comment();
                    }
                    else
                    {
                        _current = c;
                        return NormalToken(ETokenType::Div);
                    }
                }
                else if (
                    *_current == '='
                    || *_current == '<'
                    || *_current == '>'
                    || *_current == '!'
                )
                {
                    return XEquelToken();
                }
                else if (*_current == '.')
                {
                    auto c = Next();
                    if (Unicode::IsDigit(*c))
                    {
                        _buffer.clear();
                        _buffer += *_current;
                        _current = c;
                        return NumberToken(true);
                    }
                    else if (Unicode::IsLetter(*c) || *c == '_')
                    {
                        _current = c;
                        return NormalToken(ETokenType::Dot);
                    }
                    else
                    {
                        return Error(U"Unexcept Character");
                    }
                }
                else if (Unicode::IsDigit(*_current))
                {
                    return NumberToken(false);
                }
                else if (*_current == '\'' || *_current == '"')
                {
                    return SinglelineStringToken();
                }
                else if (Unicode::IsLetter(*_current) || *_current == '_')
                {
                    return IdToken();
                }
                else
                {
                    return Error(U"Unexcept Character");
                }
            }
            return TokenT::New();
        } 
        
        const StringT ModuleName() const
        {
            return _module_name;
        }

    private:
        SharedPtr<TokenT> Error(const StringT& err_info) const
        {
            StringT err_msg(err_info);
            err_msg += U" near '";
            err_msg += *_current;
            err_msg += U"' in module ";
            err_msg += _module_name;
            err_msg += U" line ";
            err_msg += ToString(_line);
            err_msg += U" column ";
            err_msg += ToString(_column);
            throw(Exception(err_msg));
            return TokenT::New();
        }

        Option<CharT> Next()
        {
            auto c = _reader->Read();
            if (c)
            {
                ++ _column;
            }
            return c;
        }

        void NewLine()
        {
            auto c = Next();
            if ((*c == '\r' || *c == '\n') && (*c != *_current))
            {
                _current = Next();
            }
            else
            {
                _current = c;
            }
            ++ _line;
            _column = 0;
        }

        void Comment()
        {
            _current = Next();
            while (_current && *_current != '\r' && *_current != '\n')
            {
                _current = Next();
            }
        }

        SharedPtr<TokenT> NormalToken(ETokenType t)
        {
            return TokenT::New(t, _line, _column);
        }

        SharedPtr<TokenT> XEquelToken()
        {
            _buffer.clear();
            _buffer += *_current;
            _current = Next();
            if (*_current == '=')
            {
                _buffer += *_current;
                _current = Next();
            }

            auto token_type = TokenT::CheckKeyword(_buffer);
            Assert(token_type);
            return NormalToken(*token_type);
        }

        SharedPtr<TokenT> NumberToken(bool has_dot)
        {
            bool scientific_notation = false;
            if (!has_dot)
            {
                _buffer.clear();
            }
            while (
                _current && (
                    Unicode::IsDigit(*_current)
                    || *_current == '.'
                )
            )
            {
                if (has_dot && *_current == '.')
                {
                    return Error(U"Unexcept Character");
                }
                if (*_current == '.')
                {
                    has_dot = true;
                }
                _buffer += *_current;
                _current = Next();
            }
            if (
                _current && (
                    *_current == 'E' 
                    || *_current == 'e'
                    )
                )
            {
                _buffer += *_current;
                _current = Next();
                if (
                    !_current || (
                        *_current != '+'
                        && *_current != '-'
                        && !Unicode::IsDigit(*_current)
                        )
                    )
                {
                    return Error(U"Incomplete Number");
                }
                if (*_current == '+' || *_current == '-')
                {
                    _buffer += *_current;
                    _current = Next();
                }
                if (!_current || !Unicode::IsDigit(*_current))
                {
                    return Error(U"Incomplete Number");
                }
                while (_current && Unicode::IsDigit(*_current))
                {
                    _buffer += *_current;
                    _current = Next();
                }
                scientific_notation = true;
            }
            if (has_dot || scientific_notation)
            {
                return TokenT::New(StringToFloat(_buffer), _line, _column);
            }
            return TokenT::New(StringToInt(_buffer), _line, _column);
        }

        SharedPtr<TokenT> SinglelineStringToken()
        {
            auto quote = _current;
            _buffer.clear();
            _current = Next();
            while (*_current != *quote)
            {
                if (!_current || *_current == '\r' || *_current == '\n')
                {
                    return Error(U"Incomplete String");
                }
                else
                {
                    auto err_msg = StringCharacter();
                    if (err_msg)
                    {
                        return Error(*err_msg);
                    }
                }
            }
            _current = Next();
            return TokenT::New(_buffer, _line, _column);
        }

        Option<StringT> StringCharacter()
        {
            if (*_current == '\\')
            {
                _current = Next();
                if (*_current == 'a')
                {
                    _buffer += '\a';
                }
                else if (*_current == 'b')
                {
                    _buffer += '\b';
                }
                else if (*_current == 'f')
                {
                    _buffer += '\f';
                }
                else if (*_current == 'n')
                {
                    _buffer += '\n';
                }
                else if (*_current == 'r')
                {
                    _buffer += '\r';
                }
                else if (*_current == 't')
                {
                    _buffer += '\t';
                }
                else if (*_current == 'v')
                {
                    _buffer += '\v';
                }
                else if (*_current == '\\')
                {
                    _buffer += '\\';
                }
                else if (*_current == '"')
                {
                    _buffer += '"';
                }
                else if (*_current == '\'')
                {
                    _buffer += '\'';
                }
                else
                {
                    return Option<StringT>(U"Unexcept Character");
                }
            }
            else
            {
                _buffer += *_current;
            }
            _current = Next();
            return Option<StringT>();
        }

        SharedPtr<TokenT> IdToken()
        {
            _buffer.clear();
            _buffer += *_current;
            _current = Next();
            while (
                _current && (
                    Unicode::IsLetter(*_current)
                    || Unicode::IsDigit(*_current)
                    || *_current == '_'
                )
            )
            {
                _buffer += *_current;
                _current = Next();
            }
            auto token_type = TokenT::CheckKeyword(_buffer);
            if (token_type)
            {
                if (*token_type == ETokenType::Nil)
                {
                    return TokenT::New(_line, _column);
                }
                else if (*token_type == ETokenType::Bool)
                {
                    return TokenT::New(_buffer == U"true", _line, _column);
                }
                return NormalToken(*token_type);
            }
            return TokenT::NewId(_buffer, _line, _column);
        }

    private:
        SharedPtr<ReaderType> _reader;
        StringT _module_name;
        Option<CharT> _current;
        SizeT _line = 1;
        SizeT _column = 0;
        StringT _buffer;
    };
}
