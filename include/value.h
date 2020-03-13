#pragma once
#include "pre_define.h"
#include "token.h"

namespace LANG_NS
{
    class EnvironmentInterface;
    namespace Value
    {
        enum class EType
        {
            Nil = 0,
            Bool,
            Int,
            Float,
            String,
            Array,
            Dict,
            Function,
        };

        static StringT TypeString(EType t)
        {
            static const TMap<EType, StringT> _type_strs = {
                {EType::Nil, U"nil"},
                {EType::Bool, U"bool"},
                {EType::Int, U"int"},
                {EType::Float, U"float"},
                {EType::String, U"str"},
                {EType::Array, U"array"},
                {EType::Dict, U"dict"},
                {EType::Function, U"func"},
            };
            auto iter = _type_strs.find(t);
            if (iter == _type_strs.end())
            {
                return U"";
            }
            return iter->second;
        }

        class Data;
        using ValuePtr = SharedPtr<Value::Data>;
        using ValuePtrList = TVector<ValuePtr>;
        using ArrayT = TVector<ValuePtr>;
        using DictT = TMap<Value::Data, ValuePtr>;
        using FunctionT = std::function<ValuePtrList(EnvironmentInterface&, const ValuePtrList&)>;

        class Data
        {
        public:
            BoolT BoolValue() const
            {
                if (
                    _type == EType::Nil
                    || (
                        _type == EType::Bool
                        && _value.b == false
                    )
                )
                {
                    return false;
                }
                return true;
            }

            IntT IntValue() const
            {
                Assert(_type == EType::Int);
                return _value.i;
            }

            FloatT FloatValue() const
            {
                Assert(_type == EType::Float);
                return _value.f;
            }

            const StringT& StringValue() const
            {
                Assert(_type == EType::String);
                return *_value.s;
            }

            const ArrayT& ArrayValue() const
            {
                Assert(_type == EType::Array);
                return *(*_value.a);
            }

            const DictT& DictValue() const
            {
                Assert(_type == EType::Dict);
                return *(*_value.d);
            }

            void SetArrayValue(SizeT index, ValuePtr val)
            {
                if (index >= (*_value.a)->size())
                {
                    (*_value.a)->resize(index + 1);
                }
                (*(*_value.a))[index] = val;
            }

            void RemoveArrayValue(SizeT index, SizeT count)
            {
                Assert(index + count <= (*_value.a)->size());
                (*_value.a)->erase((*_value.a)->begin() + index, (*_value.a)->begin() + index + count);
            }

            void InsertArrayValue(SizeT index, const ValuePtr& val)
            {
                Assert(index <= (*_value.a)->size());
                (*_value.a)->insert((*_value.a)->begin() + index, val);
            }

            void SetDictValue(const Value::Data& key, ValuePtr val)
            {
                if (val->GetType() == EType::Nil)
                {
                    (*(*_value.d)).erase(key);
                }
                else
                {
                    (*(*_value.d))[key] = val;
                }
            }

            const FunctionT& FunctionValue() const
            {
                Assert(_type == EType::Function);
                return *(*_value.fn);
            }

            StringT ToString() const
            {
                if (_type == EType::Nil)
                {
                    return U"nil";
                }
                else if (_type == EType::Bool)
                {
                    return _value.b ? U"true" : U"false";
                }
                else if (_type == EType::Int)
                {
                    return LANG_NS::ToString(_value.i);
                }
                else if (_type == EType::Float)
                {
                    return LANG_NS::ToString(_value.f);
                }
                else if (_type == EType::String)
                {
                    return *_value.s;
                }
                else if (_type == EType::Array)
                {
                    return StringT(U"Array : ") + LANG_NS::ToString((*_value.a).get());
                }
                else if (_type == EType::Dict)
                {
                    return StringT(U"Dict : ") + LANG_NS::ToString((*_value.d).get());
                }
                else if (_type == EType::Function)
                {
                    return StringT(U"Function : ") + LANG_NS::ToString((*_value.fn).get());
                }
                return U"<unknown>";
            }

            EType GetType() const
            {
                return _type;
            }

            bool Callable() const
            {
                return _type == EType::Function;
            }

            bool operator<(const Data& rhs) const
            {
                if (_type != rhs._type)
                {
                    return _type < rhs._type;
                }
                if (_type == EType::Bool)
                {
                    return _value.b < rhs._value.b;
                }
                else if (_type == EType::Int)
                {
                    return _value.i < rhs._value.i;
                }
                else if (_type == EType::Float)
                {
                    return _value.f < rhs._value.f;
                }
                else if (_type == EType::String)
                {
                    return *(_value.s) < *(rhs._value.s);
                }
                else if (_type == EType::Array)
                {
                    return (*_value.a).get() < (*rhs._value.a).get();
                }
                else if (_type == EType::Dict)
                {
                    return (*_value.d).get() < (*rhs._value.d).get();
                }
                else if (_type == EType::Function)
                {
                    return (*_value.fn).get() < (*rhs._value.fn).get();
                }
                return false;
            }
        public:
            Data()
                : _type(EType::Nil)
            {
                _value.i = 0;
            }

            Data(const BoolT b)
                : _type(EType::Bool)
            {
                _value.b = b;
            }

            Data(const int i)
                : _type(EType::Int)
            {
                _value.i = i;
            }

            Data(const unsigned int i)
                : _type(EType::Int)
            {
                _value.i = i;
            }

            Data(const long unsigned int i)
                : _type(EType::Int)
            {
                _value.i = i;
            }

            Data(const IntT i)
                : _type(EType::Int)
            {
                _value.i = i;
            }

            Data(const unsigned long long i)
                : _type(EType::Int)
            {
                _value.i = i;
            }

            Data(const float f)
                : _type(EType::Float)
            {
                _value.f = f;
            }

            Data(const FloatT f)
                : _type(EType::Float)
            {
                _value.f = f;
            }

            Data(const long double f)
                : _type(EType::Float)
            {
                _value.f = f;
            }

            Data(const CharT * s)
                : _type(EType::String)
            {
                _value.s = new StringT;
                *(_value.s) = s;
            }

            Data(const StringT& s)
                : _type(EType::String)
            {
                _value.s = new StringT;
                *(_value.s) = s;
            }

            Data(const ArrayT& a)
                : _type(EType::Array)
            {
                _value.a = new SharedPtr<ArrayT>;
                *(_value.a) = MakeShared<ArrayT>(a);
            }

            Data(const DictT& d)
                : _type(EType::Dict)
            {
                _value.d = new SharedPtr<DictT>;
                *(_value.d) = MakeShared<DictT>(d);
            }

            Data(const FunctionT& fn)
                : _type(EType::Function)
            {
                _value.fn = new SharedPtr<FunctionT>;
                *(_value.fn) = MakeShared<FunctionT>(fn);
            }

            Data(const SharedPtr<TokenT> token)
            {
                if (token->GetType() == ETokenType::Nil)
                {
                    _type = EType::Nil;
                    _value.i = 0;
                }
                else if (token->GetType() == ETokenType::Bool)
                {
                    _type = EType::Bool;
                    _value.b = token->BoolValue();
                }
                else if (token->GetType() == ETokenType::Int)
                {
                    _type = EType::Int;
                    _value.i = token->IntValue();
                }
                else if (token->GetType() == ETokenType::Float)
                {
                    _type = EType::Float;
                    _value.f = token->FloatValue();
                }
                else if (token->GetType() == ETokenType::String)
                {
                    _type = EType::String;
                    _value.s = new StringT;
                    *_value.s = token->StringValue();
                }
                else
                {
                    throw(Exception(U"Invalid Token"));
                }
            }

            Data(const Data& rhs)
                : _type(rhs._type)
            {
                if (_type == EType::Nil)
                {
                    _value.i = 0;
                }
                else if (_type == EType::Bool)
                {
                    _value.b = rhs._value.b;
                }
                else if (_type == EType::Int)
                {
                    _value.i = rhs._value.i;
                }
                else if (_type == EType::Float)
                {
                    _value.f = rhs._value.f;
                }
                else if (_type == EType::String)
                {
                    _value.s = new StringT;
                    *(_value.s) = *(rhs._value.s);
                }
                else if (_type == EType::Array)
                {
                    _value.a = new SharedPtr<ArrayT>;
                    (*_value.a) = *(rhs._value.a);
                }
                else if (_type == EType::Dict)
                {
                    _value.d = new SharedPtr<DictT>;
                    (*_value.d) = *(rhs._value.d);
                }
                else if (_type == EType::Function)
                {
                    _value.fn = new SharedPtr<FunctionT>;
                    *(_value.fn) = *(rhs._value.fn);
                }
                else
                {
                    throw(Exception(U"Invalid Value::Data"));
                }
            }

            ~Data()
            {
                if (_type == EType::String)
                {
                    delete _value.s;
                    _value.s = nullptr;
                }
                else if (_type == EType::Array)
                {
                    delete _value.a;
                    _value.a = nullptr;
                }
                else if (_type == EType::Dict)
                {
                    delete _value.d;
                    _value.d = nullptr;
                }
                else if (_type == EType::Function)
                {
                    delete _value.fn;
                    _value.fn = nullptr;
                }
            }

        private:
            EType _type = EType::Nil;
            union {
                BoolT b;
                IntT i;
                FloatT f;
                StringT* s;
                SharedPtr<ArrayT>* a;
                SharedPtr<DictT>* d;
                SharedPtr<FunctionT>* fn;
            } _value;
        };

        static SharedPtr<Data> New()
        {
            return MakeShared<Data>();
        }

        template < typename T >
        SharedPtr<Data> New(const T v)
        {
            return MakeShared<Data>(v);
        }
    }

    using ValueData = Value::Data;
    using ValuePtr = Value::ValuePtr;
    using ValuePtrList = Value::ValuePtrList;
}
