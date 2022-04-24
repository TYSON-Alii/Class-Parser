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
inline str trim(std::string& s) { return rtrim(ltrim(s)); };
class ParseClass {
public:
	template <typename T>
	using list = pmr::vector<T>;
	using sstream = stringstream;
	enum class acces_t : unsigned char { pub, priv, protect };
	using enum acces_t;
	struct mem_t {
		acces_t acces = priv;
		struct {
			str name;
			list<str> tokens;
			str pointers;
		} type;
		str name, value;
		bool is_func = false;
	};
	mem_t temp_mem;
	struct cons_t {
		acces_t acces = priv;
		list<str> tokens;
		str value;
	};
	list<cons_t> constructors;
	str className;
	struct base_t {
		acces_t acces = priv;
		str name;
	};
	list<base_t> baseClass;
	base_t temp_base;
	list<str> keywords = { "nullptr", "false", "true" };
	list<str> tokens = { "const", "constexpr", "virtual", "static", "inline", "explicit", "friend", "volatile", "short", "long", "signed", "unsigned" };
	list<str> types = { "auto", "bool", "char", "int", "float", "double", "void", "string", "str" };
	ParseClass(const str& _code) : code(_code) {
		static const auto& new_temp = [&]() { 
			temp_mem.is_func = false;
			temp_mem.value.clear();
			temp_mem.name.clear();
			temp_mem.type.tokens.clear();
			temp_mem.type.name.clear();
			temp_mem.type.pointers.clear();
		};
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
		for (auto i = split.begin(); i != split.end(); i++) {
			while (i + 1 != split.end() and *i == ";" and *(i + 1) == ";")
				i++;
			if (i != split.begin() and *(i - 1) == "}" and *i == ";")
				continue;
			fixed_split.push_back(*i);
		};
		split = fixed_split;
		fixed_split.clear();
		const auto& is_in = [](const str& v, const list<str>& l) { for (const auto& i : l) if (v == i) return true; return false; };
		enum class durum : unsigned char { no, class_name, after_class_name, class_base, class_acces, class_mem, mem_type, mem_val, mem_name, mem_after, func_args, func_body, func_var, func_code };
		using enum durum;
		acces_t acces = priv;
		durum d = no;
		bool cons_c = false;
		//for_each(split.begin(), split.end(), [](const auto& v) { cout << v << '\n'; });
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
					baseClass.push_back(temp_base);
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
					while (j + 1 != split.end() and (*(j + 1) == "&" or *(j + 1) == "*" or *(j + 1) == "&&" or *(j + 1) == "**")) {
						j++;
						temp_mem.type.pointers += *j;
					};
					d = mem_name;
				}
				else {
					types.push_back(i);
					temp_mem.type.name = i;
					while (j + 1 != split.end() and (*(j + 1) == "&" or *(j + 1) == "*" or *(j + 1) == "&&" or *(j + 1) == "**")) {
						j++;
						temp_mem.type.pointers += *j;
					};
					d = mem_name;
				}
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
					members.push_back(temp_mem);
					new_temp();
					d = class_mem;
				}
				else if (i == "(") {
					temp_mem.is_func = true;
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
						members.push_back(temp_mem);
						temp_mem.is_func = false;
						temp_mem.value.clear();
						temp_mem.name.clear();
						temp_mem.type.pointers.clear();
						d = mem_name;
					}
					else
						temp_mem.value += i;
				}
				else {
					members.push_back(temp_mem);
					new_temp();
					d = class_mem;
				};
				break;
			case func_args:
				if (i == ")") {
					d = func_body;
				};
				break;
			case func_body:
				if (i == ";") {
					if (cons_c) {
						cons_t temp_c;
						temp_c.acces = temp_mem.acces;
						temp_c.tokens = temp_mem.type.tokens;
						constructors.push_back(temp_c);
						cons_c = false;
					}
					else
						members.push_back(temp_mem);
					new_temp();
					d = class_mem;
				}
				else if (i != "{") {
					d = func_var;
				}
				else {
					d = func_code;
				};
				break;
			case func_var:
				while (j + 1 != split.end() and *j != "{" and *j != ";") {
					temp_mem.value += *j;
					j++;
				};
				if (*j == "{")
					d = func_code;
				else {
					if (cons_c) {
						cons_t temp_c;
						temp_c.acces = temp_mem.acces;
						temp_c.tokens = temp_mem.type.tokens;
						temp_c.value = temp_mem.value;
						constructors.push_back(temp_c);
						cons_c = false;
					}
					else
						members.push_back(temp_mem);
					new_temp();
					d = class_mem;
				};
				break;
			case func_code:
				while (j + 1 != split.end() and *j != "}") {
					temp_mem.value += " "s + *j;
					j++;
				};
				if (cons_c) {
					cons_t temp_c;
					temp_c.acces = temp_mem.acces;
					temp_c.tokens = temp_mem.type.tokens;
					temp_c.value = temp_mem.value;
					constructors.push_back(temp_c);
					cons_c = false;
				}
				else
					members.push_back(temp_mem);
				new_temp();
				d = class_mem;
				break;
			};
		};
	};
	list<mem_t> members;
private:
	str code;
};
