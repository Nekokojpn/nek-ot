#include "nek-ot.h"

extern int line;
extern int column;

// Tokenizer globals----->
std::vector<std::string> source;
char cc;        // Current char.
std::string cs; // Current string EX. identiflier,number.
std::vector<TK> tokens;
std::vector<std::string> literals;
std::vector<Token_t> tytokens;
bool isdq_started = false;
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
		column = source[line].size() - 1;
	}
}
void addToliteral() { literals.push_back(cs); }
bool compare_cs(const char* str) { return cs == str; };
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
	else { //‹L†. •K‚¸1•¶Žš
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

