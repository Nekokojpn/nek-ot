#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <cstdio>

// IFDEF
#define HIGH_DEBUGG

// GLOBALS------------->
std::string source;
int cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
int pc = 0;		//Program counter
int line = 0;	//source line
std::vector<int> tokens;

// TOKENS--------------->
enum {
  tok_fn = 2,

  tok_int = 100,

  tok_num = 200,

  tok_eof = -1
};

//Useful funcs----------->

void error(std::string message) { std::cerr << message << std::endl; }
void error_at(std::string message) {}

void get_char() { cc = source[pc++]; }
//Add to cs
void add_char() { cs += std::to_string(cc); }
//Compare
bool compare_cs(const char* str) { return cs == str; };


// LEXER----------------->
int gettoken() {
  get_char();
  #ifdef HIGH_DEBUGG
    std::cout<<"cc="<<cc<<std::endl;
  #endif
  // skip any spaces.
  while (isspace(cc)) {
    get_char();
  }

  // if source[pc] is alpha char.
  if (isalpha(cc)) { // Regex, [A-Z]|[a-z]+[digit]*
  std::cout<<"alph!!";
    cs = cc;
    get_char();
    while (isalnum(cc)){
        get_char();
        add_char();
    }
    #ifdef HIGH_DEBUGG
    std::cout<<"cs="<<cc<<std::endl;
    #endif
    if(cs=="fn")std::cout<<"OK";
    if (compare_cs("fn")) {
      return tok_fn;
	} else if (compare_cs("int")) {
      return tok_int;
	}
  } else if (isdigit(cc)) {

  }else if(cc == EOF)return tok_eof;
  error("Unknown token '" + std::to_string(cc) + "', Exit.");
  return 1;
}

//Load source
int load_source(){
    std::ifstream ifs("./source_test.nk");
    if(ifs.fail()){error("Failed to open source file.");return 1;}
    std::string buf;
    while(getline(ifs,buf))
    {
        source+=buf;
    }
    std::cout<<source<<std::endl;
    return 0;
}

int main() {
    if(load_source()==1)return 1;
    int tok = gettoken();
    while(tok !=tok_eof){
        tokens.push_back(tok);
        tok = gettoken();
        int n;
        std::cin>>n;
    }
    for(auto i : tokens)std::cout<<i<<std::endl;
}