#pragma once
#include "environment_interface.h"
#include "lib_array.h"
#include "pre_define.h"

namespace LANG_NS
{
    namespace BaseLib
    {
        static ValuePtrList __BitwiseAnd(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__BitwiseAnd must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() & right->IntValue())};
                }
            }
            throw(Exception(U"__BitwiseAnd Invalid params"));
            return {};
        }

        static ValuePtrList __And(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__And must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            return {Value::New(left->BoolValue() && right->BoolValue())};
        }

        static ValuePtrList __BitwiseOr(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__BitwiseOr must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() | right->IntValue())};
                }
            }
            throw(Exception(U"__BitwiseOr Invalid params"));
            return {};
        }

        static ValuePtrList __Or(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Or must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            return {Value::New(left->BoolValue() || right->BoolValue())};
        }

        static ValuePtrList __BitwiseNot(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 1)
            {
                throw(Exception(U"__BitwiseNot must be one params"));
                return {};
            }
            ValuePtr val = params[0];
            if (val->GetType() == Value::EType::Int)
            {
                return {Value::New(~(val->IntValue()))};
            }
            throw(Exception(U"__BitwiseNot Invalid params"));
            return {};
        }

        static ValuePtrList __Not(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 1)
            {
                throw(Exception(U"__Not must be one params"));
                return {};
            }
            ValuePtr val = params[0];
            return {Value::New(!val->BoolValue())};
        }
        
        static ValuePtrList __Xor(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Xor must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() ^ right->IntValue())};
                }
            }
            throw(Exception(U"__Xor Invalid params"));
            return {};
        }

        static ValuePtrList __Add(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Add must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() + right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New((FloatT)left->IntValue() + right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() + right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() + right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::String)
            {
                return {Value::New(left->StringValue() + right->ToString())};
            }
            throw(Exception(U"__Add Invalid params"));
            return {};
        }

        static ValuePtrList __Positive(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 1)
            {
                throw(Exception(U"__Positive must be one params"));
                return {};
            }
            ValuePtr val = params[0];
            if (val->GetType() == Value::EType::Int)
            {
                return {Value::New(val->IntValue())};
            }
            else if (val->GetType() == Value::EType::Float)
            {
                return {Value::New(val->FloatValue())};
            }
            throw(Exception(U"__Positive Invalid params"));
            return {};
        }

        static ValuePtrList __Negative(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 1)
            {
                throw(Exception(U"__Negative must be one params"));
                return {};
            }
            ValuePtr val = params[0];
            if (val->GetType() == Value::EType::Int)
            {
                return {Value::New(-val->IntValue())};
            }
            else if (val->GetType() == Value::EType::Float)
            {
                return {Value::New(-val->FloatValue())};
            }
            throw(Exception(U"__Negative Invalid params"));
            return {};
        }

        static ValuePtrList __Sub(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Sub must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() - right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New((FloatT)left->IntValue() - right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() - right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() - right->FloatValue())};
                }
            }
            throw(Exception(U"__Sub Invalid params"));
            return {};
        }

        static ValuePtrList __Mul(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Mul must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() * right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New((FloatT)left->IntValue() * right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() * right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() * right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::String)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    if (right->IntValue() > 0)
                    {
                        StringT temp;
                        temp.reserve(left->StringValue().size() * static_cast<SizeT>(right->IntValue()));
                        for (IntT i = 0; i < right->IntValue(); ++i)
                        {
                            temp += left->StringValue();
                        }
                        return { Value::New(temp) };
                    }
                }
            }
            throw(Exception(U"__Mul Invalid params"));
            return {};
        }

        static ValuePtrList __Div(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Div must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() / right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New((FloatT)left->IntValue() / right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() / right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() / right->FloatValue())};
                }
            }
            throw(Exception(U"__Div Invalid params"));
            return {};
        }

        static ValuePtrList __Mod(EnvironmentInterface& env, const ValuePtrList& params)
        {if (params.size() != 2)
            {
                throw(Exception(U"__Mod must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() % right->IntValue())};
                }
            }
            throw(Exception(U"__Mod Invalid params"));
            return {};
        }

        static ValuePtrList __Equel(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Equel must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            return {Value::New((!(*left < *right)) && (!(*right < *left)))};
        }

        static ValuePtrList __Greater(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Greater must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() > right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->IntValue() > right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() > right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() > right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::String)
            {
                if (right->GetType() == Value::EType::String)
                {
                    return {Value::New(left->StringValue() > right->StringValue())};
                }
            }
            throw(Exception(U"__Greater Invalid params"));
            return {};
        }

        static ValuePtrList __GreaterEquel(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__GreaterEquel must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() >= right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->IntValue() >= right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() >= right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() >= right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::String)
            {
                if (right->GetType() == Value::EType::String)
                {
                    return {Value::New(left->StringValue() >= right->StringValue())};
                }
            }
            throw(Exception(U"__GreaterEquel Invalid params"));
            return {};
        }

        static ValuePtrList __Less(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__Less must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() < right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->IntValue() < right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() < right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() < right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::String)
            {
                if (right->GetType() == Value::EType::String)
                {
                    return {Value::New(left->StringValue() < right->StringValue())};
                }
            }
            throw(Exception(U"__Less Invalid params"));
            return {};
        }

        static ValuePtrList __LessEquel(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__LessEquel must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Int)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->IntValue() <= right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->IntValue() <= right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::Float)
            {
                if (right->GetType() == Value::EType::Int)
                {
                    return {Value::New(left->FloatValue() <= right->IntValue())};
                }
                else if (right->GetType() == Value::EType::Float)
                {
                    return {Value::New(left->FloatValue() <= right->FloatValue())};
                }
            }
            else if (left->GetType() == Value::EType::String)
            {
                if (right->GetType() == Value::EType::String)
                {
                    return {Value::New(left->StringValue() <= right->StringValue())};
                }
            }
            throw(Exception(U"__LessEquel Invalid params"));
            return {};
        }

        static ValuePtrList __NotEquel(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__NotEquel must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            return {Value::New((*left < *right) || (*right < *left))};
        }

        static ValuePtrList __GetMember(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 2)
            {
                throw(Exception(U"__GetMember must be two params"));
                return {};
            }
            ValuePtr left = params[0];
            ValuePtr right = params[1];
            if (left->GetType() == Value::EType::Array)
            {
                if (right->GetType() == Value::EType::String)
                {
                    auto func = ArrayLib::GetFunc(left, right->StringValue());
                    if (func)
                    {
                        return {func};
                    }
                }
                if (right->GetType() != Value::EType::Int)
                {
                    throw(Exception(U"__GetMember key of array must be a interger"));
                    return {};
                }
                auto& array_data = left->ArrayValue();
                auto key = static_cast<SizeT>(right->IntValue());
                if (key < array_data.size() && array_data[key])
                {
                    return {array_data[key]};
                }
                return {Value::New()};
            }
            else if (left->GetType() == Value::EType::Dict)
            {
                auto& map_data = left->DictValue();
                auto iter = map_data.find(*right);
                if (iter == map_data.end())
                {
                    return {Value::New()};
                }
                return {iter->second};
            }
            else
            {
                throw(Exception(U"__GetMember first param must be a array or a map"));
            }
            return {};
        }

        static ValuePtrList LoadString(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() < 1)
            {
                throw(Exception(U"LoadString need a param"));
                return {};
            }
            auto param = params[0];
            if (param->GetType() != Value::EType::String)
            {
                throw(Exception(U"LoadString param must be String"));
                return {};
            }
            return {env.LoadString(param->StringValue())};
        }

        static ValuePtrList LoadFile(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() < 1)
            {
                throw(Exception(U"LoadFile need a param"));
                return {};
            }
            auto param = params[0];
            if (param->GetType() != Value::EType::String)
            {
                throw(Exception(U"LoadFile param must be String"));
                return {};
            }
            return {env.LoadFile(param->StringValue())};
        }

        static ValuePtrList DoString(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() < 1)
            {
                throw(Exception(U"DoString need a param"));
                return {};
            }
            auto param = params[0];
            if (param->GetType() != Value::EType::String)
            {
                throw(Exception(U"DoString param[0] must be String"));
                return {};
            }
            return env.Call(env.LoadString(param->StringValue()), ValuePtrList(params.begin() + 1, params.end()));
        }

        static ValuePtrList DoFile(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() < 1)
            {
                throw(Exception(U"DoFile need a param"));
                return {};
            }
            auto param = params[0];
            if (param->GetType() != Value::EType::String)
            {
                throw(Exception(U"DoFile param[0] must be String"));
                return {};
            }
            return env.Call(env.LoadFile(param->StringValue()), ValuePtrList(params.begin() + 1, params.end()));
        }

        static ValuePtrList Import(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() < 1)
            {
                throw(Exception(U"Import need a param"));
                return {};
            }
            auto param = params[0];
            if (param->GetType() != Value::EType::String)
            {
                throw(Exception(U"Import param[0] must be String"));
                return {};
            }
            auto loaded_modules = env.GetValue(U"__loaded");
            Assert(loaded_modules->GetType() == Value::EType::Dict);
            ValuePtrList results;
            auto iter = loaded_modules->DictValue().find(*param);
            if (iter == loaded_modules->DictValue().end())
            {
                ValuePtrList do_file_params = params;
                {
                    StringT import_path = do_file_params[0]->StringValue();
                    SizeT index;
                    while ((index = import_path.find(U".")) != StringT::npos)
                    {
                        import_path[index] = U'/';
                    }
                    if (import_path.find_last_of(LANG_EXT_NAME_U) != import_path.size() - sizeof(LANG_EXT_NAME_U) / sizeof(U""))
                    {
                        import_path += LANG_EXT_NAME_U;
                    }
                    do_file_params[0] = Value::New(import_path);
                }
                results = DoFile(env, do_file_params);
                if (results.empty())
                {
                    results.push_back(Value::New(true));
                }
                loaded_modules->SetDictValue(*param, Value::New(results));
            }
            else
            {
                Assert(iter->second->GetType() == Value::EType::Array);
                results = iter->second->ArrayValue();
            }
            return results;
        }

        static ValuePtrList Type(EnvironmentInterface& env, const ValuePtrList& params)
        {
            ValuePtrList results;
            for (auto iter = params.begin(); iter != params.end(); ++ iter)
            {
                results.push_back(Value::New(Value::TypeString((*iter)->GetType())));
            }
            return results;
        }

        static ValuePtrList Len(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() < 1)
            {
                throw(Exception(U"Len need a param"));
                return {};
            }
            auto param = params[0];
            if (param->GetType() == Value::EType::Array)
            {
                return {Value::New(param->ArrayValue().size())};
            }
            else if (param->GetType() == Value::EType::Dict)
            {
                return {Value::New(param->DictValue().size())};
            }
            else
            {
                throw(Exception(U"Len param must be a array or a map"));
            }
            return {};
        }

        static ValuePtrList Range(EnvironmentInterface& env, const ValuePtrList& params)
        {
            union {
                IntT i;
                FloatT f;
            } begin, end, interval;
            begin.i = 0;
            end.i = 0;
            interval.i = 0;
            bool interger = true;

            if (params.size() == 0)
            {
                throw(Exception(U"Len need a param or more"));
                return {};
            }
            else if (params.size() == 1)
            {
                if (
                    params[0]->GetType() != Value::EType::Int
                    && params[0]->GetType() != Value::EType::Float
                )
                {
                    throw(Exception(U"Len param[0] must be a int number or a float number"));
                    return {};
                }
                interger = params[0]->GetType() == Value::EType::Int;
                if (interger)
                {
                    begin.i = 0;
                    interval.i = 1;
                }
                else
                {
                    begin.f = 0.0;
                    interval.f = 1.0;
                }
                if (params[0]->GetType() == Value::EType::Int)
                {
                    if (interger)
                    {
                        end.i = params[0]->IntValue();
                    }
                    else
                    {
                        end.f = static_cast<FloatT>(params[0]->IntValue());
                    }
                }
                else if (params[0]->GetType() == Value::EType::Float)
                {
                    Assert(!interger);
                    end.f = params[0]->FloatValue();
                }
            }
            else if (params.size() == 2)
            {
                if (
                    params[0]->GetType() != Value::EType::Int
                    && params[0]->GetType() != Value::EType::Float
                )
                {
                    throw(Exception(U"Len param[0] must be a int number or a float number"));
                    return {};
                }
                if (
                    params[1]->GetType() != Value::EType::Int
                    && params[1]->GetType() != Value::EType::Float
                )
                {
                    throw(Exception(U"Len param[1] must be a int number or a float number"));
                    return {};
                }
                interger = (
                    (params[0]->GetType() == Value::EType::Int)
                    && (params[1]->GetType() == Value::EType::Int)
                );
                if (interger)
                {
                    interval.i = 1;
                }
                else
                {
                    interval.f = 1.0;
                }
                if (params[0]->GetType() == Value::EType::Int)
                {
                    if (interger)
                    {
                        begin.i = params[0]->IntValue();
                    }
                    else
                    {
                        begin.f = static_cast<FloatT>(params[0]->IntValue());
                    }
                }
                else if (params[0]->GetType() == Value::EType::Float)
                {
                    Assert(!interger);
                    begin.f = params[0]->FloatValue();
                }
                if (params[1]->GetType() == Value::EType::Int)
                {
                    if (interger)
                    {
                        end.i = params[1]->IntValue();
                    }
                    else
                    {
                        end.f = static_cast<FloatT>(params[1]->IntValue());
                    }
                }
                else if (params[1]->GetType() == Value::EType::Float)
                {
                    Assert(!interger);
                    end.f = params[1]->FloatValue();
                }
            }
            else if (params.size() > 3)
            {
                if (
                    params[0]->GetType() != Value::EType::Int
                    && params[0]->GetType() != Value::EType::Float
                )
                {
                    throw(Exception(U"Len param[0] must be a int number or a float number"));
                    return {};
                }
                if (
                    params[1]->GetType() != Value::EType::Int
                    && params[1]->GetType() != Value::EType::Float
                )
                {
                    throw(Exception(U"Len param[1] must be a int number or a float number"));
                    return {};
                }
                if (
                    params[2]->GetType() != Value::EType::Int
                    && params[2]->GetType() != Value::EType::Float
                )
                {
                    throw(Exception(U"Len param[2] must be a int number or a float number"));
                    return {};
                }
                interger = (
                    (params[0]->GetType() == Value::EType::Int)
                    && (params[1]->GetType() == Value::EType::Int)
                    && (params[2]->GetType() == Value::EType::Int)
                );
                if (params[0]->GetType() == Value::EType::Int)
                {
                    if (interger)
                    {
                        begin.i = params[0]->IntValue();
                    }
                    else
                    {
                        begin.f = static_cast<FloatT>(params[0]->IntValue());
                    }
                }
                else if (params[0]->GetType() == Value::EType::Float)
                {
                    Assert(!interger);
                    begin.f = params[0]->FloatValue();
                }
                if (params[1]->GetType() == Value::EType::Int)
                {
                    if (interger)
                    {
                        end.i = params[1]->IntValue();
                    }
                    else
                    {
                        end.f = static_cast<FloatT>(params[1]->IntValue());
                    }
                }
                else if (params[1]->GetType() == Value::EType::Float)
                {
                    Assert(!interger);
                    end.f = params[1]->FloatValue();
                }
                if (params[2]->GetType() == Value::EType::Int)
                {
                    if (interger)
                    {
                        interval.i = params[2]->IntValue();
                    }
                    else
                    {
                        interval.f = static_cast<FloatT>(params[2]->IntValue());
                    }
                }
                else if (params[2]->GetType() == Value::EType::Float)
                {
                    Assert(!interger);
                    interval.f = params[2]->FloatValue();
                }
            }

            Value::ArrayT array_data;
            if (interger)
            {
                if (begin.i <= end.i)
                {
                    if (interval.i <= 0)
                    {
                        throw(Exception(U"Len Invalid interval"));
                        return {};
                    }
                    for (IntT i = begin.i; i <= end.i; i += interval.i)
                    {
                        array_data.push_back(Value::New(i));
                    }
                }
                else
                {
                    if (interval.i >= 0)
                    {
                        throw(Exception(U"Len Invalid interval"));
                        return {};
                    }
                    for (IntT i = begin.i; i >= end.i; i += interval.i)
                    {
                        array_data.push_back(Value::New(i));
                    }
                }
            }
            else
            {
                if (begin.f <= end.f)
                {
                    if (interval.f <= 0)
                    {
                        throw(Exception(U"Len Invalid interval"));
                        return {};
                    }
                    for (FloatT i = begin.f; i <= end.f; i += interval.f)
                    {
                        array_data.push_back(Value::New(i));
                    }
                }
                else
                {
                    if (interval.f >= 0)
                    {
                        throw(Exception(U"Len Invalid interval"));
                        return {};
                    }
                    for (FloatT i = begin.f; i >= end.f; i += interval.f)
                    {
                        array_data.push_back(Value::New(i));
                    }
                }
            }
            return {Value::New(array_data)};
        }

        static ValuePtrList Print(EnvironmentInterface& env, const ValuePtrList& params)
        {
            for (auto iter = params.begin(); iter != params.end(); ++ iter)
            {
                std::cout << (*iter)->ToString() << " ";
            }
            return {};
        }

        static ValuePtrList Println(EnvironmentInterface& env, const ValuePtrList& params)
        {
            for (auto iter = params.begin(); iter != params.end(); ++ iter)
            {
                std::cout << (*iter)->ToString() << " ";
            }
            std::cout << std::endl;
            return {};
        }

        static void Registe(EnvironmentInterface& env)
        {
            env.RegisteFunctions({
                {U"__bitwise_and", __BitwiseAnd},
                {U"__and", __And},
                {U"__bitwise_or", __BitwiseOr},
                {U"__or", __Or},
                {U"__bitwise_not", __BitwiseNot},
                {U"__not", __Not},
                {U"__xor", __Xor},
                {U"__add", __Add},
                {U"__positive", __Positive},
                {U"__sub", __Sub},
                {U"__negative", __Negative},
                {U"__mul", __Mul},
                {U"__div", __Div},
                {U"__mod", __Mod},
                {U"__equel", __Equel},
                {U"__greater", __Greater},
                {U"__greater_equel", __GreaterEquel},
                {U"__less", __Less},
                {U"__less_equel", __LessEquel},
                {U"__not_equel", __NotEquel},
                {U"__get_member", __GetMember},
                {U"loadstring", LoadString},
                {U"loadfile", LoadFile},
                {U"dostring", DoString},
                {U"dofile", DoFile},
                {U"import", Import},
                {U"type", Type},
                {U"len", Len},
                {U"range", Range},
                {U"print", Print},
                {U"println", Println},
            });
        }
    }
}
