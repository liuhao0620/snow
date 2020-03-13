#pragma once
#include "environment_interface.h"
#include "executor.h"
#include "lib_base.h"
#include "lib_math.h"
#include "parser.h"
#include "pre_define.h"

namespace LANG_NS
{
    class Environment : public EnvironmentInterface
    {
    public:
        static Environment& GetInstance()
        {
            static Environment _instance;
            return _instance;
        }

    public:
        Environment()
        {
            BaseLib::Registe(*this);
            MathLib::Registe(*this);
            _global[U"__loaded"] = Value::New(Value::DictT());
        }

        ValuePtr LoadString(const StringT& str) override
        {
            auto reader = MakeShared<Unicode::StringReader>(str);
            auto scanner = MakeShared<Scanner>(reader);
            auto parser = MakeShared<Parser>(scanner);
            try
            {
                auto ast = parser->Parse();
                auto values = MakeShared<Executor::Node>(ast, nullptr)->Execute(*this);
                Assert(!values.empty());
                return values[0];
            }
            catch (const Exception & e)
            {
                std::cerr << "Error in LoadString : " << e.Info() << std::endl;
            }
            return nullptr;
        }

        ValuePtr LoadFile(const StringT& file_name) override
        {
            auto reader = MakeShared<Unicode::FileReader>(file_name);
            auto scanner = MakeShared<Scanner>(reader);
            auto parser = MakeShared<Parser>(scanner);
            try
            {
                auto ast = parser->Parse();
                auto values = MakeShared<Executor::Node>(ast, nullptr)->Execute(*this);
                Assert(!values.empty());
                return values[0];
            }
            catch (const Exception & e)
            {
                std::cerr << "Error in LoadFile : " << e.Info() << std::endl;
            }
            return nullptr;
        }

    public:
        void RegisteFunctions(const TMap<StringT, Value::FunctionT>& functions) override
        {
            for (auto iter = functions.begin(); iter != functions.end(); ++ iter)
            {
                _global[iter->first] = Value::New(iter->second);
            }
        }

        ValuePtr GetValue(const ValueData& k) override
        {
            auto iter = _global.find(k);
            if (iter == _global.end())
            {
                return Value::New();
            }
            return iter->second;
        }

        ValuePtr AssignValue(const ValueData& k, ValuePtr v) override
        {
            _global[k] = v;
            return _global[k];
        }

        ValuePtrList Call(ValuePtr func, const ValuePtrList& params) override
        {
            try
            {
                if (!func->Callable())
                {
                    throw(Exception(U"Cannot be Called!!!"));
                }
                if (func->GetType() == Value::EType::Function)
                {
                    return func->FunctionValue()(*this, params);
                }
            }
            catch (const Exception & e)
            {
                std::cerr << "Error in Call : " << e.Info() << std::endl;
            }
            return {};
        }

    private:
        TMap<ValueData, ValuePtr> _global;
    };
}
