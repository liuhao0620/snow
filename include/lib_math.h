#pragma once
#include <math.h>
#include "environment_interface.h"
#include "pre_define.h"

namespace LANG_NS
{
    namespace MathLib
    {
        static ValuePtrList Sqrt(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 1)
            {
                throw(Exception(U"__Sqrt must be one params"));
                return {};
            }
            ValuePtr val = params[0];
            if (val->GetType() == Value::EType::Int)
            {
                return {Value::New(sqrt((FloatT)val->IntValue()))};
            }
            else if (val->GetType() == Value::EType::Float)
            {
                return {Value::New(sqrt(val->FloatValue()))};
            }
            throw(Exception(U"__Sqrt Invalid params"));
            return {};
        }

        static ValuePtrList Abs(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 1)
            {
                throw(Exception(U"__Abs must be one params"));
                return {};
            }
            ValuePtr val = params[0];
            if (val->GetType() == Value::EType::Int)
            {
                return {Value::New(abs(val->IntValue()))};
            }
            else if (val->GetType() == Value::EType::Float)
            {
                return {Value::New(abs(val->FloatValue()))};
            }
            throw(Exception(U"__Abs Invalid params"));
            return {};
        }

        static ValuePtrList Cos(EnvironmentInterface& env, const ValuePtrList& params)
        {
            if (params.size() != 1)
            {
                throw(Exception(U"__Cos must be one params"));
                return {};
            }
            ValuePtr val = params[0];
            if (val->GetType() == Value::EType::Int)
            {
                return {Value::New(cos(val->IntValue()))};
            }
            else if (val->GetType() == Value::EType::Float)
            {
                return {Value::New(cos(val->FloatValue()))};
            }
            throw(Exception(U"__Cos Invalid params"));
            return {};
        }

        static void Registe(EnvironmentInterface& env)
        {
            Value::DictT math_dict = {
                {U"sqrt", Value::New(Sqrt)},
                {U"abs", Value::New(Abs)},
                {U"cos", Value::New(Cos)},
            };
            (void)env.AssignValue(U"math", Value::New(math_dict));
        }
    }
}