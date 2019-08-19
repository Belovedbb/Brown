#include "parser.h"

namespace parser {

	Parser::Parser(ArrayList<Token>& tokens) :tokens(tokens)
	{
		count_track = 0;
	}

	ArrayList<Stmt*> Parser::parse()
	{
		while(get_token_count() < tokens.size()-1) 
			statements.add(decl());
		return statements;
	}

	Stmt* Parser::decl() {
		if (match(TokenKind::MUT)) {
			Stmt* mut = mut_decl();
			expect_token(TokenKind::SEMICOLON, "expected token ';'");
			return mut;
		}
			
		else if (match(TokenKind::LET)) {
			Stmt* let = let_decl();
			expect_token(TokenKind::SEMICOLON, "expected token ';'");
			return let;
		}
			
		else if (match(TokenKind::PROC))
			return proc_decl();
		return statement();
	}
	
	
	Stmt* Parser::mut_decl() {
		Token identifier = expect_token(TokenKind::IDENTIFIER, "expected an identifier");
		Stmt* initializer = nullptr;
		if (match(TokenKind::EQUAL)) {
			initializer = expr_stmt();
		}
		
		return new Mut_decl(identifier, initializer);
	}

	Stmt* Parser::let_decl() {
		Token identifier = expect_token(TokenKind::IDENTIFIER, "expected an identifier");
		expect_token(TokenKind::EQUAL, "expected token '='");
		Stmt* initializer = expr_stmt();
		
		return new Let_decl(identifier, initializer);
	}

	Stmt* Parser::proc_decl() {
		Token identifier = expect_token(TokenKind::IDENTIFIER, "expected an identifier");
		expect_token(TokenKind::LEFT_PAREN, "expect token '('");
		ArrayList<Stmt*> params;
		while (peek().get_token_kind() != TokenKind::RIGHT_PAREN) {
			//move to consume mut decl
			move();
			params.add(mut_decl());
			if (peek().get_token_kind() == TokenKind::COMMA)
				expect_token(TokenKind::COMMA, "expect token ','");
			else {
				break;
			}
				
		}
		expect_token(TokenKind::RIGHT_PAREN, "expect token ')'");
		if (params.size() > 255)
			expect_token(TokenKind(-1), "cannot have more than 255 parameters");
		//consume left brace for block stmt
		expect_token(TokenKind::LEFT_BRACE, "expected token '{'");
		Stmt* body = block_stmt();
		return new Proc_decl(identifier, params, body);
	}

	Stmt* Parser::statement() {
		if (match(TokenKind::IF))
			return if_stmt();
		else if (match(TokenKind::LEFT_BRACE))
			return block_stmt();
		else if (match(TokenKind::OUT))
			return out_stmt();
		else if (match(TokenKind::WHILE))
			return while_stmt();
		else if (match(TokenKind::FOR))
			return for_stmt();
		else if ((peek().get_token_kind() == TokenKind::IDENTIFIER) && peek(1).get_token_kind() == TokenKind::EQUAL) {
			move();
			return assign_stmt();
		}
		else {
			Stmt* expr_stmt_ = expr_stmt();
			expect_token(TokenKind::SEMICOLON, "expected token ';'");
			return expr_stmt_;
		}
		
	}

	

	//outstatement -> "out" exprstatement ( ("," exprstatement)* )? ";";
	Stmt* Parser::out_stmt() {
		Stmt* expr_stmt_ = expr_stmt();
		while (peek().get_token_kind() == TokenKind::COMMA) {
			expect_token(TokenKind::COMMA, "expected a token ','");
			Stmt* new_expr_stmt_ = expr_stmt();
			//expr_stmt_ = new Out_stmt(*expr_stmt_);
		}
		expect_token(TokenKind::SEMICOLON, "expected a token ';'");
		return new Out_stmt(*expr_stmt_);
	}

	Stmt* Parser::if_stmt() {
		expect_token(TokenKind::LEFT_PAREN, "expected token '(' ");
		Stmt* condition = expr_stmt();
		expect_token(TokenKind::RIGHT_PAREN, "expected token ')' ");
		Stmt* body = statement();
		Stmt* else_cond = nullptr;
		if (match(TokenKind::ELSE)) {
			else_cond = statement();
		}
		return new If_stmt(*condition, *body, else_cond);
	}

	Stmt* Parser::while_stmt() {
		expect_token(TokenKind::LEFT_PAREN, "expected token '(' ");
		Stmt* condition = expr_stmt();
		expect_token(TokenKind::RIGHT_PAREN, "expected token ')' ");
		Stmt*  body = statement();
		return new While_stmt(*condition, *body);
	}
	//"for" "(" ( mutdeclaration | IDENTIFIER) ";" exprstatement ";" exprstatement ";" ")" declaration+ ;
	Stmt* Parser::for_stmt() {
		expect_token(TokenKind::LEFT_PAREN, "expected token '(' ");
		Stmt* initializer;
		if (peek().get_token_kind() == TokenKind::IDENTIFIER) {
			initializer = expr_stmt();
			expect_token(TokenKind::SEMICOLON, "expected token ';'");
		}
		else if (peek().get_token_kind() == TokenKind::MUT)
			initializer = decl();
		else {
			initializer = nullptr;
			expect_token(TokenKind::SEMICOLON, "expected token ';'");
		}
		
		Stmt* condition = nullptr;
		if (peek().get_token_kind() != TokenKind::SEMICOLON)
			condition = expr_stmt();
		else
			;
		expect_token(TokenKind::SEMICOLON, "expected token ';'");
		Stmt* increment = nullptr;
		if (peek().get_token_kind() != TokenKind::RIGHT_PAREN)
			increment = assign_stmt();
		expect_token(TokenKind::RIGHT_PAREN, "expect token ')'");
		Stmt* body = decl();
		ArrayList<Stmt*> stmts;
		stmts.add(body);
		stmts.add(increment);
		body = new Block_stmt(stmts);
		body = new  While_stmt(*condition, *body);

		if (initializer != nullptr) {
			ArrayList<Stmt*> stmts_zip;
			stmts_zip.add(initializer);
			stmts_zip.add(body);
			body = new Block_stmt(stmts_zip);
		}
			
		return body;
	}

	//assignmentstatement -> (IDENTIFIER "=")+ exprstatement ";";
	Stmt* Parser::assign_stmt() {
		Token identifier;
		//patch: identifier off by one bug in decl() start and inline parsing
		if (peek().get_token_kind() == TokenKind::IDENTIFIER) {
			identifier = peek();
			move();
		}
		else if(prev().get_token_kind() == TokenKind::IDENTIFIER)
			identifier = prev();
		Stmt* value = nullptr;
		expect_token(TokenKind::EQUAL, "expected token '='");
		while (peek().get_token_kind() == TokenKind::IDENTIFIER && (peek(1).get_token_kind() == TokenKind::EQUAL)) {
			Token identifier2 = peek();
			move();
			//series of identifiers
		}
	    value = expr_stmt();
		expect_token(TokenKind::SEMICOLON, "expected a token ';' ");
		return new Assign_stmt(identifier, *value);
	}

	Stmt* Parser::block_stmt() {
		ArrayList<Stmt*> inner_stmts;
		while (peek().get_token_kind() != TokenKind::RIGHT_BRACE) {
			inner_stmts.add(decl());
		}
		expect_token(TokenKind::RIGHT_BRACE, "expected token '}' ");
		return new Block_stmt(inner_stmts);
	}


	Stmt* Parser::expr_stmt() {
		Stmt* stmt = new Expr_stmt(*logic_or());
		return stmt;
	}

	Expr* Parser::logic_or() {
		Expr* expr = logic_and();
		while (peek().get_token_kind() == TokenKind::OR) {
			Token operator_ = peek();
			move();
			Expr* right = logic_and();
			expr = new Binary_expr(*expr, operator_, *right);
		}
		return expr;
	}

	Expr* Parser::logic_and() {
		Expr* expr = equality();
		while (peek().get_token_kind() == TokenKind::AND) {
			Token operator_ = peek();
			move();
			Expr* right = equality();
			expr = new Binary_expr(*expr, operator_, *right);
		}
		return expr;
	}

	Expr* Parser::equality() {
		Expr* expr = comparison();
		while ((peek().get_token_kind() == TokenKind::EQUAL_EQUAL) || (peek().get_token_kind() == TokenKind::NOT_EQUAL)) {
			Token operator_ = peek();
			move();
			Expr* right = comparison();
			expr = new Binary_expr(*expr, operator_, *right);
		}
		return expr;
	}

	Expr* Parser::comparison() {
		Expr* expr = add();
		while ( (peek().get_token_kind() == TokenKind::LESS) || (peek().get_token_kind() == TokenKind::LESS_EQUAL) ||
			(peek().get_token_kind() == TokenKind::GREATER_EQUAL) || (peek().get_token_kind() == TokenKind::GREATER) ) {
			Token operator_ = peek();
			move();
			Expr* right = add();
			expr = new Binary_expr(*expr, operator_, *right);
		}
		return expr;
	}
	Expr* Parser::add() {
		Expr* expr = mult();
		while ((peek().get_token_kind() == TokenKind::ADD) || (peek().get_token_kind() == TokenKind::MINUS)) {
			Token operator_ = peek();
			move();
			Expr* right = mult();
			expr = new Binary_expr(*expr, operator_, *right);
		}
		return expr;
	}

	Expr* Parser::mult() {
		Expr* expr = unary();
		while ((peek().get_token_kind() == TokenKind::MULT) || (peek().get_token_kind() == TokenKind::SLASH)) {
			Token operator_ = peek();
			move();
			Expr* right = unary();
			expr = new Binary_expr(*expr, operator_, *right);
		}
		return expr;
	}
	
	Expr* Parser::unary() {
		while ((peek().get_token_kind() == TokenKind::MINUS) || (peek().get_token_kind() == TokenKind::NOT)) {
			Token operator_ = peek();
			move();
			return new Unary_expr(operator_, *unary());
		}
		return callee();
	}

	Expr* Parser::callee() {
		ArrayList<Expr*> args;
		Expr* name = literal();
		if (peek().get_token_kind() != TokenKind::LEFT_PAREN)
			return name;
		else {
			Token name_ = prev();
			expect_token(TokenKind::LEFT_PAREN, "expected token '('");
			while (peek().get_token_kind() != TokenKind::RIGHT_PAREN){
				args.add(literal());
				if (peek().get_token_kind() == TokenKind::COMMA) {
					expect_token(TokenKind::COMMA, "expect token ','");
				}
				else
					break;
			}
			if (args.size() > 255)
				expect_token(TokenKind(-1), "argument length should be less than 255");
			expect_token(TokenKind::RIGHT_PAREN, "expected token ')'");
			return new Callee_expr(name_, args);
		}
	}

	Expr* Parser::literal() {
		if ((peek().get_token_kind() == TokenKind::NUMBER) ) {
			move();
			return new Literal_expr(prev().get_value());
		}
		else if (peek().get_token_kind() == TokenKind::TRUE) {
			move();
			return new Literal_expr(true);
		}
		else if (peek().get_token_kind() == TokenKind::FALSE) {
			move();
			return new Literal_expr(false);
		}
		else if (peek().get_token_kind() == TokenKind::STRING) {
			move();
			return new Literal_expr(prev().get_value());
		}
		else if ((peek().get_token_kind() == TokenKind::NIL)) {
			move();
			return new Literal_expr(NULL);
		}
		else if (peek().get_token_kind() == TokenKind::IDENTIFIER) {
			move();
			return new Identifier_expr(prev().get_lexeme());
		}
		else if (peek().get_token_kind() == TokenKind::LEFT_PAREN) {
			move();
			Expr* expr =  equality();
			expect_token(TokenKind::RIGHT_PAREN, "expected token ')' ");
			return new Paren_expr(*expr);
		}
		else {
			expect_token(TokenKind(-1), "expected a token");
		}

	}

}//end namespace parser;