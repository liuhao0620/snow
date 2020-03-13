#pragma once
#include "operator.h"
#include "scanner.h"
#include "syntax_tree.h"

namespace LANG_NS
{
	class Parser
	{
	public:
		Parser(const SharedPtr<Scanner> scanner)
			: _scanner(scanner)
			, _module_name(_scanner ? _scanner->ModuleName() : U"<unknown>")
		{

		}

		SyntaxTree::NodePtr Parse()
		{
			return ParseChunk();
		}
        
        const StringT ModuleName() const
        {
            return _module_name;
        }

	private:
		SyntaxTree::NodePtr Error(const StringT& err_info) const
		{
			StringT err_msg(err_info);
			err_msg += U" near token '";
			err_msg += _current_token->ToString();
			err_msg += U"' in module ";
			err_msg += _module_name;
			err_msg += U" line ";
			err_msg += ToString(_current_token->GetLine());
			err_msg += U" column ";
			err_msg += ToString(_current_token->GetColumn());
			throw(Exception(err_msg));
			return SyntaxTree::NodePtr();
		}

		SharedPtr<TokenT> NextToken()
		{
			if (_look_ahead_token)
			{
				_current_token = _look_ahead_token;
				_look_ahead_token.reset();
			}
			else
			{
				_current_token = _scanner->Scan();
			}
			return _current_token;
		}

		SharedPtr<TokenT> LookAheadToken()
		{
			if (!_look_ahead_token)
			{
				_look_ahead_token = _scanner->Scan();
			}
			return _look_ahead_token;
		}

		SyntaxTree::NodePtr ParseChunk()
		{
			auto chunk = MakeShared<SyntaxTree::Chunk>();
			chunk->block = ParseBlock();
			if (NextToken()->GetType() != ETokenType::Eof)
			{
				return Error(U"Except <eof>");
			}
			return chunk;
		}

		SyntaxTree::NodePtr ParseBlock()
		{
			auto block = MakeShared<SyntaxTree::Block>();
			while (
				LookAheadToken()->GetType() != ETokenType::Eof
				&& LookAheadToken()->GetType() != ETokenType::RightBrace
			)
			{
				auto statement = ParseStatement();
				if (statement)
				{
					block->statements.push_back(statement);
				}
			}
			return block;
		}

		SyntaxTree::NodePtr ParseStatement()
		{
			auto look_ahead_token = LookAheadToken();
			if (look_ahead_token->GetType() == ETokenType::Var)
			{
				return ParseVarNameListStatement();
			}
			else if (look_ahead_token->GetType() == ETokenType::If)
			{
				return ParseIfStatement();
			}
			else if (look_ahead_token->GetType() == ETokenType::While)
			{
				return ParseWhileStatement();
			}
			else if (look_ahead_token->GetType() == ETokenType::Break)
			{
				return ParseBreakStatement();
			}
			else if (look_ahead_token->GetType() == ETokenType::For)
			{
				return ParseForStatement();
			}
			else if (look_ahead_token->GetType() == ETokenType::LeftBrace)
			{
				(void)NextToken();
				auto block = ParseBlock();
				if (NextToken()->GetType() != ETokenType::RightBrace)
				{
					return Error(U"Except '}'");
				}
				return block;
			}
			else if (look_ahead_token->GetType() == ETokenType::Function)
			{
				return ParseFunctionStatement();
			}
			else if (look_ahead_token->GetType() == ETokenType::Return)
			{
				return ParseReturnStatement();
			}
			else
			{
				return ParseOtherStatement();
			}
			return SyntaxTree::NodePtr();
		}

		SyntaxTree::NodePtr ParseVarNameListStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::Var);
			auto var_define_statement = MakeShared<SyntaxTree::VarNameListStatement>();
			if (LookAheadToken()->GetType() != ETokenType::Id)
			{
				return Error(U"Unexcept Token after 'var'");
			}
			var_define_statement->name_list = ParseNameList();
			if (LookAheadToken()->GetType() == ETokenType::Assign)
			{
				(void)NextToken();
				var_define_statement->expr_list = ParseExpressionList();
			}
			return var_define_statement;
		}

		SyntaxTree::NodePtr ParseIfStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::If);
			auto if_statement = MakeShared<SyntaxTree::IfStatement>();
			if (NextToken()->GetType() != ETokenType::LeftParen)
			{
				return Error(U"Except '('");
			}
			if_statement->expr = ParseExpression();
			if (NextToken()->GetType() != ETokenType::RightParen)
			{
				return Error(U"Except ')'");
			}
			if (NextToken()->GetType() != ETokenType::LeftBrace)
			{
				return Error(U"Except '{'");
			}
			if_statement->true_branch = ParseBlock();
			if (NextToken()->GetType() != ETokenType::RightBrace)
			{
				return Error(U"Except '}'");
			}
			if (LookAheadToken()->GetType() == ETokenType::Else)
			{
				(void)NextToken();
				if (LookAheadToken()->GetType() == ETokenType::If)
				{
					if_statement->false_branch = ParseIfStatement();
				}
				else if (LookAheadToken()->GetType() == ETokenType::LeftBrace)
				{
					if_statement->false_branch = ParseElseStatement();
				}
				else
				{
					return Error(U"Unexcept Token");
				}
			}
			return if_statement;
		}

		SyntaxTree::NodePtr ParseElseStatement()
		{
			Assert(_current_token->GetType() == ETokenType::Else);
			auto else_statement = MakeShared<SyntaxTree::ElseStatement>();
			if (NextToken()->GetType() != ETokenType::LeftBrace)
			{
				return Error(U"Except '{'");
			}
			else_statement->block = ParseBlock();
			if (NextToken()->GetType() != ETokenType::RightBrace)
			{
				return Error(U"Except '}'");
			}
			return else_statement;
		}

		SyntaxTree::NodePtr ParseWhileStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::While);
			auto while_statement = MakeShared<SyntaxTree::WhileStatement>();
			if (NextToken()->GetType() != ETokenType::LeftParen)
			{
				return Error(U"Except '('");
			}
			while_statement->expr = ParseExpression();
			if (NextToken()->GetType() != ETokenType::RightParen)
			{
				return Error(U"Except ')'");
			}
			if (NextToken()->GetType() != ETokenType::LeftBrace)
			{
				return Error(U"Except '{'");
			}
			bool old_in_loop_flag = _in_loop;
			_in_loop = true;
			while_statement->block = ParseBlock();
			_in_loop = old_in_loop_flag;
			if (NextToken()->GetType() != ETokenType::RightBrace)
			{
				return Error(U"Except '}'");
			}
			return while_statement;
		}

		SyntaxTree::NodePtr ParseBreakStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::Break);
			if (!_in_loop)
			{
				return Error(U"Unexcept 'break'");
			}
			return MakeShared<SyntaxTree::BreakStatement>();
		}

		SyntaxTree::NodePtr ParseForStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::For);
			auto for_statement = MakeShared<SyntaxTree::ForStatement>();
			if (NextToken()->GetType() != ETokenType::LeftParen)
			{
				return Error(U"Except '('");
			}
			for_statement->var_name_list = ParseNameList();
			if (NextToken()->GetType() != ETokenType::In)
			{
				return Error(U"Except 'in'");
			}
			for_statement->expr = ParseExpression();
			if (NextToken()->GetType() != ETokenType::RightParen)
			{
				return Error(U"Except ')'");
			}
			if (NextToken()->GetType() != ETokenType::LeftBrace)
			{
				return Error(U"Except '{'");
			}
			bool old_in_loop_flag = _in_loop;
			_in_loop = true;
			for_statement->block = ParseBlock();
			_in_loop = old_in_loop_flag;
			if (NextToken()->GetType() != ETokenType::RightBrace)
			{
				return Error(U"Except '}'");
			}
			return for_statement;
		}

		SyntaxTree::NodePtr ParseArrayStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::LeftSquareBrace);
			auto array_statement = MakeShared<SyntaxTree::ArrayStatement>();
			if (LookAheadToken()->GetType() != ETokenType::RightSquareBrace)
			{
				array_statement->expr_list = ParseExpressionList();
			}
			if (NextToken()->GetType() != ETokenType::RightSquareBrace)
			{
				return Error(U"Except ']'");
			}
			return array_statement;
		}

		SyntaxTree::NodePtr ParseMapStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::LeftBrace);
			auto map_statement = MakeShared<SyntaxTree::MapStatement>();
			auto key_expr_list = MakeShared<SyntaxTree::ExpressionList>();
			auto val_expr_list = MakeShared<SyntaxTree::ExpressionList>();
			while (
				LookAheadToken()->GetType() == ETokenType::LeftSquareBrace
				|| LookAheadToken()->GetType() == ETokenType::Id
			)
			{
				if (LookAheadToken()->GetType() == ETokenType::LeftSquareBrace)
				{
					(void)NextToken();
					key_expr_list->exprs.push_back(ParseExpression());
					if (NextToken()->GetType() != ETokenType::RightSquareBrace)
					{
						return Error(U"Except ']'");
					}
				}
				else if (LookAheadToken()->GetType() == ETokenType::Id)
				{
					(void)NextToken();
					auto terminator = MakeShared<SyntaxTree::Terminator>();
					terminator->token = TokenT::New(_current_token->StringValue(), _current_token->GetLine(), _current_token->GetColumn());
					key_expr_list->exprs.push_back(terminator);
				}

				if (NextToken()->GetType() != ETokenType::Assign)
				{
					return Error(U"Except '='");
				}
				val_expr_list->exprs.push_back(ParseExpression());
				if (LookAheadToken()->GetType() != ETokenType::Comma)
				{
					break;
				}
				(void)NextToken();
			}
			map_statement->key_expr_list = key_expr_list;
			map_statement->val_expr_list = val_expr_list;
			if (NextToken()->GetType() != ETokenType::RightBrace)
			{
				return Error(U"Except '}'");
			}
			return map_statement;
		}

		SyntaxTree::NodePtr ParseFunctionStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::Function);
			auto function_statement = MakeShared<SyntaxTree::FunctionStatement>();
			if (LookAheadToken()->GetType() == ETokenType::Id)
			{
				function_statement->name = NextToken()->Clone();
			}
			if (NextToken()->GetType() != ETokenType::LeftParen)
			{
				return Error(U"Except '('");
			}
			function_statement->var_name_list = ParseNameList();
			if (NextToken()->GetType() != ETokenType::RightParen)
			{
				return Error(U"Except ')'");
			}
			if (NextToken()->GetType() != ETokenType::LeftBrace)
			{
				return Error(U"Except '{'");
			}
			function_statement->block = ParseBlock();
			if (NextToken()->GetType() != ETokenType::RightBrace)
			{
				return Error(U"Except '}'");
			}
			return function_statement;
		}

		SyntaxTree::NodePtr ParseReturnStatement()
		{
			Assert(NextToken()->GetType() == ETokenType::Return);
			auto return_statement = MakeShared<SyntaxTree::ReturnStatement>();
			return_statement->exprs = ParseExpressionList();
			return return_statement;
		}

		SyntaxTree::NodePtr ParseCallStatement(SyntaxTree::NodePtr func)
		{
			Assert(NextToken()->GetType() == ETokenType::LeftParen);
			auto call_statement = MakeShared<SyntaxTree::CallStatement>();
			call_statement->func = func;
			if (LookAheadToken()->GetType() != ETokenType::RightParen)
			{
				call_statement->expr_list = ParseExpressionList();
			}
			else
			{
				call_statement->expr_list = MakeShared<SyntaxTree::ExpressionList>();
			}
			
			if (NextToken()->GetType() != ETokenType::RightParen)
			{
				return Error(U"Except ')'");
			}
			return call_statement;
		}

		SyntaxTree::NodePtr ParseAssignmentStatement(SyntaxTree::NodePtr first_var)
		{
			auto assignment_statement = MakeShared<SyntaxTree::AssignmentStatement>();
			auto var_list = MakeShared<SyntaxTree::VarList>();
			auto prefix_expr = SyntaxTree::Node2VarExpression(first_var);
			if (!prefix_expr)
			{
				return Error(U"Not a left value");
			}
			var_list->vars.push_back(prefix_expr);
			while (LookAheadToken()->GetType() == ETokenType::Comma)
			{
				(void)NextToken();
				prefix_expr = SyntaxTree::Node2VarExpression(ParseExpression());
				if (!prefix_expr)
				{
					return Error(U"Not a left value");
				}
				var_list->vars.push_back(prefix_expr);
			}
			assignment_statement->var_list = var_list;
			if (LookAheadToken()->GetType() != ETokenType::Assign)
			{
				return Error(U"Except '='");
			}
			(void)NextToken();
			assignment_statement->expr_list = ParseExpressionList();
			return assignment_statement;
		}

		SyntaxTree::NodePtr ParseOtherStatement()
		{
			if (LookAheadToken()->GetType() != ETokenType::Id)
			{
				return Error(U"Except a <id>");
			}
			auto expr = ParseExpression();
			if (
				LookAheadToken()->GetType() == ETokenType::Comma
				|| LookAheadToken()->GetType() == ETokenType::Assign
			)
			{
				return ParseAssignmentStatement(expr);
			}
			return expr;
		}

		SyntaxTree::NodePtr ParseNameList()
		{
			auto name_list = MakeShared<SyntaxTree::NameList>();
			if (LookAheadToken()->GetType() == ETokenType::RightParen)
			{
				return name_list;
			}
			if (NextToken()->GetType() != ETokenType::Id)
			{
				return Error(U"Except a <id>");
			}
			name_list->names.push_back(_current_token->Clone());
			while (LookAheadToken()->GetType() == ETokenType::Comma)
			{
				(void)NextToken();
				if (NextToken()->GetType() != ETokenType::Id)
				{
					return Error(U"Except a <id> after ','");
				}
				name_list->names.push_back(_current_token->Clone());
			}
			return name_list;
		}

		SyntaxTree::NodePtr ParseExpressionList()
		{
			auto expr_list = MakeShared<SyntaxTree::ExpressionList>();
			while (true)
			{
				expr_list->exprs.push_back(ParseExpression());
				if (LookAheadToken()->GetType() != ETokenType::Comma)
				{
					break;
				}
				(void)NextToken();
			}
			return expr_list;
		}

		SyntaxTree::NodePtr ParseExpression()
		{
			return ParseExprImpl();
		}

		SyntaxTree::NodePtr ParseExprImpl()
		{
			if (LookAheadToken()->GetType() == ETokenType::If)
			{
				return ParseIfStatement();
			}
			else if (LookAheadToken()->GetType() == ETokenType::Function)
			{
				return ParseFunctionStatement();
			}
			else if (LookAheadToken()->GetType() == ETokenType::LeftSquareBrace)
			{
				return ParseArrayStatement();
			}
			else if (LookAheadToken()->GetType() == ETokenType::LeftBrace)
			{
				return ParseMapStatement();
			}
			SyntaxTree::NodePtrList expr_list;
			SyntaxTree::TokenPtrList op_list;
			do
			{
				SyntaxTree::NodePtr expr;
				if (LookAheadToken()->GetType() == ETokenType::LeftParen)
				{
					(void)NextToken();
					expr = ParseExprImpl();
					if (NextToken()->GetType() != ETokenType::RightParen)
					{
						return Error(U"Except ')'");
					}
				}
				else
				{
					expr = ParseSingleExpr();
				}
				expr_list.push_back(expr);
				if (!Operator::GetPrio(LookAheadToken()->GetType()))
				{
					break;
				}
				op_list.push_back(NextToken()->Clone());
			} while (true);
			for (SizeT i = Operator::MinPrio(); i <= Operator::MaxPrio(); ++ i)
			{
				Assert(expr_list.size() == op_list.size() + 1);
				if (op_list.empty())
				{
					break;
				}
				for (SizeT j = 0; j < op_list.size();)
				{
					Assert(expr_list.size() == op_list.size() + 1);
					if (*Operator::GetPrio(op_list[j]->GetType()) == i)
					{
						expr_list[j + 1] = MakeBinaryExpr(op_list[j], expr_list[j], expr_list[j + 1]);
						op_list.erase(op_list.begin() + j);
						expr_list.erase(expr_list.begin() + j);
					}
					else
					{
						++ j;
					}
				}
			}
			return *expr_list.begin();
		}

		SyntaxTree::NodePtr ParseSingleExpr()
		{
			if (
				LookAheadToken()->GetType() == ETokenType::Not
				|| LookAheadToken()->GetType() == ETokenType::BitwiseNot
				|| LookAheadToken()->GetType() == ETokenType::Add
				|| LookAheadToken()->GetType() == ETokenType::Sub
			)
			{
				auto unary_expr = MakeShared<SyntaxTree::UnaryExpression>();
				unary_expr->op = NextToken()->Clone();
				unary_expr->expr = ParseSingleExpr();
				return unary_expr;
			}
			else if (
				LookAheadToken()->GetType() == ETokenType::Bool
				|| LookAheadToken()->GetType() == ETokenType::Int
				|| LookAheadToken()->GetType() == ETokenType::Float
				|| LookAheadToken()->GetType() == ETokenType::String
				|| LookAheadToken()->GetType() == ETokenType::Nil
			)
			{
				return ParseTerminator();
			}
			else if (LookAheadToken()->GetType() == ETokenType::Id)
			{
				return ParseFirstPrioExpr();
			}
			return Error(U"Unexcept token");
		}

		SyntaxTree::NodePtr ParseFirstPrioExpr()
		{
			auto terminator = ParseTerminator();
			SyntaxTree::NodePtr expr = terminator;
			while (
				LookAheadToken()->GetType() == ETokenType::LeftParen
				|| LookAheadToken()->GetType() == ETokenType::LeftSquareBrace
				|| LookAheadToken()->GetType() == ETokenType::Dot
			)
			{
				if (LookAheadToken()->GetType() == ETokenType::LeftParen)
				{
					expr = ParseCallStatement(expr);
				}
				else if (LookAheadToken()->GetType() == ETokenType::LeftSquareBrace)
				{
					auto binary_expr = MakeShared<SyntaxTree::BinaryExpression>();
					binary_expr->op = NextToken()->Clone();
					binary_expr->left = expr;
					binary_expr->right = ParseExpression();
					if (NextToken()->GetType() != ETokenType::RightSquareBrace)
					{
						throw(Exception(U"Except ']'"));
						return {};
					}
					expr = binary_expr;
				}
				else if (LookAheadToken()->GetType() == ETokenType::Dot)
				{
					(void)NextToken();
					auto binary_expr = MakeShared<SyntaxTree::BinaryExpression>();
					binary_expr->op = TokenT::New(ETokenType::LeftSquareBrace, _current_token->GetLine(), _current_token->GetColumn());
					binary_expr->left = expr;
					if (NextToken()->GetType() != ETokenType::Id)
					{
						throw(Exception(U"Except a Id Token"));
						return {};
					}
					auto terminator = MakeShared<SyntaxTree::Terminator>();
					terminator->token = TokenT::New(_current_token->StringValue(), _current_token->GetLine(), _current_token->GetColumn());
					binary_expr->right = terminator;
					expr = binary_expr;
				}
			}
			return expr;
		}

		SyntaxTree::NodePtr MakeBinaryExpr(SyntaxTree::TokenPtr op, SyntaxTree::NodePtr left, SyntaxTree::NodePtr right)
		{
			auto binary_expr = MakeShared<SyntaxTree::BinaryExpression>();
			binary_expr->op = op;
			binary_expr->left = left;
			binary_expr->right = right;
			return binary_expr;
		}

		SyntaxTree::NodePtr ParseTerminator()
		{
			auto terminator = MakeShared<SyntaxTree::Terminator>();
			terminator->token = NextToken();
			return terminator;
		}

	private:
		SharedPtr<Scanner> _scanner;
		StringT _module_name;
		SharedPtr<TokenT> _current_token;
		SharedPtr<TokenT> _look_ahead_token;
		bool _in_loop = false;
	};
}
