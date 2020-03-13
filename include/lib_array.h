#pragma once
#include "environment_interface.h"
#include "pre_define.h"

namespace LANG_NS
{
    namespace ArrayLib
    {
        static ValuePtrList Remove(ValuePtr arr, EnvironmentInterface& env, const ValuePtrList& params)
        {
            Assert(arr->GetType() == Value::EType::Array);
            auto& array_data = arr->ArrayValue();
            if (array_data.empty())
            {
                throw(Exception(U"Array.Remove array is empty!!!"));
                return {};
            }
            SizeT remove_index = array_data.size() - 1;
            SizeT remove_count = 1;
            if (params.size() >= 1)
            {
                if (
                    params[0]->GetType() != Value::EType::Int
                    || params[0]->IntValue() < 0
                    )
                {
                    throw(Exception(U"Array.Remove param[0] is invalid!!!"));
                    return {};
                }
                remove_index = static_cast<SizeT>(params[0]->IntValue());
            }
            if (params.size() >= 2)
            {
                if (
                    params[1]->GetType() != Value::EType::Int
                    || params[1]->IntValue() < 0
                    )
                {
                    throw(Exception(U"Array.Remove param[1] is invalid!!!"));
                    return {};
                }
                remove_count = static_cast<SizeT>(params[1]->IntValue());
            }
            if (remove_index + remove_count > array_data.size())
            {
                throw(Exception(U"Array.Remove elements not enough!!!"));
                return {};
            }
            arr->RemoveArrayValue(remove_index, remove_count);
            return {};
        }

        static ValuePtrList Insert(ValuePtr arr, EnvironmentInterface& env, const ValuePtrList& params)
        {
            Assert(arr->GetType() == Value::EType::Array);
            auto& array_data = arr->ArrayValue();
            if (params.size() >= 2)
            {
                if (
                    params[0]->GetType() != Value::EType::Int
                    || params[0]->IntValue() < 0
                    || params[0]->IntValue() > static_cast<IntT>(array_data.size())
                    )
                {
                    throw(Exception(U"Array.Insert param[0] is invalid!!!"));
                    return {};
                }
                arr->InsertArrayValue(static_cast<SizeT>(params[0]->IntValue()), params[1]);
            }
            else if (params.size() == 1)
            {
                arr->InsertArrayValue(array_data.size(), params[0]);
            }
            else
            {
                throw(Exception(U"Array.Insert need a param!!!"));
            }
            return {};
        }

        static ValuePtr GetFunc(ValuePtr arr, const StringT& key)
        {
            static const TMap<StringT, ValuePtrList(*)(ValuePtr, EnvironmentInterface&, const ValuePtrList&)> _funcs = {
                {U"remove", Remove},
                {U"insert", Insert},
            };
            auto iter = _funcs.find(key);
            if (iter == _funcs.end())
            {
                return nullptr;
            }
            return Value::New(std::bind(iter->second, arr, std::placeholders::_1, std::placeholders::_2));
        }
    }
}