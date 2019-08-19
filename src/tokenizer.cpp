#include "tokenizer.h"

namespace lex{
	
	Token* token;

Tokenizer::Tokenizer(const string& source)
{
	this->source = source;
}

Tokenizer::~Tokenizer(){
	
}

ArrayList<Token> Tokenizer::scanToken(){
	while(!end_of_file()){
		start_of_token = current;
		scan();
	}
	token = new Token(TokenKind::END, "eof", nullptr, line, -1);
	processed_tokens.add(static_cast<Token>(*token));
	return processed_tokens;
}
	
void Tokenizer::scan(){
	char charac = advance();
	switch (charac) {
		case '(':
			addToken(TokenKind::LEFT_PAREN, nullptr, 1);
			break;
		case ')':
			addToken(TokenKind::RIGHT_PAREN, nullptr, 1);
			break;
		case '{':
			addToken(TokenKind::LEFT_BRACE, nullptr, 1);
			break;
		case '}':
			addToken(TokenKind::RIGHT_BRACE, nullptr, 1);
			break;
		case '*':
			addToken(TokenKind::MULT, nullptr, 1);
			break;
		case '+':
			addToken(TokenKind::ADD, nullptr, 1);
			break;
		case '-':
			addToken(TokenKind::MINUS, nullptr, 1);
			break;
		case ';':
			addToken(TokenKind::SEMICOLON, nullptr, 1);
			break;
		case ',':
			addToken(TokenKind::COMMA, nullptr, 1);
			break;
		case '.':
			addToken(TokenKind::DOT, nullptr, 1);
			break;
		case '/': {
			if (match('/')) {
				while (!end_of_file() && peek() != '\n') {
					advance();
				}
				if (!end_of_file()) {
					advance();
					++line;
				}
			}else if (match('*')) {
				while (true) {
					if (!end_of_file() && pos_look_ahead(0) == '*' && pos_look_ahead(1) == '/') {
						advance();
						advance();
						break;
					}
					else if(!end_of_file() && peek() != '\n')
						advance();
					else if(end_of_file()){
						compile_error(TokenKind::END, nullptr, " error ending token '/*' ", line, current);
						break;
					}
					else {
						line++;
						advance();
					}
					
				}
			}else
				addToken(TokenKind::SLASH, nullptr, 1);
			break;
		}
		case '\n':
			line++;
		case '\t':
		case ' ':
		case '\r': 
			break;
		case '>':
			if (match('='))
				addToken(TokenKind::GREATER_EQUAL, nullptr, 2);
			else
				addToken(TokenKind::GREATER, nullptr, 1);
			break;
		case '<':
			if (match('='))
				addToken(TokenKind::LESS_EQUAL, nullptr, 2);
			else {
				addToken(TokenKind::LESS, nullptr, 1);
			}
			break;
		case '=':
			if (match('='))
				addToken(TokenKind::EQUAL_EQUAL, nullptr, 2);
			else {
				addToken(TokenKind::EQUAL, nullptr, 1);
			}
			break;
		case '!':
			if (match('='))
				addToken(TokenKind::NOT_EQUAL, nullptr, 2);
			else {
				addToken(TokenKind::NOT, nullptr, 1);
			}
			break;
		case '"':{
			//take into consideration  character "
			size_t string_index = 1;
			if (!end_of_file() && peek() != '"') {
				advance();
				string_index++;
				while (!end_of_file() && peek() != '"') {
					advance();
					string_index++;
				}
			}
			if (end_of_file()) {
				compile_error(TokenKind::STRING, nullptr, " invalid string ", line, current);
				break;
			}
			advance();
			addToken(TokenKind::STRING, nullptr, ++string_index);
			break;
		}
		default: {
			if (isdigit(charac)) {
				size_t num_index = 1;

				while (!end_of_file() && isdigit(peek())) {
					advance();
					++num_index;
				}
				if (!end_of_file() && peek() == '.') {
					advance();
					while (!end_of_file() && isdigit(peek())) {
						advance();
						++num_index;
					}
				}
				if(num_index == 1)
					addToken(TokenKind::NUMBER, nullptr, num_index);
				else {
					addToken(TokenKind::NUMBER, nullptr, ++num_index);
				}
			}
			else if (isalpha(charac)) {
				size_t reserved_iden = 0;
				while (!end_of_file() && isalnum(peek())) {
					advance();
					++reserved_iden;
				}
				const string sub = source.substr(start_of_token, ++reserved_iden);
				if (keywords.contain_value(sub))
					addToken(keywords.get_key(sub), nullptr, reserved_iden);
				else
				addToken(TokenKind::IDENTIFIER, nullptr, reserved_iden);
			}
			else {
				string non_token = " ending token '";
				non_token.push_back(source.at(current-1));
				non_token.append("'");
				compile_error(TokenKind::END, nullptr, non_token, line, current);
			}
			break;
		}
	}
		
	
}

bool Tokenizer::end_of_file(){
	return current >= source.size();
}

char Tokenizer::advance(){
	++current;
	return source.at(current - 1);
}
 void Tokenizer::addToken(TokenKind type, any value, int lexeme_len){
	 string lexeme = source.substr(start_of_token, lexeme_len);
	 //trim  "" away
	 string value_ = source.substr(start_of_token + 1, lexeme_len - 2);
	 
	 if (type == TokenKind::STRING) {
		 value = value_;
	 }
	 else if (type == TokenKind::IDENTIFIER)
		 value = lexeme;
	 else if (type == TokenKind::NUMBER) {
		 value = atof(lexeme.c_str());
	 }
	 else if (keywords.contain_key(type)) {
		 value = keywords.get_value(type);
	 }
		 
	 token = new Token(type, lexeme, value, line, current);
	 processed_tokens.add(static_cast<Token>(*token));
 }

 bool Tokenizer::match(char fut_char)
 {
	 if (!end_of_file() && peek() == fut_char) {
		 advance();
		 return true;
	 }
	 return false;
 }
 char Tokenizer::peek() {
	 return pos_look_ahead(0);
 }
 char Tokenizer::pos_look_ahead(int pos)
 {
	 return source.at(current + pos);
 }
};//end namespace lex