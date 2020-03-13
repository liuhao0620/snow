#pragma once
#include "pre_define.h"
#include "value.h"

namespace LANG_NS
{
    class EnvironmentInterface : NoCopyable
    {
    public:
        virtual ValuePtr LoadString(const StringT& str) = 0;
        virtual ValuePtr LoadFile(const StringT& file_name) = 0;
        virtual void RegisteFunctions(const TMap<StringT, Value::FunctionT>& functions) = 0;
        virtual ValuePtr GetValue(const ValueData& k) = 0;
        virtual ValuePtr AssignValue(const ValueData& k, ValuePtr v) = 0;
        virtual ValuePtrList Call(ValuePtr func, const ValuePtrList& params) = 0;
    };
}