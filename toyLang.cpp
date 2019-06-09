#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include <cstdio>

// IFDEF
//#define HIGH_DEBUGG

//typedef
typedef struct {
  int ty;
  int val;
}Token_t;
typedef struct Node_t{
  int ty;
  Node_t* lhs;
  Node_t* rhs;
  int val;
  double dval;
}Node_t;

// GLOBALS------------->
std::string source;
char cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
int pc = 0;		//Program counter
int line = 0;	//source line
std::vector<int> tokens;
std::vector<std::string> literals;

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

void get_char() { cc = source[pc++];}
void undo_char(){pc--;}

void addToliteral(){literals.push_back(cs);}
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
    get_char();
    while (isalnum(cc)){
        cs+=cc;
        get_char();
    }

    undo_char();
    addToliteral();

    if (compare_cs("fn")) {
      return tok_fn;
	} else if (compare_cs("int")) {
      return tok_int;
	} else if (compare_cs("ENDOFFILE")){
      return tok_eof;
  }
  } else if (isdigit(cc)) {//[0-9]+([0-9]|.)*[0-9]+
    cs = cc;
    get_char();
    bool point = false;
    while(isdigit(cc) ||(cc=='.' && !point)){
      if(cc=='.')point=true;
      cs+=cc;
      get_char();
    }
    undo_char();
    addToliteral();
    if(point){ //double.
      return tok_num_double;
    }
    else{ //int
      return tok_num_int;
    }
  }else if(cc == '\0')return tok_eof;
  std::string s="";
  s+=cc;
  error("Unknown token '" + s + "', Exit.");
  return 1;
}

//LEXER<------------------------

//Parser------------------------>

//GLOBALS
int pos=0;

using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;


//Useful Funcs

Node_t* new_node_intliteral(int _val){
  Node_t* node = new Node_t;
  node->ty = tok_num_int;
  node->val = _val;
  return node;
}
//次のトークンが期待するtyだったら読み進める.
int consume(int ty){
  if(tokens[pos] != ty)
    return 0;
  pos++;
  return 1;
}

Node_t* term(){
  if(consume('(')){
    //うーん. どうやってdoubleとintを混ぜれるのだ.
  }
  if(tokens[pos] == tok_num_int)
    return new_node_intliteral(tokens[pos++]);
  
  error("Unanticipated expression, Exit.");
  return nullptr;
}
void parse(){
  
}

void gen(Node_t* node){
  if(node->ty == tok_int){
    //llvm::IRBuilder
  }
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
    while(tok !=tok_eof){
        tokens.push_back(tok);
        tok = gettoken();
        
    }
    int it = tokens.size();
    for(int i = 0; i<it;i++){
      std::cout<<"input:"<<literals[i]<<std::endl<<"enum=" << tokens[i]<<std::endl;
    }
}