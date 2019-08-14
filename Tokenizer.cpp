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
std::vector<Location_t> locs;
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
void skip_line() {
	while (true) {
		cc = source[line][column++];
		if (cc == '\n'  || cc == '\0') {
			line++;
			column = 0;
			cc = source[line][column++];
			break;
		}
	}
}
void addToliteral() { literals.push_back(cs); }
bool compare_cs(const char* str) { return cs == str; };
void addToloc(int len) {
	Location_t loc;
	loc.location_begin_line = line;
	loc.location_begin_column = column;
	loc.location_end_line = line;
	loc.location_end_column = line + len;
	locs.push_back(loc);
}
//<-----

// tokenizer----->
TK gettoken() {
	get_char();
	cs = "";
	//skip any spaces.
	while (isspace(cc)) {
		get_char();
	}
	if (cc == '\"') {
		addToliteral();
		addToloc(1);
		if (!isdq_started) isdq_started = true;
		else isdq_started = false;
		return TK::tok_dq;
	}
	if (isdq_started) { // string
		while (cc != '\"' && cc != '\0') {
			cs += cc;
			get_char();
			if (cc == ' ') { cs += cc; get_char(); }
		}
		if (cc == '\0') {
			error("Expected", "Expected --> \"", line, column);
		}
		undo_char();
		addToliteral();
		addToloc(cs.length());
		return TK::tok_str_string;
	}
	if (isalpha(cc)) { // Regex, [A-Z]|[a-z]+[digit]*
		cs = cc;
		get_char();
		while (true) {
			while (isalnum(cc)) {
				cs += cc;
				get_char();
			}
			if (cc == '_') {
				cs += cc;
				get_char();
			}
			if (cc == '[') {
				get_char();
				if (cc == ']') {
					undo_char();
					undo_char();
					get_char();
					cs += cc;
					get_char();
					cs += cc;
					get_char();
				}
				else {
					undo_char();
					//get_char();
					undo_char();
					get_char();
					break;
				}
			}
			else break;
		}
		undo_char();
		addToliteral();
		if (cs == "fn") { addToloc(cs.length()); return TK::tok_fn; }
		else if (cs == "i32") { addToloc(cs.length()); return TK::tok_i32; }
		else if (cs == "i32[]") { addToloc(cs.length()); return TK::tok_i32_arr; }
		else if (cs == "void") { addToloc(cs.length()); return TK::tok_void; }
		else if (cs == "ret") { addToloc(cs.length()); return TK::tok_ret; }
		else if (cs == "float") { addToloc(cs.length()); return TK::tok_float; }
		else if (cs == "double") { addToloc(cs.length()); return TK::tok_double; }
		else if (cs == "short") { addToloc(cs.length()); return TK::tok_short; }
		else if (cs == "long") { addToloc(cs.length()); return TK::tok_i64; }
		else if (cs == "char") { addToloc(cs.length()); return TK::tok_char; }
		else if (cs == "string") { addToloc(cs.length()); return TK::tok_string; }
		else if (cs == "if") { addToloc(cs.length()); return TK::tok_if; }
		else if (cs == "elif") { addToloc(cs.length()); return TK::tok_elif; }
		else if (cs == "else") { addToloc(cs.length()); return TK::tok_else; }
		else if (cs == "for") { addToloc(cs.length()); return TK::tok_for; }
		else if (cs == "while") { addToloc(cs.length()); return TK::tok_while; }
		else if (cs == "new") { addToloc(cs.length()); return TK::tok_new; }
		else if (cs == "action") { addToloc(cs.length()); return TK::tok_action; }
		else if (cs == "tok_var") { addToloc(cs.length()); return TK::tok_var; }
		else { addToloc(cs.length()); return TK::tok_identifier; }
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
		addToloc(cs.length());
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
		addToloc(1);
		if (cc == '\0') return TK::tok_eof;
		if (cc == '=') {
			get_char();
			if (cc == '=') { // ==
				return TK::tok_eqeq;
			}
			else if (cc == '>') { // =>
				return TK::tok_darrow;
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
				skip_line();
				return gettoken();
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
		if (cc == ',')	return TK::tok_comma;
		if (cc == '_')	return TK::tok_under;
		if (cc == '.')	return TK::tok_dot;
		if (cc == '[')	return TK::tok_lpb;
		if (cc == ']')	return TK::tok_rpb;
		if (cc == '%') return TK::tok_percent;
	}
	std::string s = "";
	s += cc;
	error("Unknown type", "Unknown token '" + s + "'", line, column);
	literals.push_back(s);
	return TK::tok_unknown;
}
//<-----

