#pragma once

//#define HIGH_DEBUGG			//ハイ・レベルデバッグ
#define DEBUGG						//デバッグモード.

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
#include "llvm/Support/Casting.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm\Transforms\Utils.h"
#include "llvm\IR\DIBuilder.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitstream/BitstreamWriter.h"

#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Linker/Linker.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <windows.h>				//Using at Console class
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <tuple>

using namespace llvm;

// TOKENS--------------->
enum class TK {
	tok_nope,
	tok_fn,
	tok_if,
	tok_elif,
	tok_else,
	tok_for,
	tok_while,
	tok_new,
	tok_class,
	tok_import,
	tok_in,
	tok_out,
	tok_body,
	tok_xor,
	tok_brk,
	tok_goto,
	tok_true,
	tok_false,
	tok_bool,
	tok_unsafe,
	tok_where,
	tok_op,

	tok_ret,
	tok_void,
	tok_i32,
	tok_i64,
	tok_i128,
	tok_f32,
	tok_f64,
	tok_i16,
	tok_char,
	tok_string,
	tok_action,
	tok_var,
	tok_stct, //struct
	tok_list,
	tok_macro,
	tok_line,
	tok_space,

	//literal
	tok_num_int,
	tok_num_double,
	tok_num_long,
	tok_num_long_long,
	tok_str_string,
	tok_nullptr,

	tok_semi, // ;
	tok_equal, // =
	tok_lp, // (
	tok_rp, // )
	tok_lb, // {
	tok_rb, // }
	tok_arrow, // ->
	tok_rarrow, // <-
	tok_sq, // '
	tok_dq, // "
	tok_lt, // <
	tok_lteq, // <=
	tok_rt, // >
	tok_rteq, // >=
	tok_eqeq, // ==
	tok_em, // !
	tok_emeq, //!=
	tok_comma, // ,
	tok_dot, //.
	tok_lpb, // [
	tok_rpb, // ]
	tok_percent, // %
	tok_darrow, // =>
	tok_colon, // :
	tok_cleq, //:=
	tok_dtdt, //..
	tok_doll, //$
	tok_pipe, //|
	tok_amp, // &
	tok_ltlt, // <<
	tok_rtrt, // >> 
	tok_anno, // @


	// operator
	tok_plus,
	tok_minus,
	tok_star,
	tok_slash,
	tok_plpl, //++
	tok_mimi, //--
	tok_ampamp, // &&
	tok_pipepipe, // ||
	tok_pleq, //+=
	tok_mieq, //-=

	tok_identifier,

	tok_eof,
	tok_unknown

};

#define I32_MAX 0x7FFFFFFF
#define I32_MIN -0x80000000
#define I64_MAX 0x7FFFFFFFFFFFFFFF
#define I64_MIN -0x8000000000000000



class Test {
public:
	static void CreateFunc();
};

class Sys {
public:
	class IO {
	public:
		class Printf {
		public:
			static void CreateFunc();
		};
		class Input {
			public: 
				static void CreateFunc();
		};
		class File {
			public:
				static void CreateFunc();
		};
	};
	class List {
	public:
		static void CreateFunc();
	};
	class Range {
	public:
		static void CreateFunc();
	};
	class Random {
	public:
		static void CreateFunc();
	};
	class Exit {
	public:
		static void CreateFunc();
	};
	class Sleep {
	public:
		static void CreateFunc();
	};
};

enum class AType;
enum class TypeKind;

// typedef----->
typedef struct {
	uint32_t location_begin_line;
	uint32_t location_begin_column;
	uint32_t location_end_line;
	uint32_t location_end_column;
} Location_t;
typedef struct {
	TK ty;
	std::string val;
	Location_t loc;
} Token_t;
typedef struct {
	AType ty;
	bool isArr;
	bool isList;
	std::vector<unsigned long long> arrsize;
	TypeKind kind;
	std::string name;
}Type_t;

typedef struct {
	unsigned long long idx;
	std::string elem_name;
	Type_t elem_ty;
} StctElm_t;

typedef struct {
	std::string stct_name;
	std::map<std::string, StctElm_t> elems;
	std::vector<std::string> elem_names;
} Stct_t;

//<-----

class Parser;

void load_source();

void error(std::string title, std::string message, Token_t& curtok);
void error(std::string title, std::string message, Location_t& loc);
void error(std::string title, std::string message, uint32_t line, uint32_t column);
void error(std::string message, Token_t& curtok);
void error_onlymsg(std::string message);
void error_expected(std::string lit, Token_t& curtok);
void error_unexpected(Token_t& curtok);
void warning(std::string title, std::string message, uint32_t line, uint32_t column);
void warning(std::string title, std::string message, Location_t& loc);
void warning(std::string title, std::string message, Token_t& curtok);
void error_codegen(std::string message, Location_t loc);
void warning_unexpected(Token_t& curtok);
void warning_expected(std::string lit, Token_t& curtok);

void add_err_msg(std::string _errmsg);
void add_warning_msg(std::string _warningmsg);

void writeToFile();
Parser* compile(std::string filename, bool isDumpllvm, bool isDumpollvm);
void dump();

void init_parse();
AllocaInst* createEntryBlockAlloca(Function* function, const std::string& name);

Module* getModule();

void checkBeforeWriteToBC();

class Console {
public:
	static void SetConsoleTextGray();
	static void SetConsoleTextRed();
	static void SetConsoleTextGreen();
	static void SetConsoleTextWhite();
	static void SetConsoleTextBlue();
};




enum class Op{
	Plus,
	Minus,
	Mul,
	Div,
	RDiv,
	LThan,
	Xor,
	Or,
	And,
	Nand,
	Nor,
	Not,
	LShift,
	RShift,
	LThanEqual,
	RThan,
	RThanEqual,
	EqualEqual,
	NotEqual,
	Ampamp,
	Pipepipe
};
enum class AType { //AllType
	Nop,
	I16,
	I32,
	I64,
	I128,
	F32,
	F64,
	Char,
	String,
	UserdefinedType,
	Struct,
	Void,
	Array,
	List,
	Ident,
	Bool
};
enum class TypeKind {
	Value,
	Pointer,
	Reference
};
enum class TypeAST {
	Action,
	ArrayIndex,
	ArrElements,
	BinOp,
	Bool,
	Brk,
	Call,
	Else,
	For,
	Func,
	Goto,
	Identifier,
	IdentifierArrayElementBase,
	IdentifierBase,
	If,
	Label,
	Proto,
	Ret,
	StctElements,
	String,
	StrLiteral,
	Struct,
	Subst,
	Type,
	Value,
	While,
	ListElements,
	Unsafe,
	VarOp
};
class ASTVarOp;

extern std::unique_ptr<Module> module;
extern LLVMContext context;
extern IRBuilder<> builder;

extern bool isDumpllvm;
extern bool isDumpTime;
extern bool isDumpollvm;
extern bool isOpt;
extern bool isDebug;
extern std::map<std::string, std::vector<std::string>> sources;
extern std::stack<std::string> cur_filename;

extern std::map<std::string, bool> imports;

extern std::map<std::string, FunctionCallee> functions_global;
extern std::map<std::string, StructType*> userdefined_stcts;
extern std::map<std::string, Stct_t> userdefined_stcts_elements;
extern std::map<std::string, AllocaInst*> namedvalues_global;
extern std::map<std::string, AllocaInst*> namedvalues_local;
extern std::map<std::pair<std::string, Op>, ASTVarOp*> varops;
extern std::map<std::string, bool> namedvalues_local_isinitialized;
extern std::map<std::string, Value*> namedvalues_str;
extern std::map<Type*, StructType*> ary_struct;
extern Value* underscore;
extern std::map<std::string, BasicBlock*> jmp_labels;
extern std::map<std::string, std::vector<BasicBlock*>> jmp_bbs;
extern std::map<Type*, StructType*> list_struct;

extern bool isSubst;

extern std::pair<bool, std::vector<Value*>> if_rets;
extern std::vector<BasicBlock*> if_rets_bb;

//AST Identifier
extern Value* current_inst;
extern bool isPtr;



extern AllocaInst* retvalue;
extern std::vector<BasicBlock*> retbbs;

extern std::vector<BasicBlock*> brk_bbs;

extern bool retcodegen;
extern bool gotocodegen;
extern bool isStringCodegen;
extern bool isUnsafe;

class ASTSubst;
class ASTFunc;
class ASTStctElements;
class ASTArrElements;

class AST {
public:
	Location_t loc;
	virtual Value* codegen() = 0;
	virtual Type* getType() = 0;
	virtual TypeAST getASTType() = 0;
};
class ASTIdentifierBase : public AST {
public:
	std::string name;
	ASTIdentifierBase(const std::string& _name) : name(_name) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
/*
class ASTIdentifierArrayElementBase : public AST {
public:
	std::string name;
	AST* indexes;
	ASTIdentifierArrayElementBase(const std::string& _name, AST* _indexes) : name(_name), indexes(_indexes) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
*/
class ASTIdentifier : public AST {
public:
	AST* lhs; //lhs
	AST* rhs; //For stct, rhs;
	TypeKind kind;
	ASTIdentifier(AST* _lhs, AST* _rhs, TypeKind _kind) : lhs(_lhs), rhs(_rhs), kind(_kind) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTValue : public AST {
public:
	long long value;
	double value_d;
	bool isDouble;
	bool isLongLong;
	bool isNullptr;
	ASTValue(long long  _value) : value(_value),isDouble(false), isNullptr(false) {};
	ASTValue(double  _value, bool _isDouble) : value_d(_value), isDouble(true), isNullptr(false) {};
	ASTValue(long long  _value, bool _isLongLong) : value(_value), isLongLong(true), isNullptr(false) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTStrLiteral : public AST {
public:
	std::string value;
	ASTStrLiteral(std::string _value) : value(_value) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTBinOp : public AST {
public:
	AST* lhs;
	AST* rhs;
	Op op;
	ASTBinOp(AST* _lhs, Op _op, AST* _rhs) : lhs(_lhs), op(_op), rhs(_rhs) {} ;
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};


class ASTType : public AST {
public:
	Type_t ty;
	std::string name;
	AST* id;
	/* FOR ARRAY ATTRIBUTES */
	//<ASTArrElements> elements;
	//
	/* FOR STRUCT ATTRIBUTES */
	std::string stct_name;
	//
	bool isGlobal;
	AST* expr;
	ASTType(Type_t _ty, AST* _id, AST* _expr, std::string _stct_name, bool _isGlobal) : ty(_ty), id(_id), expr(_expr), stct_name(_stct_name), isGlobal(_isGlobal) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};


class ASTAction : public AST {
public:
	std::string name;
	ASTFunc* lambda;	
	ASTAction(std::string _name, ASTFunc* _lambda) : name(_name), lambda(_lambda) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTStruct : public AST {
public:
	std::string name;
	ASTStctElements* elements;
	ASTStruct(std::string _name, ASTStctElements* _elements) : name(_name), elements(_elements) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTStctElements : public AST {
public:
	Stct_t elements;
	ASTStctElements(Stct_t _elements) : elements(_elements) {};
	Value* codegen() override;
	ArrayRef<Type*> make_aref();
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTArrElements : public AST {
public:
	std::vector<AST*> elements;
	//for comprehension
	AST* restraint;
	ASTType* arr_type;
	ASTArrElements(AST* _restraint, ASTType* _arr_type) : restraint(_restraint), arr_type(_arr_type) {};
	ASTArrElements(std::vector<AST*> _elements) : elements(_elements) {};
	Value* subst(Value* arr);
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};


class ASTString : public AST {
public:
	std::string name;
	ASTStrLiteral* expr_str;
	ASTString(std::string _name, ASTStrLiteral* _expr_str) : name(_name), expr_str(_expr_str) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTRet : public AST {
public:
	Type_t ret_type;
	AST* expr_p;
	ASTRet(Type_t _ret_type, AST* _expr_p) : ret_type(_ret_type), expr_p(_expr_p) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};

class ASTBrk : public AST {
public:
	ASTBrk() {};
	Value* codegen();
	Type* getType() override;
	TypeAST getASTType() override;
};

class ASTProto : public AST {
public:
	std::string name;
	std::vector<Type_t> args;
	std::vector<std::string> identifier;
	Type_t ret;
	ASTProto(std::string _name, std::vector<Type_t> _args, std::vector<std::string> _identifier, Type_t _ret) : name(_name), args(_args), identifier(_identifier), ret(_ret) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTFunc : public AST {
public:
	ASTProto* proto;
	std::vector<AST*> body;
	ASTFunc(ASTProto* _proto, std::vector<AST*> _body) : proto(_proto), body(_body) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTCall : public AST {
public:
	std::string name;
	std::vector<AST*> args_expr;
	ASTCall(std::string _name, std::vector<AST*> _args_expr) : name(_name), args_expr(_args_expr) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTElse : public AST {
public:
	std::vector<AST*> body;
	ASTElse(std::vector<AST*> _body) : body(_body) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTIf : public AST {
public:
	AST* proto; //BoolOp
	std::vector<AST*> body;
	std::vector<ASTIf*> ast_elseif;
	ASTElse* ast_else;
	ASTIf(AST* _proto, std::vector<AST*> _body) : proto(_proto), body(_body), ast_else(nullptr) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;

	BasicBlock* bodyBB;
	BasicBlock* contBB;
};
class ASTFor : public AST {
public:
	AST* typedeff;
	ASTIf* proto;
	AST* last;
	ASTFor(AST* _typedeff, ASTIf* _proto, AST* _last) : typedeff(_typedeff), proto(_proto), last(_last) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTWhile : public AST {
public:
	AST* proto; //BoolOp
	std::vector<AST*> body;
	ASTWhile(AST* _proto, std::vector<AST*> _body) : proto(_proto), body(_body) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTUnsafe : public AST{
public:
	std::vector<AST*> body;
	ASTUnsafe(std::vector<AST*> _body) : body(_body) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTTop : public AST {
public:
	std::vector<AST*> globals;
	ASTTop(std::vector<AST*> _globals) : globals(_globals) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTGoto : public AST {
public:
	std::string label;
	ASTGoto(std::string _label) : label(_label) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTLabel : public AST {
public:
	std::string label;
	ASTLabel(std::string _label) : label(_label) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};

class ASTSubst : public AST {
public:
	AST* id;
	AST* expr;
	std::vector<AST*> body;
	ASTSubst(AST* _id, AST* _expr) :id(_id), expr(_expr) {};
	ASTSubst(AST* _id, std::vector<AST*> _body) :id(_id), body(_body) {};
	ASTSubst(AST* _id) : id(_id) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};

class ASTBool : public AST {
public:
	bool value;
	ASTBool(bool _value) : value(_value) {};
	Value* codegen();
	Type* getType() override;
	TypeAST getASTType() override;
};

// [1][2] etc...
class ASTArrayIndex : public AST {
public:
	AST* expr;
	ASTArrayIndex(AST* _expr) : expr(_expr) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTListElements : public AST {
public:
	std::vector<AST*> elems;
	ASTListElements(std::vector<AST*> _elems) : elems(_elems) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};
class ASTVarOp : public AST {
public:
	std::string name;
	std::vector<Op> ops;
	std::vector<AST*> body;
	ASTVarOp(std::vector<Op> _ops, std::vector<AST*> _body) :ops(_ops), body(_body) {};
	Value* codegen() override;
	Type* getType() override;
	TypeAST getASTType() override;
};

class Tokenizer {
public:
	char cc;
	std::string cs;
	bool isdq_started = false;
	bool ismacro = false;
	std::vector<TK> tokens;
	std::vector<Token_t> tytokens;
	std::vector<std::string> literals;
	std::vector<Location_t> locs;
	uint32_t line = 0;
	uint32_t column = 0;
	void tokenize();
	void get_char();
	void undo_char();
	void skip_line();
	void addToliteral();
	bool compare_cs(const char* str);
	void addToloc(int len);
	TK gettoken();
};

class Codegen;
class Parser {
	int index;
	Token_t curtok;
	std::string curval; //for type;
	std::unique_ptr<Codegen> cdgen;
	std::vector<Token_t> tokens;

	bool isGlobal = false;

	bool curtokIs(TK);
	bool nexttokIs(TK);
	std::vector<AST*> afs;
	std::vector<std::string> userdefinedtype_v;

	AST* expr();
	AST* expr_add();
	AST* expr_mul();
	AST* expr_plmi();
	AST* expr_primary();
	AST* bool_expr();
	AST* Parser::bool_expr_op();
	ASTArrElements* expr_arr();
	ASTStctElements* expr_stct(); 
	ASTStrLiteral* expr_str();
	ASTGoto* expr_goto();
	AST* expr_anno();
	AST* expr_star();
	std::vector<AST*> expr_block(bool isOneExpr);
	AST* expr_identifier();
	ASTSubst* subst_expr(AST*);
	AST* expr_var();
	AST* expr_array_indexes(); //return an ASTArrayIndex;
	AST* expr_lb();
	AST* expr_lpb();

	ASTType* def_type(AST*);
	ASTString* def_string();
	ASTFunc* def_func();
	ASTAction* def_action();
	ASTRet* def_ret();
	ASTBrk* def_brk();
	ASTLabel* def_label(std::string&);

	ASTCall* func_call(std::string, bool);
	ASTIf* bool_statement();
	ASTFor* for_statement();
	ASTWhile* while_statement();

	AST* expr_identifiers(); //Gen ASTIdentifier
	void def_import();
	void def_macro();
	void replaceByMacroRules();
	bool consume(TK tk) noexcept;
	void Parser::getNextToken() noexcept;
public:
	Parser(std::vector<Token_t>);
	std::vector<AST*> parse();
	void codegen(std::vector<AST*>);
	AType getATypeByCurtok();
	Type_t getTypeFromCurtok();
	bool isExpectedSemi = true;
	void addUserdefinedType(std::string name);
	bool isUserdefinedType(std::string name);
};

class Codegen {
public:
	//-----> LLVM functions
	static void call_writef(llvm::ArrayRef<llvm::Value*> args);
	static void call_exit(int exitcode);
	static Type* getTypebyAType(AType& ty);
	static Type* getTypebyType(Type_t& t);
	static void gen_asm(std::string statement, std::string option);
	static void init_on_inst();
	static ConstantInt* getValueInt(Value* c);
	//<-----
	static void declareFunction(std::string func_name, std::string ac_func_name);
	static Value* getIdentifier(Value* v, AST* ast, Location_t& t);
	static AllocaInst* getLocalVal(std::string name, Location_t& t);
	static Value* getGlobalVal(std::string name, Location_t& t);
	static Value* getDefinedValue(std::string name, Location_t& t);
	static std::vector<Type*> getStctElements(std::string origin_stctname, AST* ast, Location_t& t);
	static Value* substList(std::string name, Type* stct, AST* ast, Location_t& t);
	static void createWritefln(std::string message);
	static void createErrWritefln(std::string message, Location_t& t);
	static BasicBlock* createBB();
	static void call_exit(Value* exit_code);
	static void createRuntimeError(std::string errmsg, Value* cond, Location_t& t);
	static std::tuple<Value*, Value*> doMatchType(Value* l, Value* r);
	static std::vector<Value*> genArgValues(ASTCall* ac);
	static Value* getListfromIndex(Type* stct_ty, Value* ptr_stct, std::vector<Value*> idx_list, Location_t& t);
	static Value* getListfromIndex(Type* stct_ty, Value* ptr_stct, Location_t& t);
	static Value* createStore(Value* val, Value* ptr, Location_t& t);
	static StructType* getAryStruct(Type* elem_ty);
	static Value* createGEP(Value* ptr, AST* index, bool isInsertZero, Location_t& t);
};

class Debug {
public:
	static void dumpType(Value* v);
	static void Debug::dumpLine(Location_t& t);
};