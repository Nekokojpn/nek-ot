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

#include "Token.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// IFDEF
//#define HIGH_DEBUGG

// typedef
typedef struct {
  int ty;
  std::string val;
} Token_t;
typedef struct Node_t {
  int ty;
  Node_t *lhs;
  Node_t *rhs;
  int val;
  double dval;
} Node_t;

// GLOBALS------------->
std::string source;
char cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
int pc = 0;     // Program counter
int line = 0;   // source line
std::vector<int> tokens;
std::vector<std::string> literals;
std::vector<Token_t> tytokens;


// Useful funcs----------->

void error(std::string message) { std::cerr << message << std::endl; }
void error_at(std::string message) {}

void get_char() { cc = source[pc++]; }
void undo_char() { pc--; }

void addToliteral() { literals.push_back(cs); }
// Compare
bool compare_cs(const char *str) { return cs == str; };

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
    while (isalnum(cc)) {
      cs += cc;
      get_char();
    }

    undo_char();
    addToliteral();

    if (compare_cs("fn")) {
      return tok_fn;
    } else if (compare_cs("int")) {
      return tok_int;
    } else if (compare_cs("void")) {
      return tok_void;
    }

    else {
      return tok_identifier;
    }
  } else if (isdigit(cc)) { //[0-9]+([0-9]|.)*[0-9]+
    cs = cc;
    get_char();
    bool point = false;
    while (isdigit(cc) || (cc == '.' && !point)) {
      if (cc == '.')
        point = true;
      cs += cc;
      get_char();
    }
    undo_char();
    addToliteral();
    if (point) { // double.
      return tok_num_double;
    } else { // int
      return tok_num_int;
    }
  } else { //記号. 必ず1文字
    std::string s = "";
    s += cc;
    literals.push_back(s);
    if (cc == '\0')
      return tok_eof;
    if (cc == '=')
      return tok_equal;
    if (cc == ';')
      return tok_semi;
    if (cc == '(')
      return tok_lp;
    if (cc == ')')
      return tok_rp;
    if (cc == '{')
      return tok_lb;
    if (cc == '}')
      return tok_rb;
    if (cc == '-') {
      get_char();
      if (cc == '>') { // arrow
        std::cout << "input:->" << std::endl;
        return tok_arrow;
      }
      undo_char();
      return tok_minus;
    }
    if (cc == '+')
      return tok_plus;
  }

  std::string s = "";
  s += cc;
  error("Unknown token '" + s + "'");
  literals.push_back(s);
  return 1;
}

// LEXER<------------------------

// Parser------------------------>

// GLOBALS
int pos = 0;
int curtok = 0;
using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value *> NamedValues;

// Useful Funcs

Node_t *new_node_intliteral(int _val) {
  Node_t *node = new Node_t;
  node->ty = tok_num_int;
  node->val = _val;
  return node;
}
//次のトークンが期待するtyだったら読み進める.
bool consume(int ty) {
  if (tytokens[curtok + 1].ty != ty)
    return false;
  curtok++;
  return true;
}

Node_t *term() {
  if (consume('(')) {
    //うーん. どうやってdoubleとintを混ぜれるのだ.
  }
  if (tokens[pos] == tok_num_int)
    return new_node_intliteral(tokens[pos++]);

  error("Unanticipated expression, Exit.");
  return nullptr;
}

FunctionType* getTypebyToken(int token) {
  if (token == tok_void)
    return FunctionType::get(Type::getVoidTy(TheContext), false);
  if (token == tok_int)
    return FunctionType::get(Type::getInt32Ty(TheContext),false);
}


class ExprAST { //Ex. 
  int x;

public:
  ExprAST(int _x) { x = _x; }
  

};
class FuncAST {
  std::string name = "";
  int retty = -1;

public:
  void setName(std::string _name) { name = _name; }
  std::string &getName() { return name; }
  void setRetTy(int _retty) { retty = _retty; }
  int getRetTy() { return retty; }
  Function *codegen() {
    if (name == "")
      error("Failed to parsing functy: fn name is missing.");
    if (retty == -1)
      error("Failed to parsing functy: ret value is missing.");
    std::cout << "fn name=" << name << std::endl;
    Function *fn =
        Function::Create(getTypebyToken(retty), Function::ExternalLinkage, name,
                         TheModule.get());
    return fn;
  }
};
void funcgen() {
  FuncAST *func = new FuncAST;
  if (!consume(tok_identifier))
    error("After fn must be an identifier");
  func->setName(tytokens[curtok].val);
  if (!consume(tok_lp))
    error("fn: " + func->getName() + " has no parentheses.");
  //引数解析.スキップ
  if (!consume(tok_rp))
    error("fn: " + func->getName() + " has no parentheses.");
  //戻り値
  if (!consume(tok_arrow))
    error("fn: " + func->getName() + " has no arrow.");
  if (tytokens[curtok].ty < 100 && tytokens[curtok].ty > 199 &&
      tytokens[curtok].ty != tok_identifier)
    error("fn: " + func->getName() + " has no ret value1.");
  curtok++;
  func->setRetTy(tytokens[curtok].ty);

  if (!consume(tok_lb))
    error("fn: " + func->getName() + " has no parentheses.");

  Builder.SetInsertPoint(BasicBlock::Create(TheContext, "", func->codegen()));
}
void exprgen() {

}
    // topofgen グローバルからの
void gen() { // fn <id>(){
  if (tytokens[curtok].ty == tok_fn) {
    funcgen();
  }
  if (tytokens[curtok].ty == tok_int) {
    exprgen();
  }
}

// Load source
int load_source() {
  std::ifstream ifs("./source_test.nk");
  if (ifs.fail()) {
    error("Failed to open source file.");
    return 1;
  }
  std::string buf;
  while (getline(ifs, buf)) {
    source += buf;
  }
  source += '\0';
  std::cout << source << std::endl;
  return 0;
}

int main() {
  if (load_source() == 1)
    return 1;
  int tok = gettoken();
  while (tok != tok_eof) {
    tokens.push_back(tok);
    tok = gettoken();
  }
  int it = tokens.size();
  std::cout << "-----Token dump-----" << std::endl;
  for (int i = 0; i < it; i++) {
    Token_t t;
    t.ty = tokens[i];
    t.val = literals[i];
    tytokens.push_back(t);
    std::cout << "input:" << literals[i] << std::endl
              << "enum=" << tokens[i] << std::endl;
  }
  Token_t t;
  t.ty = -1;
  tytokens.push_back(t);
  // Parser--->
  TheModule = make_unique<Module>("top", TheContext);

  curtok = 0;
  while (tytokens[curtok].ty != -1) {
	gen();
    curtok++;
  }
  std::cout << "-----LLVM IR-----" << std::endl;
  TheModule->dump();
}