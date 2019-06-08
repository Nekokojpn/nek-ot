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
char cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
int pc = 0;		//Program counter
int line = 0;	//source line
std::vector<int> tokens;

// TOKENS--------------->
enum {
  tok_fn = 2,

  tok_int = 100,

  tok_num_int = 200,
  tok_num_double = 201,

  tok_eof = -1
};

//Useful funcs----------->

void error(std::string message) { std::cerr << message << std::endl; }
void error_at(std::string message) {}

void get_char() { cc = source[pc++]; std::cout<<"getchar cc="<<cc<<std::endl;}
void undo_char(){pc--;}
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
    #ifdef HIGH_DEBUGG
    std::cout<<"ALPHA!"<<std::endl;
    #endif
    cs = cc;
    get_char();
    while (isalnum(cc)){
        cs+=cc;
        get_char();
    }
    undo_char();
    #ifdef HIGH_DEBUGG
    std::cout<<"cs="<<cs<<std::endl;
    #endif
    if (compare_cs("fn")) {
      return tok_fn;
	} else if (compare_cs("int")) {
      return tok_int;
	} else if (compare_cs("ENDOFFILE")){
      return tok_eof;
  }
  } else if (isdigit(cc)) {//[0-9]+([0-9]|.)*[0-9]+
    #ifdef HIGH_DEBUGG
    std::cout<<"NUMBER!"<<std::endl;
    #endif
    cs = cc;
    get_char();
    bool point = false;
    while(isdigit(cc) ||(cc=='.' && !point)){
      if(cc=='.')point=true;
      cs+=cc;
      get_char();
    }
    undo_char();
    #ifdef HIGH_DEBUGG
    std::cout<<"csNUMBER="<<cs<<std::endl;
    #endif
    if(point){ //double.
      return tok_num_double;
    }
    else{ //int
      return tok_num_int;
    }
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
    source+='\0';
    std::cout<<source<<std::endl;
    return 0;
}

int main() {
    if(load_source()==1)return 1;
    int tok = gettoken();
    #ifdef HIGH_DEBUGG
    std::cout<<"tok="<<tok<<std::endl;
    #endif
    while(tok !=tok_eof){
        tokens.push_back(tok);
        tok = gettoken();
        #ifdef HIGH_DEBUGG
        std::cout<<"tok="<<tok<<std::endl;
        #endif
    }
    //for(auto i : tokens)std::cout<<i<<std::endl;
}