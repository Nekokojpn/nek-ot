#pragma once

#define HIGH_DEBUGG			//ハイ・レベルデバッグ
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
#include "llvm/Bitcode/BitstreamWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Linker/Linker.h"
#include "llvm/IR/InlineAsm.h"

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

using namespace llvm;

// TOKENS--------------->
enum class TK {
	tok_nope = 0,
	tok_fn = 2,
	tok_if = 3,
	tok_elif = 4,
	tok_else = 5,
	tok_for = 6,
	tok_while = 7,
	tok_new = 8,

	tok_ret = 100,
	tok_void = 101,
	tok_i32 = 102,
	tok_float = 103,
	tok_double = 104,
	tok_short = 105,
	tok_i64 = 106,
	tok_char = 107,
	tok_string = 108,
	tok_i32_arr = 109,

	tok_num_int = 200,
	tok_num_double = 201,
	tok_str_string = 202,

	tok_semi = 300, // ;
	tok_equal = 301, // =
	tok_lp = 302, // (
	tok_rp = 303, // )
	tok_lb = 304, // {
	tok_rb = 305, // }
	tok_arrow = 306, // ->
	tok_sq = 307, // '
	tok_dq = 308, // "
	tok_lt = 309, // <
	tok_lteq = 310, // <=
	tok_rt = 311, // >
	tok_rteq = 312, // >=
	tok_eqeq = 313, // ==
	tok_em = 314, // !
	tok_emeq = 315, //!=
	tok_comma = 316, // ,
	tok_under = 317, // _
	tok_dot = 318,
	tok_lpb = 319, // [
	tok_rpb = 320, // ]
	tok_percent = 321, // %
	tok_darrow = 322, // =>

	// operator
	tok_plus = 400,
	tok_minus = 401,
	tok_star = 402,
	tok_slash = 403,
	tok_plpl = 404, //++
	tok_mimi = 405, //--

	tok_identifier = 500,

	tok_eof = -1,
	tok_unknown = 0

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
};



// typedef----->
typedef struct {
	int location_begin_line;
	int location_begin_column;
	int location_end_line;
	int location_end_column;
} Location_t;
typedef struct {
	TK ty;
	std::string val;
	Location_t loc;
} Token_t;
//<-----

void error(std::string title, std::string message, Token_t& curtok);
void error(std::string title, std::string message, Location_t& loc);
void error(std::string title, std::string message, int line, int column);
void error_unexpected(Token_t& curtok);

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
	Void
};
enum class ATypeArr {

};



class Codegen {
public:
	//-----> LLVM functions
	static void call_writefln(llvm::ArrayRef<llvm::Value*> args);
	static Type* getTypebyAType(AType ty);
	//<-----
};

class ASTSubst;

class AST {
public:
	Location_t loc;
	virtual Value* codegen() = 0;
};
class ASTIdentifier : public AST {
public:
	std::string name;
	ASTIdentifier(const std::string& _name) : name(_name) {};
	Value* codegen() override;
};
class ASTIdentifierArrayElement : public AST {
public:
	std::string name;
	std::unique_ptr<AST> expr;
	ASTIdentifierArrayElement(std::string _name, std::unique_ptr<AST> _expr) : name(_name), expr(std::move(_expr)) {};
	Value* codegen();
};
class ASTValue : public AST {
public:
	int value;
	ASTValue(int _value) : value(_value) {};
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
	AType ty;
	std::string name;
	std::unique_ptr<ASTSubst> expr;
	ASTType(AType _ty, std::string _name, std::unique_ptr<ASTSubst> _expr) : ty(_ty), name(_name), expr(std::move(_expr)) {};
	Value* codegen() override;
};

class ASTInt : public AST { //修正
public:
	std::string name;
	std::unique_ptr<ASTSubst> expr_p;
	ASTInt(std::string _name, std::unique_ptr<ASTSubst> _expr_p) : name(_name), expr_p(std::move(_expr_p)) {};
	Value* codegen() override;
};
class ASTIntArray : public AST {
public:
	std::string name;
	int size;
	ASTIntArray(std::string _name, int _size) : name(_name), size(_size) {};
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
	AType ret_type;
	std::unique_ptr<AST> expr_p;
	ASTRet(AType _ret_type) : ret_type(_ret_type) {};
	Value* codegen();
};
class ASTProto : public AST {
public:
	std::string name;
	std::vector<AType> args;
	std::vector<std::string> identifier;
	AType ret;
	ASTProto(std::string _name, std::vector<AType> _args, std::vector<std::string> _identifier, AType _ret) : name(_name), args(_args), identifier(_identifier), ret(_ret) {};
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
	std::unique_ptr<AST> proto;
	std::vector<std::unique_ptr<AST>> body;
	//ASTFor()
	
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
class Parser {
	int index;
	Token_t curtok;
	std::vector<Token_t> tokens;
	std::unique_ptr<AST> expr();
	std::unique_ptr<AST> expr_add();
	std::unique_ptr<AST> expr_mul();
	std::unique_ptr<AST> expr_primary();

	std::unique_ptr<ASTStrLiteral> expr_str();

	std::unique_ptr<ASTType> def_type();
	std::unique_ptr<ASTIntArray> def_int_arr();
	std::unique_ptr<ASTString> def_string();
	std::unique_ptr<ASTFunc> def_func();
	std::unique_ptr<ASTCall> func_call(const std::string& _id);
	std::vector<std::unique_ptr<AST>> expr_block();
	std::unique_ptr<ASTIf> bool_statement();
	std::unique_ptr<AST> bool_expr();
	std::unique_ptr<ASTFor> for_statement();
	std::unique_ptr<ASTWhile> while_statement();
	std::unique_ptr<AST> expr_identifier();
	std::unique_ptr<ASTSubst> subst_expr(const std::string& _id);
	std::unique_ptr<ASTRet> def_ret();

	
	
	
	bool consume(TK tk) noexcept;
	void Parser::getNextToken() noexcept;
public:
	Parser(std::vector<Token_t> _tokens);
	void parse_codegen();
	void dump();
	void setOpt(bool b);
	bool getOpt();
	AType getATypeByCurtok();
};
