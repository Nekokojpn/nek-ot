#include "nek-ot.h"



//Common globals----->
using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;

int line = 0;
int column = 0;
std::string source_filename;
//<-----

// Tokenizer globals----->
std::vector<std::string> source;
char cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
std::vector<TK> tokens;
std::vector<std::string> literals;
std::vector<Token_t> tytokens;
bool isdq_started = false;
//<-----

//Parser globals----->
int pos = 0;
int curtok = 0;

static std::map<std::string, FunctionCallee> Functions_Global;

static std::map<std::string, Value*> NamedValues_Global;
static std::map<std::string, Value*> NamedValues_Local;

static TK RetType; //戻り値の型
std::stack<int> stack;

bool isinFunc = false;
//<-----

//Common Funcs----->
void error(std::string title,std::string message) {
	std::cerr << std::endl;
	Console::SetConsoleTextRed();
	std::cerr << "error: " << title << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << " --> ";
	Console::SetConsoleTextWhite();
	std::cerr << source_filename << ":" << line+1 << ":" << column << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |" << std::endl << line+1 << " |";
	Console::SetConsoleTextWhite();
	std::string t = source[line].substr(0, source[line].size() - 1);
	int i = 0;
	while (isspace(t[i++]));
	t = t.substr(i - 1, t.size() - 1);
	std::cerr << "     " << t << std::endl; 
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	Console::SetConsoleTextWhite();
	std::cerr << "     ";
	for(int i = 0; i < column-1; i++)
		std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << "^" << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	Console::SetConsoleTextWhite();
	for (int i = 0; i < column - 1; i++)
		std::cerr << " ";
	Console::SetConsoleTextRed();
	std::cerr << message << std::endl;
	Console::SetConsoleTextBlue();
	std::cerr << "  |";
	exit(1);
}
//<-----

//Tokenizer Funcs----->
void get_char() {
	cc = source[line][column++];
	if (cc == '\n') {
		line++;
		column = 0;
		cc = source[line][column];
	}
}
void undo_char() {
	column--;
	if (column < 0) {
		line--;
		column = source[line].size()-1;
	}
}
void addToliteral() { literals.push_back(cs); }
bool compare_cs(const char* str) { return cs == str; };
//<-----

//Parser Funcs----->
bool consume(TK ty) {
	if (tytokens[curtok + 1].ty != ty)
		return false;
	curtok++;
	return true;
}
TK getnextty() { return tytokens[curtok + 1].ty; }
TK getcurty() { return tytokens[curtok].ty; }
int getnext_num() { return std::atoi(tytokens[curtok + 1].val.c_str()); }
std::string getnext_str() { return tytokens[curtok + 1].val; }
int getcur_num() { return std::atoi(tytokens[curtok].val.c_str()); }
std::string getcur_str() { return tytokens[curtok].val; }
void cout_cur() { std::cout << tytokens[curtok].val; }
void cout_cur_n() { std::cout << getcur_num() << std::endl; }
FunctionType* getTypebyToken(TK token) {
	RetType = token;
	if (token == TK::tok_void)
		return FunctionType::get(Type::getVoidTy(TheContext), false);
	if (token == TK::tok_int)
		return FunctionType::get(Type::getInt32Ty(TheContext), false);
}
//<-----

//Internal Library Classes----->
class Sys {
public:
	class IO {
	public:
		static void CreateFunc() {
			//puts --標準出力
			std::vector<Type*> putsArgs;
			putsArgs.push_back(Builder.getInt8Ty()->getPointerTo());
			ArrayRef<Type*>  argsRef(putsArgs);
			FunctionType* putsType =
				FunctionType::get(Builder.getInt32Ty(), argsRef, false);
			FunctionCallee putsFunc = TheModule->getOrInsertFunction("puts", putsType);
			Functions_Global["puts"] = putsFunc;
		}
	};
};
//<-----

// tokenizer----->
TK gettoken() {
	get_char();
	//skip any spaces.
	while (isspace(cc)) {
		get_char();
	}
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
				error("Expected", "Expected --> \"");
			}
			undo_char();
			addToliteral();
			return TK::tok_str_string;
		}
		undo_char();
		addToliteral();
		if (compare_cs("fn")) { return TK::tok_fn; }
		else if (compare_cs("int")) { return TK::tok_int; }
		else if (compare_cs("void")) { return TK::tok_void; }
		else if (compare_cs("ret")) { return TK::tok_ret; }
		else if (compare_cs("float")) { return TK::tok_float; }
		else if (compare_cs("double")) { return TK::tok_double; }
		else if (compare_cs("short")) { return TK::tok_short; }
		else if (compare_cs("long")) { return TK::tok_long; }
		else if (compare_cs("char")) { return TK::tok_char; }
		else if (compare_cs("string")) { return TK::tok_string; }
		else { return TK::tok_identifier; }
	}
	else if (isdigit(cc)) { //[0-9]+([0-9]|.)*[0-9]+
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
			return TK::tok_num_double;
		}
		else { // int
			return TK::tok_num_int;
		}
	}
	else { //記号. 必ず1文字
		std::string s = "";
		s += cc;
		literals.push_back(s);
		if (cc == '\0') return TK::tok_eof;
		if (cc == '=')	return TK::tok_equal;
		if (cc == ';')	return TK::tok_semi;
		if (cc == '(')	return TK::tok_lp;
		if (cc == ')')	return TK::tok_rp;
		if (cc == '{')	return TK::tok_lb;
		if (cc == '}')	return TK::tok_rb;
		if (cc == '\'')	return TK::tok_sq;
		if (cc == '\"') {
			if (!isdq_started) isdq_started = true;
			else isdq_started = false;
			return TK::tok_dq;
		}
		if (cc == '-') {
			get_char();
			if (cc == '>') { // arrow
				return TK::tok_arrow;
			}
			undo_char();
			return TK::tok_minus;
		}
		if (cc == '+')	return TK::tok_plus;
		if (cc == '*')	return TK::tok_star;
		if (cc == '/') {
			get_char();
			if (cc == '/') {
				get_char();
				while (cc != '\n' && cc != '\0') {
					get_char();
				}
				return TK::tok_nope;
			}
			else {
				undo_char();
				return TK::tok_slash;
			}
		}
	}
	std::string s = "";
	s += cc;
	error("Unknown type", "Unknown token '" + s + "'");
	literals.push_back(s);
	return TK::tok_unknown;
}
//<-----

// Parser------------------------>

class Func {
  std::string name = "";
  TK retty = TK::tok_unknown;

public:
  void setName(std::string _name) { name = _name; }
  std::string &getName() { return name; }
  void setRetTy(TK _retty) { retty = _retty; }
  TK getRetTy() { return retty; }
  Function *codegen() {
    if (name == "")
      error("parse error","Failed to parsing functy: fn name is missing.");
    if (retty == TK::tok_unknown)
      error("parse error", "Failed to parsing functy: ret value is missing.");
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
class FuncCall {
	std::string func_name = "";
	std::vector<Value*> args;
public:
	void setFuncName(std::string _funcname) { func_name = _funcname; }
	std::string& getFuncName() { return func_name; }
	void addArgs(Value* _val) { args.push_back(_val); }
	void codegen() {
		//引数一個しかだめ、要修正
		Builder.CreateCall(Functions_Global[func_name], args[0]);
	}
};
void funcgen() {
	isinFunc = true;
  std::unique_ptr<Func> func = std::make_unique<Func>();
  if (!consume(TK::tok_identifier))
    error("parse error", "After fn must be an identifier");
  func->setName(tytokens[curtok].val);
  if (!consume(TK::tok_lp))
    error("parse error", "fn: " + func->getName() + " has no parentheses.");
  //引数解析.スキップ
  if (!consume(TK::tok_rp))
    error("parse error", "fn: " + func->getName() + " has no parentheses.");
  //戻り値
  if (!consume(TK::tok_arrow))
    error("parse error", "fn: " + func->getName() + " has no arrow.");
  if ((int)getcurty() < 100 && (int)getcurty() > 199 &&
      getcurty() != TK::tok_identifier)
    error("parse error", "fn: " + func->getName() + " has no ret value1.");
  curtok++;
  func->setRetTy(getcurty());

  if (!consume(TK::tok_lb))
    error("parse error", "fn: " + func->getName() + " has no parentheses.");
  
  if(func->getName() == "main")
	Builder.SetInsertPoint(BasicBlock::Create(TheContext, "entry", func->codegen()));
  else 
	  Builder.SetInsertPoint(BasicBlock::Create(TheContext, "", func->codegen()));
}


void exprgen(Value* x) {
  if (getnextty() == TK::tok_plus || getnextty() == TK::tok_minus ||
      getnextty() == TK::tok_star || getnextty() == TK::tok_slash ||
      getnextty() == TK::tok_semi) {
    if (getnextty() == TK::tok_semi && x!=nullptr) {
      Builder.CreateStore(Builder.getInt32(getcur_num()), x);
      return;
	}
	else {
		
	}
  }
}
void subst_intgen() {
  if (getnextty() != TK::tok_identifier)
    error("parse error", "After type must be identifier.");
  curtok++;
  Value *x = Builder.CreateAlloca(Builder.getInt32Ty(), nullptr, getcur_str());
  if (getnextty() != TK::tok_semi && getnextty() != TK::tok_equal) {
    error("parse error", "NO");
    exit(1);
  }
if (getnextty() == TK::tok_semi) {
    curtok++;
    return;
}
if (getnextty() == TK::tok_equal) {
  curtok++;
  /* エラーハンドリング
  if (getnextty() != TK::tok_num_int || getnextty() != TK::tok_num_double ||
       getnextty() !=TK::tok_identifier) {
    error("NO");
    exit(1); //期待するものではなかった
    } 
	*/
  curtok++;
	if (getcurty() == TK::tok_num_int) {
		exprgen(x);
    }
  }
}
void stringgen() {
	
	std::unique_ptr<String> str  = std::make_unique<String>();
	if (getnextty() != TK::tok_identifier) 
		error("parse error", "After string type must be identifier.");
	curtok++;
	str->setName(getcur_str());
	if (getnextty() != TK::tok_equal && getnextty() != TK::tok_semi)
		error("parse error", "Expected --> ; ");
	curtok++;
	if (getcurty() == TK::tok_semi) { //Ex string s;

	}
	else if (getcurty() == TK::tok_equal) { //Ex string s = "sss";
		if (getnextty() != TK::tok_dq)
			error("parse error", "Syntax error2");
		curtok++;
		if (getnextty() != TK::tok_str_string)
			error("parse error", "Syntax error3");
		curtok++;
		str->setVal(getcur_str());
		if (getnextty() != TK::tok_dq)
			error("parse error", "Syntax error4");
		curtok++;
		if (getnextty() != TK::tok_semi)
			error("parse error", "Syntax error5");
		NamedValues_Global[str->getName()] = str->codegen();
	}
}
void retgen() { 
	if (RetType == TK::tok_void && getnextty() == TK::tok_semi) { //ret;  syntax ok
		Builder.CreateRetVoid();
	}
	else { //戻り値が存在する

	}
	isinFunc = false;
} 
void funccallgen() {
	std::unique_ptr<FuncCall> fc = make_unique<FuncCall>();
	fc->setFuncName(getcur_str());

	if (getnextty() != TK::tok_lp)
		error("parse error", "");
	curtok++;
	if (getnextty() != TK::tok_identifier)
		error("parse error", "");
	curtok++;
	fc->addArgs(NamedValues_Global[getcur_str()]);
	if (getnextty() != TK::tok_rp)
		error("parse error", "");
	fc->codegen();
}
    // topofgen グローバルからの
void gen() { // fn <id>(){
	if (getcurty() == TK::tok_fn) {
		funcgen();
	}
	if (getcurty() == TK::tok_num_int) {
		
	}
	if (getcurty() == TK::tok_int) {
		subst_intgen();
	}
	if (getcurty() == TK::tok_string) {
		stringgen();
	}
	if (getcurty() == TK::tok_ret) {
		retgen();
	}
	if (getcurty() == TK::tok_identifier) {
		//要修正
		funccallgen();
	}
}

/*												load_source												
		引数：なし
		戻り値：int
		概要：std::ifstreamを使用してソースファイル"source_test.nk"を読み
		込みます。それをグローバル変数std::vector<std:string> sourceにセットします。
*/
int load_source(std::string source_path) {
  std::ifstream ifs(source_path);
  if (ifs.fail()) {
    error("no such directory or file name", "Failed to open source file.");
    return 1;
  }
  source_filename = source_path;
  std::string buf;
  while (getline(ifs, buf)) {
	  std::string t = buf+'\n';
	  source.push_back(t);
  }
  buf = "";
  buf += '\0';
  source.push_back(buf);
#ifdef HIGH_DEBUGG
  std::cout << "-----Source-----" << std::endl;
  for(std::string t : source)
	std::cout  << t;
#endif
  return 0;
}

int main(int argc, char** argv) {
#ifdef DEBUGG
	std::chrono::system_clock::time_point start, end_toknize, end;
	start = std::chrono::system_clock::now();
#endif
#ifdef DEBUGG
  if (load_source("source_test.nk") == 1)
	  return 1;
#else
  if (load_source(static_cast<std::string>(argv[1])) == 1)
    return 1;
#endif

  TK tok = gettoken();
  while (tok != TK::tok_eof) {
	if(tok!=TK::tok_nope)
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
	std::cout  << tokens[i] << " " << literals[i] << std::endl;
#endif //  HIGHDEBUGG
  }
  Token_t t;
  t.ty = TK::tok_eof;
  tytokens.push_back(t);
#ifdef DEBUGG
  end_toknize = std::chrono::system_clock::now();
#endif
  // Parser--->
  TheModule = make_unique<Module>("top", TheContext);

  curtok = 0;

  Sys::IO::CreateFunc();
  while (getcurty() != TK::tok_eof) {
	gen();
    curtok++;
  }
#ifdef DEBUGG
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
#endif
}