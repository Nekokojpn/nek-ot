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

	tok_num_int,
	tok_num_double,
	tok_num_long,
	tok_num_long_long,
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
		class Input {
			public: 
				static void CreateFunc();
		};
		class File {
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

typedef struct {
	AllocaInst ainst;
	bool isArr;
	std::vector<unsigned long long> arrsize;
	TypeKind kind;
}Alloca_t;

typedef union {
	short s;
	int i;
	long l;
	float f;
	double d;
} Val_u;
//<-----

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


class Codegen {
	bool isNowGlobal = false;
public:
	//-----> LLVM functions
	static void call_writef(llvm::ArrayRef<llvm::Value*> args);
	static void call_exit(int exitcode);
	static void call_error(int exitcode);
	static Type* getTypebyAType(AType& ty);
	static Type* getTypebyType(Type_t& t);
	static void gen_asm(std::string statement, std::string option);
	static void init_on_inst();
	static std::tuple<bool, int> getValueInt(Value* c);
	//<-----
	void setIsGlobal(bool _isGlobal) { isNowGlobal = _isGlobal; }
	bool IsGlobal() { return isNowGlobal; }
	static void declareFunction(std::string func_name, std::string ac_func_name);
};

extern std::unique_ptr<Module> module;
extern LLVMContext context;
extern IRBuilder<> builder;

extern std::map<std::string, FunctionCallee> functions_global;
extern std::map<std::string, StructType*> userdefined_stcts;
extern std::map<std::string, Stct_t> userdefined_stcts_elements;
extern std::map<std::string, AllocaInst*> namedvalues_global;
extern std::map<std::string, AllocaInst*> namedvalues_local;
extern std::map<std::string, bool> namedvalues_local_isinitialized;
extern std::map<std::string, Value*> namedvalues_str;
extern Value* underscore;
extern std::map<std::string, BasicBlock*> jmp_labels;
extern std::map<std::string, std::vector<BasicBlock*>> jmp_bbs;

extern bool isSubst;

extern std::pair<bool, std::vector<Value*>> if_rets;
extern std::vector<BasicBlock*> if_rets_bb;

//ASTArrayIndexes------>
extern std::vector<Value*> idx_list;
extern bool isArrTy;
//<------

//AST Identifier
extern Value* current_inst;
extern bool isPtr;


extern Value* lambdavalue;

extern AllocaInst* retvalue;
extern std::vector<BasicBlock*> retbbs;

extern std::vector<BasicBlock*> brk_bbs;

extern bool retcodegen;
extern bool gotocodegen;
extern bool isStringCodegen;


class ASTSubst;
class ASTFunc;
class ASTStctElements;
class ASTArrElements;

class AST {
public:
	Location_t loc;
	virtual Value* codegen() = 0;
	virtual Type* getType() = 0;
};
class ASTIdentifierBase : public AST {
public:
	std::string name;
	ASTIdentifierBase(const std::string& _name) : name(_name) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTIdentifierArrayElementBase : public AST {
public:
	std::string name;
	std::unique_ptr<AST> indexes;
	ASTIdentifierArrayElementBase(const std::string& _name, std::unique_ptr<AST> _indexes) : name(_name), indexes(std::move(_indexes)) {};
	Value* codegen() override;
	Type* getType() override;
};

class ASTIdentifier : public AST {
public:
	std::unique_ptr<AST> lhs; //lhs
	std::unique_ptr<AST> rhs; //For stct, rhs;
	TypeKind kind;
	ASTIdentifier(std::unique_ptr<AST> _lhs, std::unique_ptr<AST> _rhs, TypeKind _kind) : lhs(std::move(_lhs)), rhs(std::move(_rhs)), kind(_kind) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTValue : public AST {
public:
	long long value;
	double value_d;
	bool isDouble;
	bool isLongLong;
	ASTValue(long long  _value) : value(_value),isDouble(false) {};
	ASTValue(double  _value, bool _isDouble) : value_d(_value), isDouble(true) {};
	ASTValue(long long  _value, bool _isLongLong) : value(_value), isLongLong(true) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTStrLiteral : public AST {
public:
	std::string value;
	ASTStrLiteral(std::string _value) : value(_value) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTBinOp : public AST {
public:
	std::unique_ptr<AST> lhs;
	std::unique_ptr<AST> rhs;
	Op op;
	ASTBinOp(std::unique_ptr<AST> _lhs, Op _op, std::unique_ptr<AST> _rhs) : lhs(std::move(_lhs)), op(_op), rhs(std::move(_rhs)), curTy(nullptr) {} ;
	Type* curTy;
	Value* codegen() override;
	Type* getType() override;
};


class ASTType : public AST {
public:
	Type_t ty;
	std::string name;
	std::unique_ptr<AST> id;
	/* FOR ARRAY ATTRIBUTES */
	std::unique_ptr<ASTArrElements> elements;
	//
	/* FOR STRUCT ATTRIBUTES */
	std::string stct_name;
	//
	bool isGlobal;
	std::unique_ptr<ASTSubst> expr;
	ASTType(Type_t _ty, std::unique_ptr<AST> _id, std::unique_ptr<ASTSubst> _expr, std::string _stct_name, bool _isGlobal) : ty(_ty), id(std::move(_id)), expr(std::move(_expr)), stct_name(_stct_name), isGlobal(_isGlobal) {};
	ASTType(Type_t _ty, std::unique_ptr<AST> _id, std::unique_ptr<ASTArrElements> _elements, std::string _stct_name, bool _isGlobal) : ty(_ty), id(std::move(_id)), elements(std::move(_elements)), stct_name(_stct_name), isGlobal(_isGlobal) {};
	Value* codegen() override;
	Type* getType() override;
};


class ASTAction : public AST {
public:
	std::string name;
	std::unique_ptr<ASTFunc> lambda;	
	ASTAction(std::string _name, std::unique_ptr<ASTFunc> _lambda) : name(_name), lambda(std::move(_lambda)) {};
	Value* codegen();
	Type* getType() override;
};
class ASTStruct : public AST {
public:
	std::string name;
	std::unique_ptr<ASTStctElements> elements;
	ASTStruct(std::string _name, std::unique_ptr<ASTStctElements> _elements) : name(_name), elements(std::move(_elements)) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTStctElements : public AST {
public:
	Stct_t elements;
	ASTStctElements(Stct_t _elements) : elements(_elements) {};
	Value* codegen() override;
	ArrayRef<Type*> make_aref();
	Type* getType() override;
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
	Type* getType() override;
};


class ASTString : public AST {
public:
	std::string name;
	std::unique_ptr<ASTStrLiteral> expr_str;
	ASTString(std::string _name, std::unique_ptr<ASTStrLiteral> _expr_str) : name(_name), expr_str(std::move(_expr_str)) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTRet : public AST {
public:
	Type_t ret_type;
	std::unique_ptr<AST> expr_p;
	ASTRet(Type_t _ret_type) : ret_type(_ret_type) {};
	Value* codegen();
	Type* getType() override;
};

class ASTBrk : public AST {
public:
	ASTBrk() {};
	Value* codegen();
	Type* getType() override;
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
};
class ASTFunc : public AST {
public:
	std::unique_ptr<ASTProto> proto;
	std::vector<std::unique_ptr<AST>> body;
	ASTFunc(std::unique_ptr<ASTProto> _proto, std::vector<std::unique_ptr<AST>> _body) : proto(std::move(_proto)), body(std::move(_body)) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTCall : public AST {
public:
	std::string name;
	std::vector<std::unique_ptr<AST>> args_expr;
	ASTCall(std::string _name, std::vector<std::unique_ptr<AST>> _args_expr) : name(_name), args_expr(std::move(_args_expr)) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTElse : public AST {
public:
	std::vector<std::unique_ptr<AST>> body;
	ASTElse(std::vector<std::unique_ptr<AST>> _body) : body(std::move(_body)) {};
	Value* codegen() override;
	Type* getType() override;
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
	Type* getType() override;

	BasicBlock* bodyBB;
	BasicBlock* contBB;
};
class ASTFor : public AST {
public:
	std::unique_ptr<AST> typedeff;
	std::unique_ptr<ASTIf> proto;
	std::unique_ptr<AST> last;
	ASTFor(std::unique_ptr<AST> _typedeff, std::unique_ptr<ASTIf> _proto, std::unique_ptr<AST> _last) : typedeff(std::move(_typedeff)), proto(std::move(_proto)), last(std::move(_last)) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTWhile : public AST {
public:
	std::unique_ptr<AST> proto; //BoolOp
	std::vector<std::unique_ptr<AST>> body;
	ASTWhile(std::unique_ptr<AST> _proto, std::vector<std::unique_ptr<AST>> _body) : proto(std::move(_proto)), body(std::move(_body)) {};
	Value* codegen() override;
	Type* getType() override;
};

class ASTTop : public AST {
public:
	std::vector<std::unique_ptr<AST>> globals;
	ASTTop(std::vector<std::unique_ptr<AST>> _globals) : globals(std::move(_globals)) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTGoto : public AST {
public:
	std::string label;
	ASTGoto(std::string _label) : label(_label) {};
	Value* codegen() override;
	Type* getType() override;
};
class ASTLabel : public AST {
public:
	std::string label;
	ASTLabel(std::string _label) : label(_label) {};
	Value* codegen() override;
	Type* getType() override;
};

class ASTSubst : public AST {
public:
	std::unique_ptr<AST> id;
	std::unique_ptr<AST> expr;
	std::vector<std::unique_ptr<AST>> body;
	ASTSubst(std::unique_ptr<AST> _id, std::unique_ptr<AST> _expr) :id(std::move(_id)), expr(std::move(_expr)) {};
	ASTSubst(std::unique_ptr<AST> _id, std::vector<std::unique_ptr<AST>> _body) :id(std::move(_id)), body(std::move(_body)) {};
	ASTSubst(std::unique_ptr<AST> _id) : id(std::move(_id)) {};
	Value* codegen() override;
	Type* getType() override;
};

class ASTBool : public AST {
public:
	bool value;
	ASTBool(bool _value) : value(_value) {};
	Value* codegen();
	Type* getType() override;
};

// [1][2] etc...
class ASTArrayIndexes : public AST {
public:
	std::unique_ptr<AST> lhs;
	std::unique_ptr<AST> rhs;
	ASTArrayIndexes(std::unique_ptr <AST> _lhs, std::unique_ptr<AST> _rhs) : lhs(std::move(_lhs)), rhs(std::move(_rhs)) {};
	Value* codegen() override;
	Type* getType() override;
};
/*
class ASTImport : public AST {
public:
};
*/
class Parser {
	int index;
	Token_t curtok;
	std::string curval; //for type;
	std::unique_ptr<Codegen> cdgen;
	std::map<std::string, Token_t> stcts;
	std::vector<Token_t> tokens;

	bool curtokIs(TK);
	bool nexttokIs(TK);
	std::vector<std::unique_ptr<AST>> afs;

	std::unique_ptr<AST> expr();
	std::unique_ptr<AST> expr_add();
	std::unique_ptr<AST> expr_mul();
	std::unique_ptr<AST> expr_plmi();
	std::unique_ptr<AST> expr_primary();
	std::unique_ptr<AST> bool_expr();
	std::unique_ptr<AST> Parser::bool_expr_op();
	std::unique_ptr<ASTArrElements> expr_arr();
	std::unique_ptr<ASTStctElements> expr_stct(); 
	std::unique_ptr<ASTStrLiteral> expr_str();
	std::unique_ptr<ASTGoto> expr_goto();
	std::unique_ptr<AST> expr_anno();
	std::unique_ptr<AST> expr_star();
	std::vector<std::unique_ptr<AST>> expr_block(bool);
	std::unique_ptr<AST> expr_identifier();
	std::unique_ptr<ASTSubst> subst_expr(std::unique_ptr<AST>);
	std::unique_ptr<AST> expr_var();
	std::unique_ptr<AST> expr_array_indexes(); //return an ASTArrayIndex;

	std::unique_ptr<ASTType> def_type(std::unique_ptr<AST>);
	std::unique_ptr<ASTString> def_string();
	std::unique_ptr<ASTFunc> def_func();
	std::unique_ptr<ASTAction> def_action();
	std::unique_ptr<ASTRet> def_ret();
	std::unique_ptr<AST> def_stct();
	std::unique_ptr<ASTBrk> def_brk();
	std::unique_ptr<ASTLabel> def_label(std::string&);

	std::unique_ptr<ASTCall> func_call(std::string, bool);
	std::unique_ptr<ASTIf> bool_statement();
	std::unique_ptr<ASTFor> for_statement();
	std::unique_ptr<ASTWhile> while_statement();

	std::unique_ptr<AST> expr_identifiers(); //Gen ASTIdentifier
	void def_import();
	bool consume(TK tk) noexcept;
	void Parser::getNextToken() noexcept;
public:
	Parser(std::vector<Token_t>);
	std::vector<std::unique_ptr<AST>> parse();
	void codegen(std::vector<std::unique_ptr<AST>>);
	void dump();
	AType getATypeByCurtok();
	Type_t getTypeFromCurtok();
	void add_userdefined_stct(Token_t&);
	bool find_userdefined_stct(std::string);
	std::vector<std::string> imports;
	bool isExpectedSemi = true;
};
