#pragma once
#include "../helpers/array_list.h"
#include "tokenizer.h"
#include "eval.h"


using token::Token;
using lex::Tokenizer;
using stmt_::Stmt;
using stmt_::Expr_stmt;
using stmt_::If_stmt;
using stmt_::Out_stmt;
using stmt_::Block_stmt;
using stmt_::Assign_stmt;
using stmt_::While_stmt;
using stmt_::Mut_decl;
using stmt_::Let_decl;
using stmt_::Proc_decl;
using expr_::Expr;
using expr_::Binary_expr;
using expr_::Literal_expr;
using expr_::Identifier_expr;
using expr_::Unary_expr;
using expr_::Paren_expr;
using expr_::Callee_expr;

extern void compile_error(TokenKind kind, void* object, string message, int line, int pos);

namespace parser {

	class Parser {
	public:
		Parser(ArrayList<Token>&);
		ArrayList<Stmt*> parse();
	private:
		ArrayList<Token> tokens;
		ArrayList<Stmt*> statements;
		Stmt* decl();
		Stmt* mut_decl();
		Stmt* let_decl();
		Stmt* proc_decl();
		Stmt* statement();
		Stmt* block_stmt();
		Stmt* assign_stmt();
		Stmt* out_stmt();
		Stmt* while_stmt();
		Stmt* for_stmt();
		Stmt* if_stmt();
		Stmt* expr_stmt();
		Expr* equality();
		Expr* logic_or();
		Expr* logic_and();
		Expr* comparison();
		Expr* mult();
		Expr* add();
		Expr* unary();
		Expr* callee();
		Expr* literal();
		//helpers
		size_t count_track;
		bool tokens_end() {
			return count_track >= tokens.size();
		}

		Token prev() {
			return tokens.get(count_track - 1);
		}

		Token move() {
			count_track++;
			return tokens.get(count_track);
		}
		Token peek() {
			return peek(0);
		}
		Token peek(int pos) {
			return tokens.get(count_track + pos);
		}
		Token expect_token(TokenKind type, string message) {
			if (!tokens_end() && peek().get_token_kind() != type) {
				compile_error(type, nullptr, message,peek().get_line(), peek().get_position());
				return peek();
			}
			else if (!tokens_end()) {
				move();
				return prev();
			}

		}
		const int get_token_count() const {
			return count_track;
		}

		bool match(TokenKind expected) {
			if (peek().get_token_kind() == expected) {
				move();
				return true;
			}
			return false;
		}

		void sync_error() {

		}
	};

}//end namespace parser