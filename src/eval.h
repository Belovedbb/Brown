#pragma once

#include "callable.h"
#include <cassert>

using calls_interface::Procedure_callable;

inline Environment global;

namespace expr_ {

	class Expr_visitor;

	class Expr {
	public:
		virtual any accept(Expr_visitor& visitor) = 0;
	};

	class Binary_expr;
	class Unary_expr;
	class Literal_expr;
	class Paren_expr;
	class Identifier_expr;
	class Callee_expr;

	class Expr_visitor {
	public:
		virtual any visit(Binary_expr& binary) = 0;
		virtual any visit(Unary_expr& unary) = 0;
		virtual any visit(Literal_expr& literal) = 0;
		virtual any visit(Paren_expr& paren) = 0;
		virtual any visit(Identifier_expr& identifier) = 0;
		virtual any visit(Callee_expr& callee) = 0;
		//more implements
	};


	class Binary_expr : public Expr {
	public:
		Binary_expr(Expr& left, Token operator_, Expr& right): left(left), operator_(operator_), right(right){}

		any accept(Expr_visitor& visitor) override{
			return visitor.visit(*this);
		}
		Expr& get_left() {
			return left;
		}
		Expr& get_right() {
			return right;
		}
		Token get_operator_() {
			return operator_;
		}
	private:
		Expr& left;
		Token operator_;
		Expr& right;
	};

	class Unary_expr : public Expr {
	public:
		Unary_expr(Token operator_, Expr& literal) : operator_(operator_), literal(literal) {}

		any accept(Expr_visitor& visitor) override {
			return visitor.visit(*this);
		}
		Token get_operator() {
			return operator_;
		}
		Expr& get_literal() {
			return literal;
		}
	private:
		Token operator_;
		Expr& literal;
	};
	
	class Literal_expr : public Expr {
	public:
		any accept(Expr_visitor& visitor) override {
			return visitor.visit(*this);
		}
		Literal_expr(any object) : object(object){
		}

		any get_object() {
			return object;
		}

	private:
		any object;
	};

	class Identifier_expr : public Expr {
	public:
		any accept(Expr_visitor& visitor) override {
			return visitor.visit(*this);
		}
		Identifier_expr(string identifier) : identifier(identifier) {
		}

		string get_identifier() {
			return identifier;
		}

	private:
		string identifier;
	};

	class Paren_expr : public Expr {
	public:
		any accept(Expr_visitor& visitor) override {
			return visitor.visit(*this);
		}
		Paren_expr(Expr& expr_object) : expr_object(expr_object) {
		}

		Expr& get_expr_object() {
			return expr_object;
		}

	private:
		Expr& expr_object;
	};

	class Callee_expr : public Expr {
	public:
		any accept(Expr_visitor& visitor) override {
			return visitor.visit(*this);
		}

		Callee_expr(Token caller_name, ArrayList<Expr*> args_list) : args_list(args_list), caller_name(caller_name) {
		}

		ArrayList<Expr*> get_args_list(){
			return args_list;
		}
		Token get_name() {
			return caller_name;
		}

	private:
		ArrayList<Expr*> args_list;
		Token caller_name;
	};

	class Expr_visitor_impl : public Expr_visitor{
	public:

		any visit(Binary_expr& binary) override{
			any left = binary.get_left().accept(*this);
			Token token = binary.get_operator_();
			any right = binary.get_right().accept(*this);

			if( (left.type() == typeid(double)) && (right.type() == typeid(double)) ){
				if(token.get_token_kind() == TokenKind::ADD || token.get_token_kind() == TokenKind::MINUS
					|| token.get_token_kind() == TokenKind::MULT || token.get_token_kind() == TokenKind::SLASH)
					return double_help(any_cast<double>(left), token, any_cast<double>(right) );
				if (token.get_token_kind() == TokenKind::GREATER || token.get_token_kind() == TokenKind::GREATER_EQUAL
					|| token.get_token_kind() == TokenKind::LESS || token.get_token_kind() == TokenKind::LESS_EQUAL
					|| token.get_token_kind() == TokenKind::EQUAL_EQUAL || token.get_token_kind() == TokenKind::NOT_EQUAL
					|| token.get_token_kind() == TokenKind::OR || token.get_token_kind() == TokenKind::AND)
					return bool_help(any_cast<double>(left), token, any_cast<double>(right));
			}
			else if ((left.type() == typeid(string)) && (right.type() == typeid(string)))
				return string_help(any_cast<string>(left), token, any_cast<string>(right));
			else
			return NULL;
		}

		any visit(Unary_expr& unary) override {
			any expr = unary.get_literal().accept(*this);
			Token operator_ = unary.get_operator();
			if (operator_.get_token_kind() == TokenKind::MINUS) {
				if (expr.type() != typeid(double)) {
					//handle exception
				}
				else
					return (- any_cast<double>(expr));
			}
			else if (operator_.get_token_kind() == TokenKind::NOT) {
				return !eval_truthfulness(expr);
			}
			else {
				return "nil";
			}
		}

		any visit(Literal_expr& literal) override {
			return literal.get_object();
		}

		any visit(Paren_expr& paren_expr) override {
			return paren_expr.get_expr_object().accept(*this);
		}

		any visit(Identifier_expr& identifier) override {
			if (global.contains_identifier(identifier.get_identifier() ))
				//&& global.get_entry(identifier.get_identifier()) != NULL
				return global.get_entry(identifier.get_identifier());
			else
				;
				//handle runtime exception
		}

		any visit(Callee_expr& callee) override {
			any name = callee.get_name().get_value();
			ArrayList<any> evaluated_args;
			for (int i = 0; i < callee.get_args_list().size(); i++) {
				evaluated_args.add(callee.get_args_list().get(i)->accept(*this));
			}

			if (name.type() != typeid(string)) {
				//handle "" instance too, runtime error
				return NULL;
			}
			string name_ = any_cast<string>(name);
			env::Var_proc_type proc_type = global.get_identifier_verification().get_value(name_);
			if (proc_type != env::Var_proc_type::PROC){
				//handle runtime error : not declared procedure
				return NULL;
			}
			
			//downcasting to Procedure klass
			auto proc = any_cast<Procedure_callable*>(global.get_entry(name_));//used the interface coz of cyclic namespacing prob
			if (evaluated_args.size() != proc->arity()) {
				//handle runtime error: procedure parameter does not match argument
				return NULL;
			}
			return proc->call(global, evaluated_args);
		}

	private:
		//helper
		double double_help(const double& left, const Token& operator_,const double& right) {
			if (operator_.get_token_kind() == TokenKind::ADD)
				return left + right;
			else if (operator_.get_token_kind() == TokenKind::MINUS)
				return left - right;
			else if (operator_.get_token_kind() == TokenKind::MULT)
				return left * right;
			else if (operator_.get_token_kind() == TokenKind::SLASH)
				return left / right;
		}
		bool bool_help(const double& left, const Token operator_, const double& right) {
			if (operator_.get_token_kind() == TokenKind::LESS)
				return (left < right);
			else if (operator_.get_token_kind() == TokenKind::GREATER)
				return (left > right);
			else if (operator_.get_token_kind() == TokenKind::LESS_EQUAL)
				return left <= right;
			else if (operator_.get_token_kind() == TokenKind::GREATER_EQUAL)
				return left >= right;
			else if (operator_.get_token_kind() == TokenKind::EQUAL_EQUAL)
				return left == right;
			else if (operator_.get_token_kind() == TokenKind::NOT_EQUAL)
				return left != right;
			else if (operator_.get_token_kind() == TokenKind::OR)
				return left || right;
			else if (operator_.get_token_kind() == TokenKind::AND)
				return left && right;
		}
		string string_help(const string& left, const Token operator_, const string& right) {
			if (operator_.get_token_kind() == TokenKind::ADD)
				return (left + right);
		}
		bool eval_truthfulness(const any& expr) {
			if (!expr.has_value())
				return false;
			else if (expr.type() == typeid(string) && (any_cast<string>(expr) == "nil"))
				return false;
			else if (expr.type() == typeid(bool))
				return any_cast<bool>(expr);
			return true;
		}

	};

	class Evaluate {
	public:
		any eval(Expr& expr) {
			return expr.accept(visitor);
		}
	private:
		Expr_visitor_impl visitor;
		
	};

};//end namespace expr;

namespace stmt_ {

	class Visitor_stmt;
	class Stmt {
	public:
		virtual any accept(Visitor_stmt& visitor) = 0;
	};

	class Expr_stmt;
	class If_stmt;
	class Block_stmt;
	class Out_stmt;
	class While_stmt;
	class Assign_stmt;
	class Let_decl;
	class Mut_decl;
	class Proc_decl;

	class Visitor_stmt {
	public:
		virtual any visit(Expr_stmt&) = 0;
		virtual void visit(If_stmt&) = 0;
		virtual void visit(Block_stmt&) = 0;
		virtual any visit(Out_stmt&) = 0;
		virtual void visit(Let_decl&) = 0;
		virtual void visit(Mut_decl&) = 0;
		virtual void visit(Assign_stmt&) = 0;
		virtual void visit(While_stmt&) = 0;
		virtual void visit(Proc_decl&) = 0;
		//other visitor stmt implements
	};
	
	class Expr_stmt : public Stmt {
	public:
		Expr_stmt(expr_::Expr& expr_stmt): expr_stmt(expr_stmt) {

		}
		any accept(Visitor_stmt& visitor) override {
			return visitor.visit(*this);
		}
		expr_::Expr& get_expr() {
			return expr_stmt;
		}
	private:
		expr_::Expr& expr_stmt;
	};

	class If_stmt : public Stmt {
	public:
		If_stmt(Stmt& condition, Stmt& body, Stmt* else_cond): condition(condition), 
			body(body), else_cond(else_cond){}

		any accept(Visitor_stmt& visitor) {
			visitor.visit(*this);
			return NULL;
		}
		Stmt& get_condition(){
			return condition;
		}

		Stmt& get_body() {
			return body;
		}
		Stmt* get_else_cond() {
			return else_cond;
		}

	private:
		Stmt& condition;
		Stmt& body;
		Stmt* else_cond;
	};

	class Block_stmt : public Stmt {
	public:
		Block_stmt(ArrayList<Stmt*> inner_stmts): inner_stmts(inner_stmts){}

		ArrayList<Stmt*> get_inner_stmts() {
			return inner_stmts;
		}

		any accept(Visitor_stmt& visitor) override {
			visitor.visit(*this);
			return NULL;
		}

	private:
		ArrayList<Stmt*> inner_stmts;
	};

	class Out_stmt : public Stmt {
	public:
		Out_stmt(Stmt& expr_stmt) : expr_stmt(expr_stmt) {}

		Stmt& get_expr_stmt() {
			return expr_stmt;
		}

		any accept(Visitor_stmt& visitor) override {
			return visitor.visit(*this);
		}

	private:
		Stmt& expr_stmt;
	};

	class Assign_stmt : public Stmt {
	public: 
		Assign_stmt(Token name, Stmt& value) : name(name), value(value){}

		any accept(Visitor_stmt& visitor){
			 visitor.visit(*this);
			 return NULL;
		}
		Token get_name() const {
			return name;
		}
		Stmt& get_value() const {
			return value;
		}
	private:
		Token name;
		Stmt& value;
	};

	class While_stmt : public Stmt {
	public:
		While_stmt(Stmt& condition, Stmt& statement) : condition(condition), statement(statement) {}

		any accept(Visitor_stmt& visitor) {
			visitor.visit(*this);
			return NULL;
		}
		
		Stmt& get_condition() const {
			return condition;
		}
		Stmt& get_statement() const {
			return statement;
		}
	private:
		Stmt& condition;
		Stmt& statement;
	};

	class Let_decl : public Stmt {
	public:
		Let_decl(Token identifier, Stmt* initializer) : identifier(identifier), initializer(initializer) {}

		Token get_identifier() {
			return identifier;
		}
		Stmt* get_initializer() {
			return initializer;
		}

		any accept(Visitor_stmt& visitor) override {
			visitor.visit(*this);
			return NULL;
		}

	private:
		Stmt* initializer;
		Token identifier;
	};

	class Mut_decl : public Stmt {
	public:
		Mut_decl(Token identifier, Stmt* initializer) : identifier(identifier), initializer(initializer) {}

		Token get_identifier() {
			return identifier;
		}
		Stmt* get_initializer() {
			return initializer;
		}

		any accept(Visitor_stmt& visitor) override {
			visitor.visit(*this);
			return NULL;
		}

	private:
		Stmt* initializer;
		Token identifier;
	};

	class Proc_decl : public Stmt {
	public:
		Proc_decl(Token identifier, ArrayList<Stmt*> params, Stmt* block_stmt):
			identifier(identifier), params(params), block_stmt(block_stmt){}

		any accept(Visitor_stmt& visitor) {
			visitor.visit(*this);
			return NULL;
		}

		ArrayList<Stmt*> get_params() {
			return params;
		}
		Stmt* get_block_stmt() {
			return block_stmt;
		}

		Token get_identifier() {
			return identifier;
		}
	private:
		Token identifier;
		ArrayList<Stmt*> params;
		Stmt* block_stmt;
	};

	

	class Visitor_stmt_impl : public Visitor_stmt {
		private:
			/************************************************************************************/
			class Procedure : public Procedure_callable {
			public:
				Procedure(Environment& closure, Proc_decl& procedure) : closure(closure), procedure(procedure) {}
				//for calls
				any call(Environment& prev_env, ArrayList<any> args) override {
					ArrayList<string> params;
					for (int i = 0; i < prev_env.get_identifier_verification().size(); i++) {
						for (int j = 0; j < prev_env.get_identifier_verification().get_entry(i).size(); i++) {
							params.add(prev_env.get_identifier_verification().get_entry(i).get(j).get_entry_key());
						}
					}
					assert(args.size() == params.size());
					Environment current(&prev_env);
					//global = current;
					for (int i = 0; i < args.size(); i++) {
						global.store_entry(params.get(i), args.get(i));
					}
					//Stmt* block = new Block_stmt(procedure.get_block_stmt(), global);
					//eval block
					Visitor_stmt_impl visit_;
					procedure.get_block_stmt()->accept(visit_);
					return 0;

				}
				int arity() const override {
					return procedure.get_params().size();
				}

			private:
				Environment& closure;
				Proc_decl& procedure;
			};
			/*****************************************************************************/
	public:
		
		any visit(Expr_stmt& expr) override {
			expr_::Evaluate expr_eval;
			return expr_eval.eval(expr.get_expr());
		}

		void visit(If_stmt& if_stmt) override {
			bool condition = truthful(if_stmt.get_condition().accept(*this));
			if (condition) {
				if_stmt.get_body().accept(*this);
			}
			else if (if_stmt.get_else_cond() != nullptr ) {
				if_stmt.get_else_cond()->accept(*this);
			}
		}
		void visit(Block_stmt& block_stmt) override {
			//Environment local_block = Environment(&global);
			ArrayList<Stmt*> statements = block_stmt.get_inner_stmts(); 
			for (int i = 0; i < statements.size(); i++) {
				statements.get(i)->accept(*this);
			}
		}

		any visit(Out_stmt& out_stmt) override {
			any result = out_stmt.get_expr_stmt().accept(*this);
			cout << convert_to_string(result) << endl;
			return NULL;
		}

		void visit(Let_decl& let_decl) override {
			Token result = let_decl.get_identifier();
			string identifier_value = any_cast<string>(result.get_value());
			if (global.contains_identifier(identifier_value))
				//handle const runtime error
				return;
				;
			any initializer = let_decl.get_initializer()->accept(*this);
			global.store_entry( identifier_value, initializer);
			global.store_identifier_label(identifier_value, env::Var_proc_type::LET);
		}

		void visit(Mut_decl& mut_decl) override {
			Token result = mut_decl.get_identifier();
			string identifier_value = any_cast<string>(result.get_value());
			any initializer = NULL;
				if(mut_decl.get_initializer()) 
					initializer = mut_decl.get_initializer()->accept(*this);
			global.store_entry(identifier_value, initializer);
			global.store_identifier_label(identifier_value, env::Var_proc_type::MUT);
		}

		void visit(Assign_stmt& assign_stmt) override {
			string var_name = any_cast<string>(assign_stmt.get_name().get_value());
			if (!global.get_identifier_verification().contain_key(var_name)) {
				//handle runtime error : not defined
				return;

			}
			else {
				env::Var_proc_type var_type = global.get_identifier_verification().get_value(var_name);
				if (var_type == env::Var_proc_type::LET) {
					//error cant assign immutable variable object
					return;
				}
				else if (var_type == env::Var_proc_type::MUT){
					any value = assign_stmt.get_value().accept(*this);
					global.store_entry( var_name, value);
				}
			}
		}

		void visit(While_stmt& while_stmt) override {
			while ( truthful(while_stmt.get_condition().accept(*this)) ) {
				while_stmt.get_statement().accept(*this);
			}
		}

		void visit(Proc_decl& proc_decl) override {
			Environment proc_local(&global);
			//global = proc_local;
			for(int i = 0; i < proc_decl.get_params().size(); i++)
				proc_decl.get_params().get(i)->accept(*this);
			Procedure proc(proc_local, proc_decl);
			string proc_name = any_cast<string>(proc_decl.get_identifier().get_value());
			if (global.get_identifier_verification().contain_key(proc_name)) {
				//error cant use variable name let or mut
				return;
			}
			global.store_entry(proc_name, proc);
			global.store_identifier_label(proc_name, env::Var_proc_type::PROC);
		}

	private:
		string convert_to_string(any& result) {
			if (result.type() == typeid(double))
				return std::to_string(any_cast<double>(result));
			else if (result.type() == typeid(string))
				return any_cast<string>(result);
			else if (result.type() == typeid(bool)) {
				if (!any_cast<bool>(result))
					return "false";
				else
					return "true";
			}
			else
				return "nil";
		}
		bool truthful(any& result) {
			if (result.type() == typeid(bool))
				return any_cast<bool>(result);
			else if (result.type() == typeid(int) && (any_cast<int>(result) == NULL))
				return false;
			else if (result.type() == typeid(string) && (any_cast<string>(result) == "nil"))
				return false;
			else
				return true;
		}
	};

	const static class Evaluate{
	public:
		Evaluate(ArrayList<Stmt*> statements) : statements(statements){}

		Visitor_stmt_impl visitor_impl;

		void evaluate_statements() {
			for (int i = 0; i < statements.size(); i++) {
				Stmt* statement = statements.get(i);
				statement->accept(visitor_impl);
			}
		}
	private:
		ArrayList<Stmt*> statements;
	};

}//end namespace stmt;