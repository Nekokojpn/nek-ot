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

	tok_ret = 100,
	tok_void = 101,
	tok_int = 102,
	tok_float = 103,
	tok_double = 104,
	tok_short = 105,
	tok_long = 106,
	tok_char = 107,
	tok_string = 108,

	tok_num_int = 200,
	tok_num_double = 201,
	tok_str_string = 202,

	tok_semi = 300,
	tok_equal = 301,
	tok_lp = 302, // (
	tok_rp = 303, // )
	tok_lb = 304, // {
	tok_rb = 305, // }
	tok_arrow = 306, // ->
	tok_sq = 307, // '
	tok_dq = 308, // "

	// operator
	tok_plus = 400,
	tok_minus = 401,
	tok_star = 402,
	tok_slash = 403,

	tok_identifier = 500,

	tok_eof = -1,
	tok_unknown = 0

};

// typedef----->
typedef struct {
	TK ty;
	std::string val;
	int location_begin;
	int location_end;
} Token_t;
//<-----

void error(std::string title, std::string message, int line, int column);

LLVMContext& getContext();
IRBuilder<>& getBuilder();
Module* getModule();
std::map<std::string, Value*>& getNamedValues_Local();


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
	Div
};
enum class AType {
	Int,
	Float,
	Double,
	Char,
	String
};
class AST {
public:
	virtual Value* codegen() = 0;
};
class ASTIdentifier : public AST {
public:
	Value* value;
	ASTIdentifier(Value* _value) : value(_value) {};
	Value* codegen() override;
};
class ASTValue : public AST {
public:
	int value;
	ASTValue(int _value) : value(_value) {};
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
class ASTInt : public AST {
public:
	std::string name;
	std::unique_ptr<AST> expr_p;
	ASTInt(std::string _name) : name(_name) {};
	Value* codegen() override;
};
class ASTArgProto : public AST { // int 
public:
	std::string identifier;
	Value* userDefinedType;
	AType type;
	bool isUserDefined;
	ASTArgProto(Value* _userDefinedType, std::string _identifier) : userDefinedType(_userDefinedType), identifier(_identifier) { isUserDefined = true; };
	ASTArgProto(AType _type, std::string _identifier) : type(_type), identifier(_identifier) { isUserDefined = false; };
	Value* codegen() override;
};
class ASTProto : public AST {
public:
	std::string name;
	std::vector<ASTArgProto> args;
	ASTProto(std::string _name, std::vector<ASTArgProto> _args) : name(_name), args(_args) {};
	Value* codegen() override;
};
class ASTFunc : public AST {
public:
	std::unique_ptr<ASTProto> proto;
	std::unique_ptr<AST> body;
	ASTFunc(std::unique_ptr<ASTProto> _proto, std::unique_ptr<AST> _body) : proto(std::move(_proto)), body(std::move(_body)) {};
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
	std::unique_ptr<ASTInt> def_int();
	std::unique_ptr<ASTFunc> def_func();
	std::unique_ptr<AST> expr_block();
	
	bool consume(TK tk);
	void Parser::getNextToken();
public:
	Parser(std::vector<Token_t> _tokens);
	void parse_codegen();
};
