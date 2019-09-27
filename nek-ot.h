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
#include "llvm/BitStream/BitstreamWriter.h"

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

	tok_ret,
	tok_void,
	tok_i32,
	tok_float,
	tok_double,
	tok_short,
	tok_i64,
	tok_char,
	tok_string,
	tok_action,
	tok_var,
	tok_stct, //struct

	tok_num_int,
	tok_num_double,
	tok_str_string,

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


	// operator
	tok_plus,
	tok_minus,
	tok_star,
	tok_slash,
	tok_plpl, //++
	tok_mimi, //--
	tok_ampamp, // &&

	tok_identifier,

	tok_eof,
	tok_unknown

};
class Test {
public:
	static void CreateFunc();
};

class Sys {
public:
	class IO {
	public:
		class OutPuti8Ptr {
		public:
			static void CreateFunc();
		};
		class Printf {
			
		public:
			static void CreateFunc();
		};
		class Printfln {
			public:
				static void CreateFunc();
		};
		class Input {
			public: 
				static void CreateFunc();
		};
	};
	class Cast {
	public:
		class CastInt32toInt8ptr {
		public:
			static void CreateFunc();
		};
		class CastInt32toInt8Array {
		public:
			static void CreateFunc();
		};
	};
	class Internal {
	public:
		class Chkstk {
		public:
			static void CreateFunc();
		};
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
	unsigned long long idx;
	Type* ty;
	std::string identifier;
} Stct_t;
typedef struct {
	AType ty;
	bool isArr;
	std::vector<unsigned long long> arrsize;
	TypeKind kind;
}Type_t;
typedef struct {
	AllocaInst ainst;
	bool isArr;
	std::vector<unsigned long long> arrsize;
	TypeKind kind;
}Alloca_t;
//<-----

void error(std::string title, std::string message, Token_t& curtok);
void error(std::string title, std::string message, Location_t& loc);
void error(std::string title, std::string message, uint32_t line, uint32_t column);
void error_expected(std::string lit, Token_t& curtok);
void error_unexpected(Token_t& curtok);
void warning(std::string title, std::string message, uint32_t line, uint32_t column);
void warning(std::string title, std::string message, Location_t& loc);
void warning(std::string title, std::string message, Token_t& curtok);
void warning_unexpected(Token_t& curtok);
void warning_expected(std::string lit, Token_t& curtok);

void add_err_msg(std::string _errmsg);
void add_warning_msg(std::string _warningmsg);


void init_parse();
AllocaInst* createEntryBlockAlloca(Function* function, const std::string& name);

Module* getModule();

class Console {
public:
	static void SetConsoleTextGray();
	static void SetConsoleTextRed();
	static void SetConsoleTextGreen();
	static void SetConsoleTextWhite();
	static void SetConsoleTextBlue();
};

void get_char();
void undo_char();
void addToliteral();
bool compare_cs(const char* str);
TK gettoken();

int load_source(std::string source_path);

enum class Op{
	Plus,
	Minus,
	Mul,
	Div,
	RDiv,
	LThan,
	LThanEqual,
	RThan,
	RThanEqual,
	EqualEqual,
	NotEqual
};
enum class AType { //AllType
	Nop,
	I32,
	Float,
	Double,
	Char,
	String,
	Struct,
	Void
};
enum class TypeKind {
	Value,
	Pointer,
	Reference
};



class Codegen {
public:
	//-----> LLVM functions
	static void call_writefln(llvm::ArrayRef<llvm::Value*> args);
	static void call_writef(llvm::ArrayRef<llvm::Value*> args);
	static Type* getTypebyAType(AType& ty);
	static Type* getTypebyType(Type_t& t);
	//<-----
};

class ASTSubst;
class ASTFunc;
class ASTStctElements;
class ASTArrElements;

class AST {
public:
	Location_t loc;
	virtual Value* codegen() = 0;
};

class ASTIdentifier : public AST {
public:
	std::string name;
	TypeKind kind;
	ASTIdentifier(const std::string& _name, TypeKind _kind) : name(_name), kind(_kind) {};
	Value* codegen() override;
};
class ASTIdentifierArrayElement : public AST {
public:
	std::string name;
	TypeKind kind;
	std::vector<std::unique_ptr<AST>> expr_v;
	ASTIdentifierArrayElement(std::string _name, std::vector<std::unique_ptr<AST>> _expr_v, TypeKind _kind) : name(_name), expr_v(std::move(_expr_v)), kind(_kind) {};
	Value* codegen();
};
class ASTIdentifierStctElement : public AST {
public:
	std::string name;
	std::vector<std::string> elem_names;
	ASTIdentifierStctElement(std::string _name, std::vector<std::string> _elem_names) :name(_name), elem_names(_elem_names) {};
	Value* codegen() override;
};
class ASTValue : public AST {
public:
	long long value;
	ASTValue(long long  _value) : value(_value) {};
	Value* codegen() override;
};
class ASTStrLiteral : public AST {
public:
	std::string value;
	ASTStrLiteral(std::string _value) : value(_value) {};
	Value* codegen() override;
};
class ASTBinOp : public AST {
public:
	std::unique_ptr<AST> lhs;
	std::unique_ptr<AST> rhs;
	Op op;
	ASTBinOp(std::unique_ptr<AST> _lhs, Op _op, std::unique_ptr<AST> _rhs) : lhs(std::move(_lhs)), op(_op), rhs(std::move(_rhs)) {} ;
	Value* codegen() override;
};


class ASTType : public AST { // i32 i = 0;
public:
	Type_t ty;
	std::string name;
	/* FOR ARRAY ATTRIBUTES */
	std::unique_ptr<ASTArrElements> elements;
	//
	/* FOR STRUCT ATTRIBUTES */
	std::string stct_name;
	//
	std::unique_ptr<ASTSubst> expr;
	ASTType(Type_t _ty, std::string _name, std::unique_ptr<ASTSubst> _expr, std::string _stct_name) : ty(_ty), name(_name), expr(std::move(_expr)), stct_name(_stct_name) {};
	ASTType(Type_t _ty, std::string _name, std::unique_ptr<ASTArrElements> _elements, std::string _stct_name) : ty(_ty), name(_name), elements(std::move(_elements)), stct_name(_stct_name) {};
	Value* codegen() override;
};


class ASTAction : public AST {
public:
	std::string name;
	std::unique_ptr<ASTFunc> lambda;	
	ASTAction(std::string _name, std::unique_ptr<ASTFunc> _lambda) : name(_name), lambda(std::move(_lambda)) {};
	Value* codegen();
};
class ASTStruct : public AST {
public:
	std::string name;
	std::unique_ptr<ASTStctElements> elements;
	ASTStruct(std::string _name, std::unique_ptr<ASTStctElements> _elements) : name(_name), elements(std::move(_elements)) {};
	Value* codegen() override;
};
class ASTStctElements : public AST {
public:
	std::vector<std::pair<Type_t, std::string>> elements;
	ASTStctElements(std::vector<std::pair<Type_t, std::string>> _elements) : elements(_elements) {};
	Value* codegen() override;
	ArrayRef<Type*> make_aref();
};
class ASTArrElements : public AST {
public:
	std::vector<std::unique_ptr<AST>> elements;
	//for comprehension
	std::unique_ptr<AST> restraint;
	std::unique_ptr<ASTType> arr_type;
	ASTArrElements(std::unique_ptr<AST> _restraint, std::unique_ptr<ASTType> _arr_type) : restraint(std::move(_restraint)), arr_type(std::move(_arr_type)) {};
	ASTArrElements(std::vector<std::unique_ptr<AST>> _elements) : elements(std::move(_elements)) {};
	Value* subst(Value* arr, std::vector<unsigned long long> arr_size_v);
	Value* codegen() override;
};


class ASTString : public AST {
public:
	std::string name;
	std::unique_ptr<ASTStrLiteral> expr_str;
	ASTString(std::string _name, std::unique_ptr<ASTStrLiteral> _expr_str) : name(_name), expr_str(std::move(_expr_str)) {};
	Value* codegen() override;
};
class ASTRet : public AST {
public:
	Type_t ret_type;
	std::unique_ptr<AST> expr_p;
	ASTRet(Type_t _ret_type) : ret_type(_ret_type) {};
	Value* codegen();
};

class ASTProto : public AST {
public:
	std::string name;
	std::vector<Type_t> args;
	std::vector<std::string> identifier;
	Type_t ret;
	ASTProto(std::string _name, std::vector<Type_t> _args, std::vector<std::string> _identifier, Type_t _ret) : name(_name), args(_args), identifier(_identifier), ret(_ret) {};
	Value* codegen() override;
};
class ASTFunc : public AST {
public:
	std::unique_ptr<ASTProto> proto;
	std::vector<std::unique_ptr<AST>> body;
	ASTFunc(std::unique_ptr<ASTProto> _proto, std::vector<std::unique_ptr<AST>> _body) : proto(std::move(_proto)), body(std::move(_body)) {};
	Value* codegen() override;
};
class ASTCall : public AST {
public:
	std::string name;
	std::vector<std::unique_ptr<AST>> args_expr;
	ASTCall(std::string _name, std::vector<std::unique_ptr<AST>> _args_expr) : name(_name), args_expr(std::move(_args_expr)) {};
	Value* codegen() override;
};
class ASTElse : public AST {
public:
	std::vector<std::unique_ptr<AST>> body;
	ASTElse(std::vector<std::unique_ptr<AST>> _body) : body(std::move(_body)) {};
	Value* codegen() override;
};
/*
class ASTIfExpr : public AST {
public:
	std::unique_ptr<AST> cond, then, els;
	ASTIfExpr(std::unique_ptr<AST> _cond, std::unique_ptr<AST> _then, std::unique_ptr<AST> _els) : cond(std::move(_cond)), then(std::move(_then)), els(std::move(_els)) {};
	Value* codegen() override;
};
*/
class ASTIf : public AST {
public:
	std::unique_ptr<AST> proto; //BoolOp
	std::vector<std::unique_ptr<AST>> body;
	std::unique_ptr<ASTIf> ast_elif;
	std::unique_ptr<ASTElse> ast_else;
	ASTIf(std::unique_ptr<AST> _proto, std::vector<std::unique_ptr<AST>> _body) : proto(std::move(_proto)), body(std::move(_body)) {};
	Value* codegen() override;
};
class ASTFor : public AST {
public:
	std::vector<std::unique_ptr<AST>> start;
	std::unique_ptr<AST> proto; //BoolOp
	std::vector<std::unique_ptr<AST>> body;
	std::vector<std::unique_ptr<AST>> end;
	//ASTFor(std::unique_ptr<AST> _proto, std::vector<std::unique_ptr<AST>> _body, std::unique_ptr<ASTSubst> _end) : proto(std::move(_proto)), body(std::move(_body)), end(std::move(_end)) {};
	Value* codegen() override;
};
class ASTWhile : public AST {
public:
	std::unique_ptr<AST> proto; //BoolOp
	std::vector<std::unique_ptr<AST>> body;
	ASTWhile(std::unique_ptr<AST> _proto, std::vector<std::unique_ptr<AST>> _body) : proto(std::move(_proto)), body(std::move(_body)) {};
	Value* codegen() override;
};

class ASTSubst : public AST {
public:
	std::unique_ptr<ASTIdentifier> id;
	std::unique_ptr<ASTIdentifierArrayElement> id2;
	std::unique_ptr<AST> expr;
	std::vector<std::unique_ptr<AST>> body;
	ASTSubst(std::unique_ptr<ASTIdentifier> _id, std::unique_ptr<AST> _expr) :id(std::move(_id)), expr(std::move(_expr)) {};
	ASTSubst(std::unique_ptr<ASTIdentifierArrayElement> _id2, std::unique_ptr<AST> _expr) :id2(std::move(_id2)), expr(std::move(_expr)) {};
	ASTSubst(std::unique_ptr<ASTIdentifier> _id, std::vector<std::unique_ptr<AST>> _body) :id(std::move(_id)), body(std::move(_body)) {};
	ASTSubst(std::unique_ptr<ASTIdentifierArrayElement> _id2, std::vector<std::unique_ptr<AST>> _body) :id2(std::move(_id2)), body(std::move(_body)) {};
	ASTSubst(std::unique_ptr<ASTIdentifier> _id) : id(std::move(_id)) {};
	ASTSubst(std::unique_ptr<ASTIdentifierArrayElement> _id2) : id2(std::move(_id2)) {};
	Value* codegen() override;
};
/*
class ASTImport : public AST {
public:
};
*/
class Parser {
	int index;
	Token_t curtok;
	std::map<std::string, Token_t> stcts;
	std::vector<Token_t> tokens;
	std::unique_ptr<AST> expr();
	std::unique_ptr<AST> expr_add();
	std::unique_ptr<AST> expr_mul();
	std::unique_ptr<AST> expr_primary();

	std::unique_ptr<ASTStrLiteral> expr_str();

	std::unique_ptr<ASTType> def_type(const std::string& _id);
	std::unique_ptr<ASTString> def_string();
	std::unique_ptr<ASTFunc> def_func();
	std::unique_ptr<ASTAction> def_action();
	std::unique_ptr<ASTCall> func_call(const std::string& _id, bool isdoll);
	std::vector<std::unique_ptr<AST>> expr_block();
	std::unique_ptr<ASTIf> bool_statement();
	std::unique_ptr<AST> bool_expr();
	std::unique_ptr<ASTFor> for_statement();
	std::unique_ptr<ASTWhile> while_statement();
	std::unique_ptr<AST> expr_identifier();
	std::unique_ptr<ASTSubst> subst_expr(const std::string& _id);
	std::unique_ptr<ASTRet> def_ret();
	std::unique_ptr<AST> def_stct();
	std::unique_ptr<ASTArrElements> expr_arr();
	std::unique_ptr<ASTStctElements> expr_stct();
	std::unique_ptr<AST> expr_dot(std::string& identifier);
	bool consume(TK tk) noexcept;
	void Parser::getNextToken() noexcept;
public:
	Parser(std::vector<Token_t> _tokens);
	void parse_codegen();
	void dump();
	void setOpt(bool b);
	bool getOpt();
	AType getATypeByCurtok();
	Type_t getTypeFromCurtok();
	void add_userdefined_stct(Token_t& cur);
	bool find_userdefined_stct(std::string stct_name);
};
