#pragma once
#include "../helpers/array_list.h"
#include "../helpers/hash_map.h"
#include "token.h"

using token::Token;
using hash::HashMap;

extern void compile_error(TokenKind kind, void* object,string message, int line, int pos);
extern void runtime_error(Token token);
extern HashMap<TokenKind, string> keywords;

namespace lex{
	
using list::ArrayList;

class Tokenizer{
	public:
		Tokenizer() = delete;
		Tokenizer(const string&);
		~Tokenizer();
		ArrayList<Token> scanToken();
	private:
		string source;
		int current = 0;
		int start_of_token = 0;
		int line = 1;
		void scan();
		inline bool end_of_file();
		ArrayList<Token> processed_tokens;
		char advance();
		void addToken(TokenKind, any, int);
		bool match(char);
		char peek();
		char pos_look_ahead(int pos);
};

};//end namespace lex

