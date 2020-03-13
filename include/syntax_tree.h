#pragma once
#include "environment_interface.h"
#include "pre_define.h"
#include "token.h"

namespace LANG_NS
{
    namespace SyntaxTree
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

        static ValuePtr GetValueFromList(const ValuePtrList& values, SizeT index = 0)
        {
            if (values.size() <= index)
            {
                return Value::New();
            }
            return values[index];
        }

        enum class NodeType
        {
            Chunk = 0,
            Block,
            FunctionStatement,
            ReturnStatement,
            CallStatement,
            VarNameListStatement,
            AssignmentStatement,
            IfStatement,
            ElseStatement,
            WhileStatement,
            BreakStatement,
            ForStatement,
            ArrayStatement,
            MapStatement,
            VarList,
            NameList,
            ExpressionList,
            BinaryExpression,
            UnaryExpression,
            VarExpression,
            Terminator,
        };

        class NodeBase
        {
        public:
            NodeBase(NodeType t)
                : node_type(t) 
            {}
            virtual ~NodeBase() {}
            NodeType node_type;
        };

        using NodePtr = SharedPtr<NodeBase>;
        using NodePtrList = TVector<SharedPtr<NodeBase>>;
        using TokenPtr = SharedPtr<TokenT>;
        using TokenPtrList = TVector<SharedPtr<TokenT>>;

#define DEF_SYNTAX_TREE_NODE_TYPE(type_name, var_list) \
        class type_name : public NodeBase \
        { \
        public: \
            type_name() : NodeBase(NodeType::type_name) {} \
            var_list \
        }

        DEF_SYNTAX_TREE_NODE_TYPE(Chunk,
            NodePtr block;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(Block,
            NodePtrList statements;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(FunctionStatement,
            TokenPtr name;
            NodePtr var_name_list;      // NameList
            NodePtr block;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(ReturnStatement,
            NodePtr exprs;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(CallStatement,
            NodePtr func;
            NodePtr expr_list;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(VarNameListStatement,
            NodePtr name_list;
            NodePtr expr_list;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(AssignmentStatement,
            NodePtr var_list;
            NodePtr expr_list;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(IfStatement,
            NodePtr expr;
            NodePtr true_branch;
            NodePtr false_branch;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(ElseStatement,
            NodePtr block;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(WhileStatement,
            NodePtr expr;
            NodePtr block;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(BreakStatement, );

        DEF_SYNTAX_TREE_NODE_TYPE(ForStatement,
            NodePtr var_name_list;
            NodePtr expr;
            NodePtr block;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(ArrayStatement,
            NodePtr expr_list;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(MapStatement,
            NodePtr key_expr_list;
            NodePtr val_expr_list;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(NameList,
            TokenPtrList names;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(VarList,
            NodePtrList vars;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(ExpressionList,
            NodePtrList exprs;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(BinaryExpression,
            NodePtr left;
            NodePtr right;
            TokenPtr op;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(UnaryExpression,
            NodePtr expr;
            TokenPtr op;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(VarExpression,
            NodePtr expr;
            NodePtr key;
        );

        DEF_SYNTAX_TREE_NODE_TYPE(Terminator,
            TokenPtr token;
        );

#undef DEF_SYNTAX_TREE_NODE_TYPE

        static SharedPtr<VarExpression> Node2VarExpression(NodePtr node)
        {
            if (node->node_type == NodeType::Terminator)
            {
                auto terminator = std::static_pointer_cast<SyntaxTree::Terminator>(node);
                if (terminator->token->GetType() != ETokenType::Id)
                {
                    return nullptr;
                }
                auto prefix_expr = MakeShared<VarExpression>();
                auto key_terminator = MakeShared<Terminator>();
                key_terminator->token = TokenT::New(terminator->token->StringValue(), terminator->token->GetLine(), terminator->token->GetColumn());
                prefix_expr->key = key_terminator;
                return prefix_expr;
            }
            else if (node->node_type == NodeType::BinaryExpression)
            {
                auto binary_expr = std::static_pointer_cast<SyntaxTree::BinaryExpression>(node);
                if (binary_expr->op->GetType() != ETokenType::LeftSquareBrace)
                {
                    return nullptr;
                }
                auto prefix_expr = MakeShared<VarExpression>();
                prefix_expr->expr = binary_expr->left;
                prefix_expr->key = binary_expr->right;
                return prefix_expr;
            }
            return nullptr;
        }

#include <iostream>
#include <string>
#include <memory>
        static void DebugPrint(NodePtr node, size_t tab = 0)
        {
            using std::cout;
            using std::endl;
            using std::static_pointer_cast;
            cout << endl;
            switch (node->node_type)
            {
            case NodeType::Chunk:
            {
                cout << std::string(tab, '\t') << "[Chunk]" << endl;
                auto chunk = static_pointer_cast<SyntaxTree::Chunk>(node);
                cout << std::string(tab, '\t') << "block = ";
                DebugPrint(chunk->block, tab + 1);
                break;
            }
            case NodeType::Block:
            {
                cout << std::string(tab, '\t') << "[Block]" << endl;
                auto block = static_pointer_cast<SyntaxTree::Block>(node);
                for (auto iter = block->statements.begin(); iter != block->statements.end(); ++iter)
                {
                    cout << std::string(tab, '\t') << "statement = ";
                    DebugPrint(*iter, tab + 1);
                }
                break;
            }
            case NodeType::FunctionStatement:
            {
                cout << std::string(tab, '\t') << "[FunctionStatement]" << endl;
                auto function_statement = static_pointer_cast<SyntaxTree::FunctionStatement>(node);
                if (function_statement->name)
                {
                    cout << std::string(tab, '\t') << "name = " << function_statement->name->ToString() << endl;
                }
                else
                {
                    cout << std::string(tab, '\t') << "name = <unnamed>" << endl;
                }
                cout << std::string(tab, '\t') << "var_name_list = ";
                DebugPrint(function_statement->var_name_list, tab + 1);
                cout << std::string(tab, '\t') << "block = ";
                DebugPrint(function_statement->block, tab + 1);
                break;
            }
            case NodeType::ReturnStatement:
            {
                cout << std::string(tab, '\t') << "[ReturnStatement]" << endl;
                auto return_statement = static_pointer_cast<SyntaxTree::ReturnStatement>(node);
                cout << std::string(tab, '\t') << "exprs = ";
                DebugPrint(return_statement->exprs, tab + 1);
                break;
            }
            case NodeType::CallStatement:
            {
                cout << std::string(tab, '\t') << "[CallStatement]" << endl;
                auto call_statement = static_pointer_cast<SyntaxTree::CallStatement>(node);
                cout << std::string(tab, '\t') << "func = ";
                DebugPrint(call_statement->func, tab + 1);
                cout << std::string(tab, '\t') << "expr_list = ";
                DebugPrint(call_statement->expr_list, tab + 1);
                break;
            }
            case NodeType::WhileStatement:
            {
                cout << std::string(tab, '\t') << "[WhileStatement]" << endl;
                auto while_statement = static_pointer_cast<SyntaxTree::WhileStatement>(node);
                cout << std::string(tab, '\t') << "expr = ";
                DebugPrint(while_statement->expr, tab + 1);
                cout << std::string(tab, '\t') << "block = ";
                DebugPrint(while_statement->block, tab + 1);
                break;
            }
            case NodeType::BreakStatement:
            {
                cout << std::string(tab, '\t') << "[BreakStatement]" << endl;
                break;
            }
            case NodeType::VarNameListStatement:
            {
                cout << std::string(tab, '\t') << "[VarNameListStatement]" << endl;
                auto var_define_statement = static_pointer_cast<SyntaxTree::VarNameListStatement>(node);
                cout << std::string(tab, '\t') << "name_list = ";
                DebugPrint(var_define_statement->name_list, tab + 1);
                cout << std::string(tab, '\t') << "expr_list = ";
                DebugPrint(var_define_statement->expr_list, tab + 1);
                break;
            }
            case NodeType::AssignmentStatement:
            {
                cout << std::string(tab, '\t') << "[AssignmentStatement]" << endl;
                auto assignment_statement = static_pointer_cast<SyntaxTree::AssignmentStatement>(node);
                cout << std::string(tab, '\t') << "var_list = ";
                DebugPrint(assignment_statement->var_list, tab + 1);
                cout << std::string(tab, '\t') << "expr_list = ";
                DebugPrint(assignment_statement->expr_list, tab + 1);
                break;
            }
            case NodeType::IfStatement:
            {
                cout << std::string(tab, '\t') << "[IfStatement]" << endl;
                auto if_statement = static_pointer_cast<SyntaxTree::IfStatement>(node);
                cout << std::string(tab, '\t') << "expr = ";
                DebugPrint(if_statement->expr, tab + 1);
                cout << std::string(tab, '\t') << "true_branch = ";
                DebugPrint(if_statement->true_branch, tab + 1);
                cout << std::string(tab, '\t') << "false_branch = ";
                DebugPrint(if_statement->false_branch, tab + 1);
                break;
            }
            case NodeType::ElseStatement:
            {
                cout << std::string(tab, '\t') << "[ElseStatement]" << endl;
                auto else_statement = static_pointer_cast<SyntaxTree::ElseStatement>(node);
                cout << std::string(tab, '\t') << "block = ";
                DebugPrint(else_statement->block, tab + 1);
                break;
            }
            case NodeType::NameList:
            {
                cout << std::string(tab, '\t') << "[NameList]" << endl;
                auto name_list = static_pointer_cast<SyntaxTree::NameList>(node);
                for (auto iter = name_list->names.begin(); iter != name_list->names.end(); ++iter)
                {
                    cout << std::string(tab, '\t') << "name = " << (*iter)->ToString() << endl;
                }
                break;
            }
            case NodeType::VarList:
            {
                cout << std::string(tab, '\t') << "[VarList]" << endl;
                auto var_list = static_pointer_cast<SyntaxTree::VarList>(node);
                for (auto iter = var_list->vars.begin(); iter != var_list->vars.end(); ++iter)
                {
                    cout << std::string(tab, '\t') << "var = ";
                    DebugPrint(*iter, tab + 1);
                }
                break;
            }
            case NodeType::ExpressionList:
            {
                cout << std::string(tab, '\t') << "[ExpressionList]" << endl;
                auto expr_list = static_pointer_cast<SyntaxTree::ExpressionList>(node);
                for (auto iter = expr_list->exprs.begin(); iter != expr_list->exprs.end(); ++iter)
                {
                    cout << std::string(tab, '\t') << "expr = ";
                    DebugPrint(*iter, tab + 1);
                }
                break;
            }
            case NodeType::BinaryExpression:
            {
                cout << std::string(tab, '\t') << "[BinaryExpression]" << endl;
                auto expr = static_pointer_cast<SyntaxTree::BinaryExpression>(node);
                cout << std::string(tab, '\t') << "op = " << expr->op->ToString() << endl;
                cout << std::string(tab, '\t') << "left = ";
                DebugPrint(expr->left, tab + 1);
                cout << std::string(tab, '\t') << "right = ";
                DebugPrint(expr->right, tab + 1);
                break;
            }
            case NodeType::UnaryExpression:
            {
                cout << std::string(tab, '\t') << "[UnaryExpression]" << endl;
                auto expr = static_pointer_cast<SyntaxTree::UnaryExpression>(node);
                cout << std::string(tab, '\t') << "op = " << expr->op->ToString() << endl;
                cout << std::string(tab, '\t') << "expr = ";
                DebugPrint(expr->expr, tab + 1);
                break;
            }
            case NodeType::Terminator:
            {
                cout << std::string(tab, '\t') << "[Terminator]" << endl;
                auto terminator = static_pointer_cast<SyntaxTree::Terminator>(node);
                cout << std::string(tab, '\t') << terminator->token->ToString() << endl;
                break;
            }
            default:
                break;
            }
        }
    }
}
