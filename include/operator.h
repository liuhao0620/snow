#pragma once
#include "pre_define.h"
#include "token.h"

namespace LANG_NS
{
    namespace Operator
    {
        class Helper
        {
            Helper()
                : OpPrioMap{
                    {ETokenType::BitwiseNot, 2},
                    {ETokenType::Not, 2},
                    {ETokenType::Mul, 3},
                    {ETokenType::Div, 3},
                    {ETokenType::Mod, 3},
                    {ETokenType::Add, 4},
                    {ETokenType::Sub, 4},
                    {ETokenType::Greater, 6},
                    {ETokenType::GreaterEquel, 6},
                    {ETokenType::Less, 6},
                    {ETokenType::LessEquel, 6},
                    {ETokenType::Equel, 7},
                    {ETokenType::NotEquel, 7},
                    {ETokenType::BitwiseAnd, 8},
                    {ETokenType::Xor, 9},
                    {ETokenType::BitwiseOr, 10},
                    {ETokenType::And, 11},
                    {ETokenType::Or, 12},
                }
            {}

            TMap<ETokenType, SizeT> OpPrioMap;

            friend Option<SizeT> GetPrio(ETokenType token_type);
        };

        static Option<SizeT> GetPrio(ETokenType token_type)
        {
            static const Helper __helper;
            auto iter = __helper.OpPrioMap.find(token_type);
            if (iter == __helper.OpPrioMap.end())
            {
                return Option<SizeT>();
            }
            return Option<SizeT>(iter->second);
        }

        static const SizeT MaxPrio()
        {
            return 12;
        }

        static const SizeT MinPrio()
        {
            return 3;
        }
    }
}
