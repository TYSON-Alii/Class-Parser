#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;
using str = string;
inline str ltrim(str s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
	return s;
};
inline str rtrim(str s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
	return s;
};
inline str trim(const str& s) { return rtrim(ltrim(s)); };
class ParseClass {
public:
	enum class acces_t : unsigned char { pub, priv, protect };
	using enum acces_t;
	template <typename T>
	using list = pmr::vector<T>;
	using sstream = stringstream;
	str className;
	struct type_t {
		str name;
		list<str> tokens;
		str pointers;
	};
	struct arg_t {
		str type, name, value;
		list<str> tokens;
		str pointers;
	};
	struct var_t {
		acces_t acces = priv;
		type_t type;
		str name, value;
	};
	struct func_t : var_t {
		func_t() = default;
		func_t(const var_t& v) : var_t(v) { };
		list<arg_t> args;
	};
	struct cons_t {
		acces_t acces = priv;
		list<str> tokens;
		str value;
		list<arg_t> args;
	};
	struct base_t {
		acces_t acces = priv;
		str name;
	};
	// list<str> keywords = { "nullptr", "false", "true" }; now not required
	list<str> tokens = { "const", "constexpr", "virtual", "static", "inline", "explicit", "friend", "volatile", "short", "long", "signed", "unsigned" };
	list<str> types;// = { "auto", "bool", "char", "int", "float", "double", "void", "string", "str" };
	ParseClass(const str& _code) : code(_code) {
		base_t temp_base;
		var_t temp_mem;
		func_t temp_func;
		const list<str>& ops = { "{", "}","[", "]", "(", ")", "<", ">", "=", "+", "-", "/", "*", "%", "&", "|", "^", ".", ":", ",", ";", "\"", "?", "==", "!=", ">=", "<=", "<<", ">>", "--", "++", "&&", "||", "+=", "-=", "*=", "/=", "%=", "^=", "|=", "&=", "->", "::" };
		const auto& is_in = [](auto v, auto l) { for (const auto& i : l) if (v == i) return true; return false; };
		const auto& is_op = [&](const str& s) { for (const auto& i : ops) if (s == i) return true; return false; };
		list<str> split, fixed_split;
		str temp_str;
		bool literal_c = false, comment_c = false, inline_comment = false;
		for (auto i = code.begin(); i != code.end(); i++) {
			if (literal_c) {
				if (*(i - 1) != '\\' and *i == '"') {
					literal_c = false;
					split.push_back(temp_str + '"');
					temp_str.clear();
				}
				else
					temp_str += *i;
			}
			else if (inline_comment) {
				if (*(i - 1) != '\\' and *i == '\n')
					inline_comment = false;
			}
			else if (comment_c) {
				if (*i == '/' and *(i - 1) == '*')
					comment_c = false;
			}
			else
				switch (*i) {
				case '"':
					literal_c = true;
					temp_str += *i;
					break;
				case '/':
					if (i + 1 != code.end()) {
						if (*(i + 1) == '/') {
							inline_comment = true;
							break;
						}
						else if (*(i + 1) == '*') {
							comment_c = true;
							break;
						};
					};
					temp_str += *i;
					break;
				case ' ':
				case '\t':
				case '\n':
					if (!temp_str.empty()) {
						split.push_back(temp_str);
						temp_str.clear();
					};
					break;
				default:
					temp_str += *i;
				};
		};
		for (const auto& i : split) {
			if (i.front() != '"') {
				temp_str.clear();
				for (auto j = i.begin(); j != i.end(); j++) {
					switch (*j) {
					case '{': case '}':
					case '[': case ']':
					case '(': case ')':
					case '<': case '>':
					case '=': case '+': case '-': case '/': case '*': case '%':
					case '&': case '|': case '^': case '!': case '~':
					case '.': case ':': case ',': case ';':
					case '\'': case '?':
						if (!trim(temp_str).empty()) {
							fixed_split.push_back(trim(temp_str));
							temp_str.clear();
						};
						temp_str += *j;
						if (j + 1 != i.end()) {
							if (// boolean
								(*j == '=' and *(j + 1) == '=') or
								(*j == '!' and *(j + 1) == '=') or
								(*j == '>' and *(j + 1) == '=') or
								(*j == '<' and *(j + 1) == '=') or
								// logical
								(*j == '&' and *(j + 1) == '&') or
								(*j == '|' and *(j + 1) == '|') or
								// shift left/right
								(*j == '<' and *(j + 1) == '<') or
								(*j == '>' and *(j + 1) == '>') or
								// plus plus, minus minus
								(*j == '-' and *(j + 1) == '-') or
								(*j == '+' and *(j + 1) == '+') or
								// arithmetic
								(*j == '+' and *(j + 1) == '=') or
								(*j == '-' and *(j + 1) == '=') or
								(*j == '*' and *(j + 1) == '=') or
								(*j == '/' and *(j + 1) == '=') or
								(*j == '%' and *(j + 1) == '=') or
								// bit set
								(*j == '^' and *(j + 1) == '=') or
								(*j == '|' and *(j + 1) == '=') or
								(*j == '&' and *(j + 1) == '=') or
								// member acces
								(*j == '-' and *(j + 1) == '>') or
								(*j == ':' and *(j + 1) == ':')) {
								temp_str += *(j + 1);
								j++;
							};
						};
						fixed_split.push_back(temp_str);
						temp_str.clear();
						break;
					default:
						temp_str += *j;
					};
				};
				const str& t = trim(temp_str);
				if (!t.empty())
					fixed_split.push_back(t);
			}
			else
				fixed_split.push_back(i);
		};
		split = fixed_split;
		fixed_split.clear();
		str temp_classtr;
		bool class_t = false;
		int class_bc = 0;
		fixed_split.push_back("class");
		for (auto i = split.begin()+1; i != split.end() - 1; i++) {
			const auto& j = *i;
			if (class_t == false and j == "class") {
				class_bc = 0;
				class_t = true;
			};
			if (j == "{")
				class_bc++;
			else if (j == "}")
				class_bc--;
			if (class_t) {
				temp_classtr += j + ' ';
			}
			else
				fixed_split.push_back(j);
			if (class_t and class_bc == 0 and *(i + 1) == ";") {
				i++;
				classes.push_back(ParseClass(temp_classtr));
				temp_classtr.clear();
				class_t = false;
			};
		};
		split = fixed_split;
		fixed_split.clear();
		for (auto i = split.begin(); i != split.end(); i++) {
			while (i + 1 != split.end() and *i == ";" and *(i + 1) == ";")
				i++;
			if (i != split.begin() and *(i - 1) == "}" and *i == ";")
				continue;
			fixed_split.push_back(*i);
		};
		split = fixed_split;
		fixed_split.clear();
		temp_str.clear();
		enum class durum : unsigned char { no, class_name, after_class_name, class_base, class_acces, class_mem, mem_type, mem_val, mem_name, mem_after, func_args, func_body, func_var, func_code, type_alias, typedef_alias };
		using enum durum;
		acces_t acces = priv;
		durum d = no;
		bool cons_c = false, is_func = false;
		const auto& new_temp = [&]() {
			const auto last_acces = temp_mem.acces;
			temp_mem = var_t();
			temp_mem.acces = last_acces;
			temp_func = func_t();
			is_func = false;
		};
		list<str> temp_list;
		const auto& parse_args = [&]() {
			int bracket_c = 0;
			list<arg_t> args;
			arg_t temp_arg;
			bool val_c = false;
			for (const auto& i : temp_list) {
				if (i == "(")
					bracket_c++;
				else if (i == ")")
					bracket_c--;
				if (i == "," and bracket_c == 0) {
					val_c = false;
					temp_arg.value = trim(temp_arg.value);
					if (!is_in(temp_arg.type, types)) types.push_back(temp_arg.type);
					args.push_back(temp_arg);
					temp_arg = arg_t();
				}
				else {
					if (val_c) {
						temp_arg.value += i;
						temp_arg.value += ' ';
					}
					else {
						if (is_in(i, tokens)) {
							temp_arg.tokens.push_back(i);
						}
						else if (i == "=") {
							val_c = true;
						}
						else if (i == "&"s or i == "*"s or i == "&&"s) {
							temp_arg.pointers += i;
						}
						else {
							if (temp_arg.type.empty()) {
								temp_arg.type = i;
							}
							else
								temp_arg.name = i;
						};
					};
				};
			};
			if (!temp_arg.type.empty()) {
				temp_arg.value = trim(temp_arg.value);
				if (!is_in(temp_arg.type, types)) types.push_back(temp_arg.type);
				args.push_back(temp_arg);
			};
			temp_list.clear();
			return args;
		};
		int bracket_c = 0, cbracket_c = 0;
		for (auto j = split.begin(); j != split.end(); j++) {
			const auto& i = *j;
			switch (d) {
			case no:
				if (i == "class"s) {
					d = class_name;
				};
				break;
			case class_name:
				className = i;
				types.push_back(className);
				d = after_class_name;
				break;
			case after_class_name:
				if (i == ":"s) {
					d = class_base;
				}
				else if (i == "{"s) {
					d = class_mem;
				};
				break;
			case class_base:
				if (i == "{")
					d = class_mem;
				else if (i == "public")
					temp_base.acces = pub;
				else if (i == "private")
					temp_base.acces = priv;
				else if (i == "protected")
					temp_base.acces = protect;
				else if (i != ",") {
					temp_base.name = i;
					baseClasses.push_back(temp_base);
					types.push_back(temp_base.name);
					temp_base = base_t();
				};
				break;
			case class_mem:
				if (i == className and *(j + 1) == "(") {
					cons_c = true;
					temp_mem.name = i;
					d = mem_after;
				}
				else if (i == "using") {
					d = type_alias;
				}
				else if (i == "typedef") {
					d = typedef_alias;
				}
				else if (i == "public"s) {
					temp_mem.acces = pub;
					d = class_acces;
				}
				else if (i == "private"s) {
					temp_mem.acces = priv;
					d = class_acces;
				}
				else if (i == "protected") {
					temp_mem.acces = protect;
					d = class_acces;
				}
				else if (is_in(i, tokens)) {
					temp_mem.type.tokens.push_back(i);
				}
				else if (is_in(i, types)) {
					temp_mem.type.name = i;
					while (j + 1 != split.end() and (*(j + 1) == "&" or *(j + 1) == "*" or *(j + 1) == "&&")) {
						j++;
						temp_mem.type.pointers += *j;
					};
					d = mem_name;
				}
				else {
					if (i != "{" and i != "}" and
						i != "[" and i != "]" and
						i != ";" and i != ",") {
						types.push_back(i);
						temp_mem.type.name = i;
						while (j + 1 != split.end() and (*(j + 1) == "&" or *(j + 1) == "*" or *(j + 1) == "&&" or *(j + 1) == "**")) {
							j++;
							temp_mem.type.pointers += *j;
						};
						d = mem_name;
					}
					else
						d = class_mem;
				};
				break;
			case type_alias:
				if (*(j + 1) == "=") {
					temp_str = *j;
					while (*j != ";") j++;
					types.push_back(temp_str);
					temp_str.clear();
					d = class_mem;
				};
				break;
			case typedef_alias:
				while (*j != ";") j++;
				types.push_back(*(j - 1));
				d = class_mem;
				break;
			case class_acces:
				if (i == ":"s)
					d = class_mem;
				break;
			case mem_name:
				temp_mem.name = i;
				d = mem_after;
				break;
			case mem_after:
				if (i == ";") {
					variables.push_back(temp_mem);
					new_temp();
					d = class_mem;
				}
				else if (i == "(") {
					is_func = true;
					bracket_c = 1;
					temp_func = temp_mem;
					d = func_args;
				}
				else if (i == "[") {
				}
				else if (i == "=") {
					d = mem_val;
				};
				break;
			case mem_val:
				if (i != ";") {
					if (i == ",") {
						variables.push_back(temp_mem);
						is_func = false;
						temp_mem.value.clear();
						temp_mem.name.clear();
						temp_mem.type.pointers.clear();
						d = mem_name;
					}
					else
						temp_mem.value += i;
				}
				else {
					variables.push_back(temp_mem);
					new_temp();
					d = class_mem;
				};
				break;
			case func_args:
				if (i == ")") {
					bracket_c--;
					if (bracket_c == 0) {
						temp_func.args = parse_args();
						d = func_body;
					};
				}
				else if (i == "(")
					bracket_c++;
				else
					temp_list.push_back(i);
				break;
			case func_body:
				if (i == ";") {
					if (temp_func.value.starts_with("="))
						temp_func.value = str(temp_func.value.begin()+1, temp_func.value.end());
					temp_func.value = trim(temp_func.value);
					if (cons_c) {
						cons_t temp_c;
						temp_c.acces  = temp_func.acces;
						temp_c.tokens = temp_func.type.tokens;
						temp_c.args   = temp_func.args;
						temp_c.value  = temp_func.value;
						constructors.push_back(temp_c);
						cons_c = false;
					}
					else
						functions.push_back(temp_func);
					new_temp();
					d = class_mem;
				}
				else if (i != "{") {
					temp_func.value += *j;
					if (!is_in(*j, ops) and !is_in(*(j + 1), ops))
						temp_func.value += ' ';
					// d = func_var;
				}
				else {
					cbracket_c = 1;
					temp_func.value = trim(temp_func.value);
					d = func_code;
				};
				break;
			case func_var:
				while (j != split.end() and *j != "{" and *j != ";") {
					temp_func.value += " "s + *j;
					j++;
				};
				if (*j == "{")
					d = func_code;
				else {
					if (cons_c) {
						cons_t temp_c;
						temp_c.acces  = temp_func.acces;
						temp_c.tokens = temp_func.type.tokens;
						temp_c.value  = temp_func.value;
						temp_c.args   = temp_func.args;
						constructors.push_back(temp_c);
						cons_c = false;
					}
					else
						functions.push_back(temp_func);
					new_temp();
					d = class_mem;
				};
				break;
			case func_code:
				if (!temp_func.value.empty())
					temp_func.value += ' ';
				temp_func.value += "{ ";
				while (j + 1 != split.end()) {
					if (*j == "}")
						cbracket_c--;
					else if (*j == "{")
						cbracket_c++;
					if (cbracket_c == 0)
						break;
					temp_func.value += *j;
					if (!is_in(*j, ops) and !is_in(*(j + 1), ops))
						temp_func.value += ' ';
					j++;
				};
				temp_func.value += " }";
				if (cons_c) {
					cons_t temp_c;
					temp_c.acces  = temp_func.acces;
					temp_c.tokens = temp_func.type.tokens;
					temp_c.value  = temp_func.value;
					temp_c.args   = temp_func.args;
					constructors.push_back(temp_c);
					cons_c = false;
				}
				else
					functions.push_back(temp_func);
				new_temp();
				d = class_mem;
				break;
			};
		};
	};
	list<ParseClass> classes;
	list<var_t> variables;
	list<func_t> functions;
	list<base_t> baseClasses;
	list<cons_t> constructors;
private:
	str code;
};
ostream& operator<<(ostream& os, const ParseClass::type_t& type) {
	for (const auto& token : type.tokens)
		os << token << ' ';
	os << type.name << type.pointers;
	return os;
};
ostream& operator<<(ostream& os, const pmr::vector<ParseClass::arg_t>& args) {
	for (const auto& arg : args) {
		for (const auto& token : arg.tokens)
			os << token << ' ';
		os << arg.type << arg.pointers;
		if (!arg.name.empty())
			os << ' ' << arg.name;
		if (!arg.value.empty())
			os << " = " << arg.value;
		os << ", ";
	};
	if (!args.empty())
		os << "\b \b\b";
	return os;
};
ostream& operator<<(ostream& os, const ParseClass::var_t& variable) {
	os << ((variable.acces == ParseClass::acces_t::pub) ? "pub"s : (variable.acces == ParseClass::acces_t::priv) ? "priv"s : "protect"s);
	os << ' ' << variable.type << ' ' << variable.name;
	if (!variable.value.empty()) os << " = " << variable.value;
	return os;
};
ostream& operator<<(ostream& os, const ParseClass::func_t& func) {
	os << ((func.acces == ParseClass::acces_t::pub) ? "pub"s : (func.acces == ParseClass::acces_t::priv) ? "priv"s : "protect"s);
	os << ' ' << func.type << ' ' << func.name << '(' << func.args << ") ";
	if (!func.value.empty())
		if (func.value == "0" or func.value == "delete" or func.value == "default" or func.value == "nullptr")
			os << "= " << func.value;
		else
			os << func.value;
	return os;
};
std::ostream& operator<<(std::ostream& os, const ParseClass& parser) {
	static const auto& enum_name = [](auto en) -> str { if (en == ParseClass::acces_t::pub) return "pub"; else if (en == ParseClass::acces_t::priv) return "priv"; else if (en == ParseClass::acces_t::protect) return "protect"; return "none"; };
	os << "Class Name: " << parser.className << '\n';
	if (!parser.baseClasses.empty()) {
		os << "Base Classes:\n";
		for (const auto& base : parser.baseClasses)
			os << "- " << enum_name(base.acces) << " class " << base.name << '\n';
	};
	if (!parser.constructors.empty()) {
		os << "Constructors:\n";
		for (const auto& cons : parser.constructors) {
			os << "- " << enum_name(cons.acces) << ' ';
			os << parser.className << '(' << cons.args << ") ";
			if (cons.value == "0" or cons.value == "delete" or cons.value == "default" or cons.value == "nullptr")
				os << "= " << cons.value;
			else
				os << cons.value;
			os << '\n';
		};
	};
	if (!parser.variables.empty()) {
		os << "Variables:\n";
		for (const auto& variable : parser.variables) {
			os << "- " << variable << '\n';
		};
	};
	if (!parser.functions.empty()) {
		os << "Functions:\n";
		for (const auto& func : parser.functions) {
			os << "- " << func << '\n';
		};
	};
	if (!parser.types.empty()) {
		os << "Types:\n";
		for (const auto& type : parser.types)
			os << "- " << type << '\n';
	};
	if (!parser.classes.empty()) {
		os << "Nested Classes:\n";
		for (const auto& clss : parser.classes)
			os << clss << '\n';
	};
	return os;
};
