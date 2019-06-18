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
#include <stack>

// IFDEF
#define HIGH_DEBUGG

// typedef
typedef struct {
  int ty;
  std::string val;
} Token_t;

// GLOBALS------------->
std::string source;
char cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
int pc = 0;     // Program counter
int line = 0;   // source line
std::vector<int> tokens;
std::vector<std::string> literals;
std::vector<Token_t> tytokens;
bool isdq_started = false;


// Useful funcs----------->

void error(std::string message) { std::cerr << message << std::endl; exit(1); }
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

	if (isdq_started) {
		while (cc != '\"' && cc != '\0') {
			cs += cc;
			get_char();
		}
		if (cc == '\0') {
			error("tokenization error");
		}
		undo_char();
		addToliteral();
		return tok_str_string;
	}

    undo_char();
    addToliteral();

    if (compare_cs("fn")) {
      return tok_fn;
    } else if (compare_cs("int")) {
      return tok_int;
    } else if (compare_cs("void")) {
      return tok_void;
    } else if (compare_cs("ret")) {
      return tok_ret;
    }
	else if (compare_cs("float")) { return tok_float; }
	else if (compare_cs("double")) { return tok_double; }
	else if (compare_cs("short")) { return tok_short; }
	else if (compare_cs("long")) { return tok_long; }
	else if (compare_cs("char")) { return tok_char; }
	else if (compare_cs("string")) { return tok_string; }

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
	if (cc == '\'')
		return tok_sq;
	if (cc == '\"') {
		if (!isdq_started) isdq_started = true;
		else isdq_started = false;
		return tok_dq;
	}

    if (cc == '-') {
      get_char();
      if (cc == '>') { // arrow
        return tok_arrow;
      }
      undo_char();
      return tok_minus;
    }
    if (cc == '+')
      return tok_plus;
	if (cc == '*')
		return tok_star;
	if (cc == '/')
		return tok_slash;
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

static int RetType = 0; //戻り値の型
std::stack<int> stack;

bool isinFunc = false;


// Useful Funcs

//次のトークンが期待するtyだったら読み進める.
bool consume(int ty) {
  if (tytokens[curtok + 1].ty != ty)
    return false;
  curtok++;
  return true;
}

int getnextty() { return tytokens[curtok + 1].ty; }

//get current tokens.type
int getcurty() { return tytokens[curtok].ty; }

int getnext_num() { return std::atoi(tytokens[curtok + 1].val.c_str()); }
std::string getnext_str() { return tytokens[curtok + 1].val; }

int getcur_num() { return std::atoi(tytokens[curtok].val.c_str()); }
std::string getcur_str() { return tytokens[curtok].val; }

void cout_cur() { std::cout << tytokens[curtok].val; }
void cout_cur_n() { std::cout << getcur_num() << std::endl; }



FunctionType* getTypebyToken(int token) {
	RetType = token;
  if (token == tok_void)
    return FunctionType::get(Type::getVoidTy(TheContext), false);
  if (token == tok_int)
    return FunctionType::get(Type::getInt32Ty(TheContext),false);
}

class Func {
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
    Function *fn =
        Function::Create(getTypebyToken(retty), Function::ExternalLinkage, name,
                         TheModule.get());
    return fn;
  }
};
class String {
	std::string name = "";
	std::string val = "";
public:
	void setName(std::string _name) { name = _name; }
	std::string& getName() { return name; }
	void setVal(std::string _val) { val = _val; }
	std::string& getVal() { return val; }
	Value* codegen() {
		return Builder.CreateGlobalStringPtr(val,name);
	}
};
void funcgen() {
	isinFunc = true;
  std::unique_ptr<Func> func = std::make_unique<Func>();
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
  if (getcurty() < 100 && getcurty() > 199 &&
      getcurty() != tok_identifier)
    error("fn: " + func->getName() + " has no ret value1.");
  curtok++;
  func->setRetTy(getcurty());

  if (!consume(tok_lb))
    error("fn: " + func->getName() + " has no parentheses.");
  
  if(func->getName() == "main")
	Builder.SetInsertPoint(BasicBlock::Create(TheContext, "entry", func->codegen()));
  else 
	  Builder.SetInsertPoint(BasicBlock::Create(TheContext, "", func->codegen()));
}


void exprgen(Value* x) {
  if (getnextty() == tok_plus || getnextty() == tok_minus ||
      getnextty() == tok_star || getnextty() == tok_slash ||
      getnextty() == tok_semi) {
    if (getnextty() == tok_semi && x!=nullptr) {
      Builder.CreateStore(Builder.getInt32(getcur_num()), x);
	  /*
	  Value* hello = Builder.CreateGlobalStringPtr("Hello world");
	  
	  // puts をとりだしてつかえるようにセットアップする。
	  std::vector<Type*> putsArgs;
	  putsArgs.push_back(Builder.getInt8Ty()->getPointerTo());
	  ArrayRef<Type*>  argsRef(putsArgs);

	  // puts の型
	  llvm::FunctionType* putsType =
		  llvm::FunctionType::get(Builder.getInt32Ty(), argsRef, false);
	  // puts 関数をいよいよとりだした!
	  llvm::FunctionCallee putsFunc = TheModule->getOrInsertFunction("puts", putsType);
	  Builder.CreateCall(putsFunc, hello);
	  */
      return;
	}
	else {
		
	}
  }
}
void subst_intgen() {
  if (getnextty() != tok_identifier)
    error("After type must be identifier.");
  curtok++;
  Value *x = Builder.CreateAlloca(Builder.getInt32Ty(), nullptr, getcur_str());
  if (getnextty() != tok_semi && getnextty() != tok_equal) {
    error("NO");
    exit(1);
  }
if (getnextty() == tok_semi) {
    curtok++;
    return;
}
if (getnextty() == tok_equal) {
  curtok++;
  /* エラーハンドリング
  if (getnextty() != tok_num_int || getnextty() != tok_num_double ||
       getnextty() !=tok_identifier) {
    error("NO");
    exit(1); //期待するものではなかった
    } 
	*/
  curtok++;
	if (getcurty() == tok_num_int) {
		exprgen(x);
    }
  }
}
void stringgen() {
	
	std::unique_ptr<String> str  = std::make_unique<String>();
	if (getnextty() != tok_identifier) 
		error("After string type must be identifier.");
	curtok++;
	str->setName(getcur_str());
	if (getnextty() != tok_equal && getnextty() != tok_semi)
		error("Syntax error1");
	curtok++;
	if (getcurty() == tok_semi) { //Ex string s;

	}
	else if (getcurty() == tok_equal) { //Ex string s = "sss";
		if (getnextty() != tok_dq)
			error("Syntax error2");
		curtok++;
		if (getnextty() != tok_str_string)
			error("Syntax error3");
		curtok++;
		str->setVal(getcur_str());
		if (getnextty() != tok_dq)
			error("Syntax error4");
		curtok++;
		if (getnextty() != tok_semi)
			error("Syntax error5");
		std::cout<<"ieee";
		str->codegen();//ここから　戻り値ありなんだけどこれを管理する方法。
	}
}
void retgen() { 
	if (RetType == tok_void && getnextty() == tok_semi) { //ret;  syntax ok
		Builder.CreateRetVoid();
	}
	else { //戻り値が存在する

	}
	isinFunc = false;
} 
    // topofgen グローバルからの
void gen() { // fn <id>(){
	if (getcurty() == tok_fn) {
		funcgen();
	}
	if (getcurty() == tok_num_int) {
		
	}
	if (getcurty() == tok_int) {
		subst_intgen();
	}
	if (getcurty() == tok_string) {
		stringgen();
	}
	if (getcurty() == tok_ret) {
		retgen();
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
	std::chrono::system_clock::time_point start, end_toknize, end;
	start = std::chrono::system_clock::now();
  if (load_source() == 1)
    return 1;
  int tok = gettoken();
  while (tok != tok_eof) {
    tokens.push_back(tok);
    tok = gettoken();
  }
  int it = tokens.size();
#ifdef HIGH_DEBUGG
  std::cout << "-----Token dump-----" << std::endl;
#endif //  HIGHDEBUGG
  for (int i = 0; i < it; i++) {
    Token_t t;
    t.ty = tokens[i];
    t.val = literals[i];
    tytokens.push_back(t);
#ifdef  HIGH_DEBUGG
	std::cout << "input:" << literals[i] << std::endl
              << "enum=" << tokens[i] << std::endl;
#endif //  HIGHDEBUGG
  }
  Token_t t;
  t.ty = -1;
  tytokens.push_back(t);
  end_toknize = std::chrono::system_clock::now();
  // Parser--->
  TheModule = make_unique<Module>("top", TheContext);

  curtok = 0;
  while (getcurty() != -1) {
	gen();
    curtok++;
  }

  end = std::chrono::system_clock::now();
  std::cout << "-----LLVM IR-----" << std::endl;
  TheModule->dump();
  std::cout << "-----time-----" << std::endl;
  double all_time = static_cast<double>(
	  std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
	  1000000.0);
  double toknize_time = static_cast<double>(
	  std::chrono::duration_cast<std::chrono::microseconds>(end_toknize - start)
	  .count() /
	  1000000.0);
  double parse_time = static_cast<double>(
	  std::chrono::duration_cast<std::chrono::microseconds>(end - end_toknize)
	  .count() /
	  1000000.0);
  printf("All time %lf[s]\n", all_time);
  printf("Tokenize time %lf[s]\n", toknize_time);
  printf("Parse time %lf[s]\n", parse_time);
}