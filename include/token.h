#pragma once
#include "pre_define.h"
#include "unicode.h"

namespace LANG_NS
{
    #define TOKEN_NAME_MAKER(xx) \
        xx(True, "true") \
        xx(False, "false") \
        xx(Function, "func") \
        xx(Return, "return") \
        xx(Var, "var") \
        xx(If, "if") \
        xx(Else, "else") \
        xx(While, "while") \
        xx(Break, "break") \
        xx(For, "for") \
        xx(In, "in") \
        xx(BitwiseAnd, "&") \
        xx(And, "&&") \
        xx(BitwiseOr, "|") \
        xx(Or, "||") \
        xx(BitwiseNot, "~") \
        xx(Not, "!") \
        xx(Xor, "^") \
        xx(Add, "+") \
        xx(Sub, "-") \
        xx(Mul, "*") \
        xx(Div, "/") \
        xx(Mod, "%") \
        xx(Assign, "=") \
        xx(Equel, "==") \
        xx(Greater, ">") \
        xx(GreaterEquel, ">=") \
        xx(Less, "<") \
        xx(LessEquel, "<=") \
        xx(NotEquel, "!=") \
        xx(LeftParen, "(") \
        xx(RightParen, ")") \
        xx(LeftSquareBrace, "[") \
        xx(RightSquareBrace, "]") \
        xx(LeftBrace, "{") \
        xx(RightBrace, "}") \
        xx(Comma, ",") \
        xx(Dot, ".") \
        xx(Eof, "<eof>")

    #define TOKEN_NAME_2ENUM(kw, kws) kw,
    #define TOKEN_NAME_TYPE2STR(kw, kws) {ETokenType::kw, U"<token_type " kws " >"},
    #define TOKEN_NAME_STR2TYPE(kw, kws) {U"" kws, ETokenType::kw},

    enum class ETokenType
    {
        Nil = 0,
        Bool,
        Int,
        Float,
        String,
        Id,
        TOKEN_NAME_MAKER(TOKEN_NAME_2ENUM)
    };

    TMap<ETokenType, StringT> TokenNameMapByType = {
        TOKEN_NAME_MAKER(TOKEN_NAME_TYPE2STR)
    };

    TMap<StringT, ETokenType> TokenTypeMapByName = {
        TOKEN_NAME_MAKER(TOKEN_NAME_STR2TYPE)
    };
    
    #undef TOKEN_NAME_2ENUM
    #undef TOKEN_NAME_STR2TYPE
    #undef TOKEN_NAME_TYPE2STR
    #undef TOKEN_NAME_MAKER

    class TokenT : NoCopyable
    {
    public:
        static Option<ETokenType> CheckKeyword(const CharT c)
        {
            return CheckKeyword(StringT(1, c));
        }

        static Option<ETokenType> CheckKeyword(const StringT& str)
        {
            if (str == U"nil")
            {
                return Option<ETokenType>(ETokenType::Nil);
            }
            else if (str == U"true" || str == U"false")
            {
                return Option<ETokenType>(ETokenType::Bool);
            }
            auto iter = TokenTypeMapByName.find(str);
            if (iter == TokenTypeMapByName.end())
            {
                return Option<ETokenType>();
            }
            return Option<ETokenType>(iter->second);
        }

        static SharedPtr<TokenT> New(const ETokenType t, SizeT line, SizeT column)
        {
            Assert(
                t != ETokenType::Bool
                && t != ETokenType::Int
                && t != ETokenType::Float
                && t != ETokenType::String
                && t != ETokenType::Id
                && t != ETokenType::Nil
                && t != ETokenType::True
                && t != ETokenType::False
            );
            return SharedPtr<TokenT>(new TokenT(t, line, column));
        }

        static SharedPtr<TokenT> New()
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Eof, 0, 0));
            return token;
        }

        static SharedPtr<TokenT> New(SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Nil, line, column));
            return token;
        }

        static SharedPtr<TokenT> New(const BoolT b, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Bool, line, column));
            token->_value.b = b;
            return token;
        }

        static SharedPtr<TokenT> New(const IntT i, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Int, line, column));
            token->_value.i = i;
            return token;
        }

        static SharedPtr<TokenT> New(const int i, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Int, line, column));
            token->_value.i = i;
            return token;
        }

        static SharedPtr<TokenT> New(const FloatT f, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Float, line, column));
            token->_value.f = f;
            return token;
        }

        static SharedPtr<TokenT> New(const float f, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Float, line, column));
            token->_value.f = f;
            return token;
        }

        static SharedPtr<TokenT> New(const StringT& s, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::String, line, column));
            (*token->_value.s) = s;
            return token;
        }

        static SharedPtr<TokenT> New(const CharT* s, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::String, line, column));
            (*token->_value.s) = s;
            return token;
        }

        static SharedPtr<TokenT> NewId(const StringT& id, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Id, line, column));
            (*token->_value.s) = id;
            return token;
        }

        static SharedPtr<TokenT> NewId(const CharT* s, SizeT line, SizeT column)
        {
            SharedPtr<TokenT> token(new TokenT(ETokenType::Id, line, column));
            (*token->_value.s) = s;
            return token;
        }

    public:
        virtual ~TokenT()
        {
            if (_type == ETokenType::String || _type == ETokenType::Id)
            {
                delete _value.s;
                _value.s = nullptr;
            }
        }

        ETokenType GetType() const
        {
            return _type;
        }

        const BoolT BoolValue() const
        {
            Assert(_type == ETokenType::Bool);
            return _value.b;
        }

        const IntT IntValue() const
        {
            Assert(_type == ETokenType::Int);
            return _value.i;
        }

        const FloatT FloatValue() const
        {
            Assert(_type == ETokenType::Float);
            return _value.f;
        }

        const StringT StringValue() const
        {
            Assert(
                _type == ETokenType::String
                || _type == ETokenType::Id
            );
            return *(_value.s);
        }

        SizeT GetLine() const
        {
            return _line;
        }

        SizeT GetColumn() const
        {
            return _column;
        }

        StringT ToString() const
        {
            switch (_type)
            {
            case ETokenType::Nil:
                return U"nil";
            case ETokenType::Bool:
                return _value.b ? U"true" : U"false";
                break;
            case ETokenType::Int:
                return LANG_NS::ToString(_value.i);
                break;
            case ETokenType::Float:
                return LANG_NS::ToString(_value.f);
                break;
            case ETokenType::String:
                return *_value.s;
                break;
            case ETokenType::Id:
                return StringT(U"<Id ") + *_value.s + U">";
                break;
            default:
                return TokenNameMapByType[_type];
                break;
            }
            return U"<unknown_token_type>";
        }

        Option<StringT> OperatorFunctionName(bool binary_op = true) const
        {
            const static TMap<ETokenType, StringT> binary_op_func_names = {
                {ETokenType::BitwiseAnd, U"__bitwise_and"},
                {ETokenType::And, U"__and"},
                {ETokenType::BitwiseOr, U"__bitwise_or"},
                {ETokenType::Or, U"__or"},
                {ETokenType::Xor, U"__xor"},
                {ETokenType::Add, U"__add"},
                {ETokenType::Sub, U"__sub"},
                {ETokenType::Mul, U"__mul"},
                {ETokenType::Div, U"__div"},
                {ETokenType::Mod, U"__mod"},
                {ETokenType::Equel, U"__equel"},
                {ETokenType::Greater, U"__greater"},
                {ETokenType::GreaterEquel, U"__greater_equel"},
                {ETokenType::Less, U"__less"},
                {ETokenType::LessEquel, U"__less_equel"},
                {ETokenType::NotEquel, U"__not_equel"},
                {ETokenType::LeftSquareBrace, U"__get_member"},
            };
            const static TMap<ETokenType, StringT> unary_op_func_names = {
                {ETokenType::BitwiseNot, U"__bitwise_not"},
                {ETokenType::Not, U"__not"},
                {ETokenType::Add, U"__positive"},
                {ETokenType::Sub, U"__negative"},
            };
            if (!binary_op)
            {
                auto iter = unary_op_func_names.find(_type);
                if (iter == unary_op_func_names.end())
                {
                    return Option<StringT>();
                }
                return iter->second;
            }
            auto iter = binary_op_func_names.find(_type);
            if (iter == binary_op_func_names.end())
            {
                return Option<StringT>();
            }
            return iter->second;
        }

        SharedPtr<TokenT> Clone() const
        {
            SharedPtr<TokenT> token(new TokenT(_type, _line, _column));
            if (_type == ETokenType::Bool)
            {
                token->_value.b = _value.b;
            }
            else if (_type == ETokenType::Int)
            {
                token->_value.i = _value.i;
            }
            else if (_type == ETokenType::Float)
            {
                token->_value.f = _value.f;
            }
            else if (_type == ETokenType::String || _type == ETokenType::Id)
            {
                *(token->_value.s) = *(_value.s);
            }
            return token;
        }

    private:
        TokenT(const ETokenType t, SizeT line, SizeT column)
            : _type(t)
            , _line(line)
            , _column(column)
        {
            _value.i = 0;
            if (_type == ETokenType::String || _type == ETokenType::Id)
            {
                _value.s = new StringT;
            }
        }

    private:
        ETokenType _type;
        union {
            bool b;
            IntT i;
            FloatT f;
            StringT* s;
        } _value;
        SizeT _line;
        SizeT _column;
    };
} // namespace LANG_NS

