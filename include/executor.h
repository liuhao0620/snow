#pragma once
#include <memory>
#include "environment_interface.h"
#include "pre_define.h"
#include "syntax_tree.h"

namespace LANG_NS
{
	namespace Executor
	{
        class ControlException : public std::exception
        {
        public:
            enum class EType
            {
                Return = 0,
                Break,
                Continute,
            };

            ControlException(EType t, ValuePtrList v = ValuePtrList())
                : ce_type(t)
                , ce_values(v)
            {
            }

        public:
            EType ce_type;
            ValuePtrList ce_values;
        };

        class Node;
        static ValuePtr GetValueFromList(const ValuePtrList& values, SizeT index = 0);
        static ValuePtrList ChunkCall(SharedPtr<Node> block, EnvironmentInterface& env, const ValuePtrList& params);
        static ValuePtrList FunctionCall(SharedPtr<Node> block, EnvironmentInterface& env, const ValuePtrList& params, const ValuePtrList& param_names);

        class Node
        {
        public:
            Node(const SyntaxTree::NodePtr& syntax_tree_node, Node* parent)
                : _syntax_tree_node(syntax_tree_node)
                , _parent(parent)
            {}

            const SyntaxTree::NodePtr GetSyntaxTreeNode() const
            {
                return _syntax_tree_node;
            }

            ValuePtr GetValue(const ValueData& k, EnvironmentInterface& env)
            {
                auto iter = _node_env.find(k);
                if (iter != _node_env.end())
                {
                    return iter->second;
                }
                if (_parent)
                {
                    return _parent->GetValue(k, env);
                }
                return env.GetValue(k);
            }

            void SetValue(const ValueData& k, ValuePtr v, EnvironmentInterface& env)
            {
                _node_env[k] = v;
            }

            ValuePtr AssignValue(const ValueData& k, ValuePtr v, EnvironmentInterface& env)
            {
                if (_node_env.find(k) != _node_env.end())
                {
                    _node_env[k] = v;
                    return _node_env[k];
                }
                if (_parent)
                {
                    return _parent->AssignValue(k, v, env);
                }
                return env.AssignValue(k, v);
            }

            SharedPtr<Node> MakeChildNode(const SyntaxTree::NodePtr& syntax_tree_node)
            {
                auto child = MakeShared<Node>(syntax_tree_node, this);
                _children.push_back(child);
                return child;
            }

            ValuePtrList Execute(EnvironmentInterface& env)
            {
                if (!_syntax_tree_node)
                {
                    throw(Exception(U"Cannot execute a null syntax tree node!!!"));
                    return {};
                }
                switch (_syntax_tree_node->node_type)
                {
                case SyntaxTree::NodeType::Chunk:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::Chunk>(_syntax_tree_node);
                    return {
                        Value::New(
                            std::bind(
                                &ChunkCall,
                                MakeShared<Node>(actual_node->block, nullptr),
                                std::placeholders::_1,
                                std::placeholders::_2
                            )
                        )
                    };
                }
                case SyntaxTree::NodeType::Block:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::Block>(_syntax_tree_node);
                    for (auto iter = actual_node->statements.begin(); iter != actual_node->statements.end(); ++iter)
                    {
                        (void)MakeChildNode(*iter)->Execute(env);
                    }
                    return {};
                }
                case SyntaxTree::NodeType::FunctionStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::FunctionStatement>(_syntax_tree_node);
                    auto func = Value::New(
                        std::bind(
                            &FunctionCall,
                            MakeChildNode(actual_node->block),
                            std::placeholders::_1,
                            std::placeholders::_2,
                            MakeChildNode(actual_node->var_name_list)->Execute(env)
                        )
                    );
                    if (actual_node->name)
                    {
                        Assert(actual_node->name->GetType() == ETokenType::Id);
                        (void)env.AssignValue(ValueData(actual_node->name->StringValue()), func);
                    }
                    return { func };
                }
                case SyntaxTree::NodeType::ReturnStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::ReturnStatement>(_syntax_tree_node);
                    throw(ControlException(ControlException::EType::Return, MakeChildNode(actual_node->exprs)->Execute(env)));
                    return {};
                }
                case SyntaxTree::NodeType::CallStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::CallStatement>(_syntax_tree_node);
                    auto func_value = GetValueFromList(MakeChildNode(actual_node->func)->Execute(env));
                    return env.Call(func_value, MakeChildNode(actual_node->expr_list)->Execute(env));
                }
                case SyntaxTree::NodeType::VarNameListStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::VarNameListStatement>(_syntax_tree_node);
                    auto name_val_list = MakeChildNode(actual_node->name_list)->Execute(env);
                    ValuePtrList expr_val_list;
                    if (actual_node->expr_list)
                    {
                        expr_val_list = MakeChildNode(actual_node->expr_list)->Execute(env);
                    }
                    for (SizeT i = 0; i < name_val_list.size(); ++i)
                    {
                        _parent->SetValue(*name_val_list[i], GetValueFromList(expr_val_list, i), env);
                    }
                    return {};
                }
                case SyntaxTree::NodeType::AssignmentStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::AssignmentStatement>(_syntax_tree_node);
                    auto var_key_list = MakeChildNode(actual_node->var_list)->Execute(env);
                    Assert(var_key_list.size() % 2 == 0);
                    auto expr_val_list = MakeChildNode(actual_node->expr_list)->Execute(env);
                    for (SizeT i = 0; i < var_key_list.size(); i += 2)
                    {
                        if (var_key_list[i + 1]->GetType() == Value::EType::Nil)
                        {
                            (void)AssignValue(*var_key_list[i], GetValueFromList(expr_val_list, i / 2), env);
                        }
                        else if (var_key_list[i + 1]->GetType() == Value::EType::Array)
                        {
                            if (var_key_list[i]->GetType() != Value::EType::Int)
                            {
                                throw(Exception(U"Assign key of array must be a interger"));
                                return {};
                            }
                            auto key = static_cast<SizeT>(var_key_list[i]->IntValue());
                            var_key_list[i + 1]->SetArrayValue(key, GetValueFromList(expr_val_list, i / 2));
                        }
                        else if (var_key_list[i + 1]->GetType() == Value::EType::Dict)
                        {
                            auto key = var_key_list[i];
                            var_key_list[i + 1]->SetDictValue(*key, GetValueFromList(expr_val_list, i / 2));
                        }
                        else
                        {
                            throw(Exception(U"Assign Invalid left value"));
                            return {};
                        }
                    }
                    return {};
                }
                case SyntaxTree::NodeType::IfStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::IfStatement>(_syntax_tree_node);
                    auto expr_value = GetValueFromList(MakeChildNode(actual_node->expr)->Execute(env));
                    if (!expr_value->BoolValue())
                    {
                        if (actual_node->false_branch)
                        {
                            return MakeChildNode(actual_node->false_branch)->Execute(env);
                        }
                        else
                        {
                            return {};
                        }

                    }

                    try
                    {
                        (void)MakeChildNode(actual_node->true_branch)->Execute(env);
                    }
                    catch (const ControlException & e)
                    {
                        if (e.ce_type == ControlException::EType::Return)
                        {
                            return e.ce_values;
                        }
                        throw(e);
                    }
                    return {};
                }
                case SyntaxTree::NodeType::ElseStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::ElseStatement>(_syntax_tree_node);
                    try
                    {
                        (void)MakeChildNode(actual_node->block)->Execute(env);
                    }
                    catch (const ControlException & e)
                    {
                        if (e.ce_type == ControlException::EType::Return)
                        {
                            return e.ce_values;
                        }
                        throw(e);
                    }
                    return {};
                }
                case SyntaxTree::NodeType::WhileStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::WhileStatement>(_syntax_tree_node);
                    try
                    {
                        while (true)
                        {
                            auto expr_value = GetValueFromList(MakeChildNode(actual_node->expr)->Execute(env));
                            if (!expr_value->BoolValue())
                            {
                                break;
                            }
                            (void)MakeChildNode(actual_node->block)->Execute(env);
                        }
                    }
                    catch (const ControlException & e)
                    {
                        if (e.ce_type != ControlException::EType::Break)
                        {
                            throw(e);
                        }
                    }
                    return {};
                }
                case SyntaxTree::NodeType::BreakStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::BreakStatement>(_syntax_tree_node);
                    throw(ControlException(ControlException::EType::Break));
                    return {};
                }
                case SyntaxTree::NodeType::ForStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::ForStatement>(_syntax_tree_node);
                    // define params
                    auto name_val_list = MakeChildNode(actual_node->var_name_list)->Execute(env);
                    for (auto iter = name_val_list.begin(); iter != name_val_list.end(); ++iter)
                    {
                        SetValue(**iter, Value::New(), env);
                    }
                    auto expr_val = GetValueFromList(MakeChildNode(actual_node->expr)->Execute(env));
                    //
                    try
                    {
                        if (expr_val->GetType() == Value::EType::Array)
                        {
                            auto& array_data = expr_val->ArrayValue();
                            for (auto iter = array_data.begin(); iter != array_data.end(); ++iter)
                            {
                                if (name_val_list.size() >= 1)
                                {
                                    SetValue(*name_val_list[0], *iter, env);
                                }
                                (void)MakeChildNode(actual_node->block)->Execute(env);
                            }
                        }
                        else if (expr_val->GetType() == Value::EType::Dict)
                        {
                            auto& map_data = expr_val->DictValue();
                            for (auto iter = map_data.begin(); iter != map_data.end(); ++iter)
                            {
                                if (name_val_list.size() >= 1)
                                {
                                    SetValue(*name_val_list[0], Value::New(iter->first), env);
                                }
                                if (name_val_list.size() >= 2)
                                {
                                    SetValue(*name_val_list[1], iter->second, env);
                                }
                                (void)MakeChildNode(actual_node->block)->Execute(env);
                            }
                        }
                        else
                        {
                            throw(Exception(U"need a array or a Dict"));
                        }
                    }
                    catch (const ControlException & e)
                    {
                        if (e.ce_type != ControlException::EType::Break)
                        {
                            throw(e);
                        }
                    }
                    return {};
                }
                case SyntaxTree::NodeType::ArrayStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::ArrayStatement>(_syntax_tree_node);
                    Value::ArrayT a;
                    if (actual_node->expr_list)
                    {
                        a = MakeChildNode(actual_node->expr_list)->Execute(env);
                    }
                    return { Value::New(a) };
                }
                case SyntaxTree::NodeType::MapStatement:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::MapStatement>(_syntax_tree_node);
                    Value::DictT d;
                    auto key_list = MakeChildNode(actual_node->key_expr_list)->Execute(env);
                    auto val_list = MakeChildNode(actual_node->val_expr_list)->Execute(env);
                    Assert(key_list.size() == val_list.size());
                    for (SizeT i = 0; i < key_list.size() && i < val_list.size(); ++i)
                    {
                        d[*key_list[i]] = val_list[i];
                    }
                    return { Value::New(d) };
                }
                case SyntaxTree::NodeType::VarList:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::VarList>(_syntax_tree_node);
                    ValuePtrList var_value_list;
                    for (auto iter = actual_node->vars.begin(); iter != actual_node->vars.end(); ++iter)
                    {
                        ValuePtrList var_vals = MakeChildNode(*iter)->Execute(env);
                        var_value_list.insert(var_value_list.end(), var_vals.begin(), var_vals.end());
                    }
                    return var_value_list;
                }
                case SyntaxTree::NodeType::NameList:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::NameList>(_syntax_tree_node);
                    ValuePtrList name_val_list;
                    for (auto iter = actual_node->names.begin(); iter != actual_node->names.end(); ++iter)
                    {
                        Assert((*iter)->GetType() == ETokenType::Id);
                        name_val_list.push_back(Value::New((*iter)->StringValue()));
                    }
                    return name_val_list;
                }
                case SyntaxTree::NodeType::ExpressionList:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::ExpressionList>(_syntax_tree_node);
                    ValuePtrList values;
                    ValuePtrList temp_values;
                    for (auto iter = actual_node->exprs.begin(); iter != actual_node->exprs.end(); ++iter)
                    {
                        temp_values = MakeChildNode(*iter)->Execute(env);
                        values.push_back(GetValueFromList(temp_values));
                    }
                    if (temp_values.size() > 1)
                    {
                        values.insert(values.end(), temp_values.begin() + 1, temp_values.end());
                    }
                    return values;
                }
                case SyntaxTree::NodeType::BinaryExpression:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::BinaryExpression>(_syntax_tree_node);
                    auto func_val = GetValue(*actual_node->op->OperatorFunctionName(), env);
                    return env.Call(
                        func_val,
                        {
                            GetValueFromList(MakeChildNode(actual_node->left)->Execute(env)),
                            GetValueFromList(MakeChildNode(actual_node->right)->Execute(env))
                        }
                    );
                }
                case SyntaxTree::NodeType::UnaryExpression:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::UnaryExpression>(_syntax_tree_node);
                    auto func_val = GetValue(*actual_node->op->OperatorFunctionName(false), env);
                    return env.Call(
                        func_val,
                        {
                            GetValueFromList(MakeChildNode(actual_node->expr)->Execute(env))
                        }
                    );
                }
                case SyntaxTree::NodeType::VarExpression:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::VarExpression>(_syntax_tree_node);
                    ValuePtr key_val = GetValueFromList(MakeChildNode(actual_node->key)->Execute(env));
                    ValuePtr expr_val = Value::New();
                    if (actual_node->expr)
                    {
                        expr_val = GetValueFromList(MakeChildNode(actual_node->expr)->Execute(env));
                    }
                    return { key_val, expr_val };
                }
                case SyntaxTree::NodeType::Terminator:
                {
                    auto actual_node = std::static_pointer_cast<SyntaxTree::Terminator>(_syntax_tree_node);
                    if (actual_node->token->GetType() == ETokenType::Id)
                    {
                        return { GetValue(actual_node->token->StringValue(), env) };
                    }
                    return { Value::New(actual_node->token) };
                }
                default:
                    break;
                }
                return {};
            }

        private:
            SyntaxTree::NodePtr _syntax_tree_node;
            TMap<ValueData, ValuePtr> _node_env;
            Node* _parent;
            TVector<SharedPtr<Node>> _children;
        };

        static ValuePtr GetValueFromList(const ValuePtrList& values, SizeT index)
        {
            if (values.size() <= index)
            {
                return Value::New();
            }
            return values[index];
        }
        
        static ValuePtrList ChunkCall(SharedPtr<Node> block, EnvironmentInterface& env, const ValuePtrList& params)
        {
            // define params
            block->SetValue(U"args", Value::New(params), env);

            // run func
            try
            {
                Assert(block->Execute(env).empty());
            }
            catch (const ControlException & e)
            {
                if (e.ce_type == ControlException::EType::Return)
                {
                    return e.ce_values;
                }
                throw(Exception(U"Unexcept ControlException"));
            }
            return {};
        }

        static ValuePtrList FunctionCall(SharedPtr<Node> block, EnvironmentInterface& env, const ValuePtrList& params, const ValuePtrList& param_names)
        {
            // define params
            for (SizeT i = 0; i < param_names.size(); ++i)
            {
                block->SetValue(*param_names[i], GetValueFromList(params, i), env);
            }

            // run func
            try
            {
                Assert(block->Execute(env).empty());
            }
            catch (const ControlException & e)
            {
                if (e.ce_type == ControlException::EType::Return)
                {
                    return e.ce_values;
                }
                throw(Exception(U"Unexcept ControlException"));
            }
            return {};
        }
    }
}
