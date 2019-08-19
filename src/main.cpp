#include <fstream>
#include <sstream>
#include "parser.h"


using std::ifstream;
using std::stringstream;

using parser::Parser;
using stmt_::Evaluate;


void run_repl();
void run_file(const char*);
void run(const string&);

void initialize_keywords(HashMap<TokenKind, string>&);

static bool had_compile_error = false;
static bool had_runtime_error = false;

extern void compile_error(TokenKind kind, void* object, string message, int line, int pos){
	cerr << " error at line " << line <<" position "<<pos<<" "<< message << endl;
	had_compile_error = true;
	return;
}

extern void runtime_error(Token token){
	
}

HashMap<TokenKind, string> keywords; 

int main( int argc, char** argv){
	initialize_keywords(keywords);

	if(argc > 2 || argc < 1){
		cerr<<"Usage: output address or repl terminal";
		exit(EXIT_FAILURE);
	}
	else if (argc == 1) {
		run_file("C:/Users/Beloved/Documents/CodeLite/Brown/src/hello.brn");
	}
	else if(argc == 2){
		run_file(argv[1]);
		if (had_compile_error)
			exit(EXIT_FAILURE);
	}else{
		run_repl();
	}

}

void run_file(const char* path){
	ifstream reader = ifstream(path);
	if(!reader.is_open()){
		cerr<<"error reading from file";
		exit(-1);
	}
	stringstream buf;
	buf<<reader.rdbuf();
	run(buf.str());
}

void run_repl(){
	string source;
	do{
			had_compile_error = false;
			cout<<">> ";
			getline(cin, source);
			run(source);
		}while(true);
}

void run(const string& source){
		Tokenizer scanner(source);
		ArrayList<Token>& tokens = scanner.scanToken();
		/*for (int i = 0; i < tokens.size(); i++) {
			cout << tokens.get(i) << endl;
		}*/
		Parser parser(tokens);
		ArrayList<Stmt*> statements = parser.parse();
		if (had_compile_error)
			return;
		Evaluate evaluator(statements);
		evaluator.evaluate_statements();
}

void initialize_keywords(HashMap<TokenKind, string>& keywords)
{
	keywords.put(TokenKind::LET, "let");
	keywords.put(TokenKind::MUT, "mut");
	keywords.put(TokenKind::HELP, "help");
	keywords.put(TokenKind::ARRAY, "array");
	keywords.put(TokenKind::CLASS, "class");
	keywords.put(TokenKind::FOR, "for");
	keywords.put(TokenKind::WHILE, "while");
	keywords.put(TokenKind::IF, "if");
	keywords.put(TokenKind::ELSE, "else");
	keywords.put(TokenKind::THIS, "this");
	keywords.put(TokenKind::OUT, "out");
	keywords.put(TokenKind::SUPER, "super");
	keywords.put(TokenKind::IMPORT, "import");
	keywords.put(TokenKind::PROC, "proc");
	keywords.put(TokenKind::NIL, "nil");
	keywords.put(TokenKind::AND, "and");
	keywords.put(TokenKind::OR, "or");
	keywords.put(TokenKind::TRUE, "true");
	keywords.put(TokenKind::FALSE, "false");
}
