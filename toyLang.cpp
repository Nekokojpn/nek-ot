#include <iostream>
#include <string>

// GLOBALS------------->
std::string source;
char cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
int pc = 0;		//Program counter
int line = 0;	//source line

// TOKENS--------------->
enum {
  tok_fn = 2,

  tok_int = 100,

  tok_num = 200
};

//Useful funcs----------->
void get_char() { cc = source[pc++]; }
//Add to cs
void add_char() { cs += cc; }
//Compare
bool compare_cs(const char* str) { return cs == str; };


// LEXER----------------->
int gettoken() {
  get_char();
  // skip any spaces.
  while (isspace(cc)) {
    get_char();
  }

  // if source[pc] is alpha char.
  if (isalpha(cc)) { // Regex, [A-Z]|[a-z]+[digit]*
    cs = cc;

    while (!isspace(cc) || isalnum(cc)) {
      add_char();
	}
    if (compare_cs("fn")) {
      return tok_fn;
	} else if (compare_cs("int")) {
      return tok_int;
	}
  } else if (isdigit(cc)) {

  }
  error("Unknown token '" + std::to_string(cc) + "', Exit.");
}

void error(std::string message) { std::cerr << message << std::endl; }
void error_at(std::string message) {
  
}

int main() {}