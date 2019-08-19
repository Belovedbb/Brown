#pragma once 
#include "../helpers/headers.h"

enum class TokenKind {
	//one chars
	MULT, ADD, SLASH, MINUS,
	COMMA, DOT, LEFT_PAREN,
	RIGHT_PAREN, LEFT_BRACE,
	RIGHT_BRACE, SEMICOLON,
	//one or two chars
	GREATER, GREATER_EQUAL, LESS,
	LESS_EQUAL, EQUAL, EQUAL_EQUAL,
	NOT, NOT_EQUAL,
	//keyword
	LET, MUT, HELP, ARRAY,
	CLASS, FOR, WHILE, IF, ELSE,
	THIS, OUT, SUPER, IMPORT,
	PROC, NIL, AND, OR, TRUE, FALSE,
	//longer
	IDENTIFIER, NUMBER, STRING,
	END
};

extern hash::HashMap<TokenKind, string> keywords;

namespace token{
	

class Token{

	public:
	Token() = default;
	Token(TokenKind type, string lexeme, any value, int line, int position){
		this->type = type;
		this->lexeme = lexeme;
		if (value.has_value())
			value.swap(this->value);
		this->line = line;
		this->position = position;
	}

	const TokenKind get_token_kind() const{
		return type;
	}
	const any get_value() const{
		return value;
	}

	const int get_line() const{
		return line;
	}
	const int get_position() const{
		return position;
	}
	const string get_lexeme() const{
		return lexeme;
	}

private:
	TokenKind type;
	string lexeme;
	any value;
	int line, position;

	friend ostream& operator<<(ostream& out, const Token& token) {
		out << " lexeme " << token.lexeme << " with value ";
		if (token.type == TokenKind::STRING)
			out << any_cast<string>(token.value);
		else if (token.type == TokenKind::NUMBER)
			out << any_cast<double>(token.value);
		else if (keywords.contain_key(token.type))
			out << any_cast<string>(token.value);
		else
			out << " null";
		out << " at line " << token.line << " position " << token.position;
		return out;
	}
};

}//end namespace token
