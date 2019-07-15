#include "nek-ot.h"

int line;
int column;

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
		cc = source[line][column++];
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
				error("Expected", "Expected --> \"", line, column);
			}
			undo_char();
			addToliteral();
			return TK::tok_str_string;
		}
		undo_char();
		addToliteral();
		if (cs == "fn") { return TK::tok_fn; }
		else if (cs == "int") { return TK::tok_int; }
		else if (cs == "void") { return TK::tok_void; }
		else if (cs == "ret") { return TK::tok_ret; }
		else if (cs == "float") { return TK::tok_float; }
		else if (cs == "double") { return TK::tok_double; }
		else if (cs == "short") { return TK::tok_short; }
		else if (cs == "long") { return TK::tok_long; }
		else if (cs == "char") { return TK::tok_char; }
		else if (cs == "string") { return TK::tok_string; }
		else if (cs == "if") { return TK::tok_if; }
		else if (cs == "elif") { return TK::tok_elif; }
		else if (cs == "else") { return TK::tok_else; }
		else if (cs == "for") { return TK::tok_for; }
		else if (cs == "while") { return TK::tok_while; }
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
	else { // Symbol.
		std::string s = "";
		s += cc;
		literals.push_back(s);
		if (cc == '\0') return TK::tok_eof;
		if (cc == '=') {
			get_char();
			if (cc == '=') { // ==
				return TK::tok_eqeq;
			}
			undo_char();
			return TK::tok_equal;
		}
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
		if (cc == '<') {
			get_char();
			if (cc == '=') { // <=
				return TK::tok_lteq;
			}
			undo_char();
			return TK::tok_lt;
		}
		if (cc == '>') {
			get_char();
			if (cc == '=') { // >=
				return TK::tok_rteq;
			}
			undo_char();
			return TK::tok_rt;
		}
		if (cc == '!') {
			get_char();
			if (cc == '=') { // !=
				return TK::tok_emeq;
			}
			undo_char();
			return TK::tok_em;
		}

	}
	std::string s = "";
	s += cc;
	error("Unknown type", "Unknown token '" + s + "'", line, column);
	literals.push_back(s);
	return TK::tok_unknown;
}
//<-----

