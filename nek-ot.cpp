#include "nek-ot.h"
//Common globals----->

LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
std::unique_ptr<Module> TheModule;

int tytoken_size;
std::string source_filename;
//<-----
extern std::vector<std::string> source;
extern char cc;        
extern std::string cs; 
extern std::vector<TK> tokens;
extern std::vector<std::string> literals;
extern std::vector<Token_t> tytokens;






//Parser globals----->
int pos = 0;
int curtok = 0;

static std::map<std::string, FunctionCallee> Functions_Global;

static std::map<std::string, Value*> NamedValues_Global;
static std::map<std::string, Value*> NamedValues_Local;

static TK RetType; //戻り値の型
std::stack<TK> stack;

bool isinFunc = false;
//<-----

//Common Funcs----->

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

LLVMContext& getContext() {
	return TheContext;
}
IRBuilder<>& getBuilder() {
	return Builder;
}
std::unique_ptr<Module> getModule() {
	return std::move(TheModule);
}

// Parser------------------------>

/*
void gen_as_polish_notation(AST* ast) {
	if (ast->get_nd_type() == NDType::BinOp) {
		ASTBinOp* binop = (ASTBinOp*)ast;
		
		char c;
		if (binop->op == Op::Plus)
			c = '+';
		else if (binop->op == Op::Minus)
			c = '-';
		else if (binop->op == Op::Mul)
			c = '*';
		else
			c = '/';
		std::cout << c << " ";

		gen_as_polish_notation(binop->lhs.get());
		gen_as_polish_notation(binop->rhs.get());
	}
	else if (ast->get_nd_type() == NDType::Number) {
		ASTValue* value = (ASTValue*)ast;
		std::cout << value->value << " ";
	}
}
*/

int main(int argc, char** argv) {
#ifdef DEBUGG
	std::chrono::system_clock::time_point start, end_toknize, end;
	start = std::chrono::system_clock::now();
#endif
#ifdef DEBUGG
  if (load_source("arith_expr.nk") == 1)
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
	std::cout  << (int)tokens[i] << " " << literals[i] << std::endl;
#endif //  HIGHDEBUGG
  }
  Token_t t;
  t.ty = TK::tok_eof;
  tytokens.push_back(t);
  tytoken_size = tytokens.size();
#ifdef DEBUGG
  end_toknize = std::chrono::system_clock::now();
#endif
  // Parser--->

  TheModule = make_unique<Module>("top", TheContext);
Sys::IO::CreateFunc();
  Function* mainFunc =
	  Function::Create(FunctionType::get(Type::getVoidTy(TheContext), false),
		  Function::ExternalLinkage, "main", TheModule.get());
  Builder.SetInsertPoint(BasicBlock::Create(TheContext, "", mainFunc));

  
  auto parser = Parser(tytokens);
  auto ast = parser.parse();
  //gen_as_polish_notation(ast.get());
  std::cout << std::endl;

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