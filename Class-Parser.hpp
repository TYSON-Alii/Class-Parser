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
	struct cast_t {
		acces_t acces = priv;
		str type, value;
		list<str> tokens;
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
	list<str> tokens {"const", "constexpr", "virtual", "static", "inline", "explicit", "friend", "volatile", "register", "short", "long", "signed", "unsigned" };
	list<str> keywords { "return", "break", "case", "catch", "class", "concept", "continue", "decltype", "default", "delete", "do", "else", "if", "enum", "export", "extern", "for", "goto", "namespace", "new", "noexcept", "operator", "private", "public", "protected", "requires", "sizeof", "struct", "switch", "template", "throw", "try", "typedef", "typename", "union", "while" };
	list<str> ops {
		"{", "}","[", "]", "(", ")", "<", ">", "=", "+", "-", "/", "*", "%", "&", "|", "^", ".", ":", ",", ";", "?",
		"==", "!=", ">=", "<=", "<<", ">>", "--", "++", "&&", "||", "+=", "-=", "*=", "/=", "%=", "^=", "|=", "&=", "->", "::",
		"<=>", "<<=", ">>="
	};
	struct lit_pair { str beg, end; };
	list<lit_pair> lits { {"\"","\"" }, {"'","'" }, { "//","\n" }, { "/*", "*/" }, {"#", "\n"} };
	/*
	struct macro_t { str name, value; };
	struct fmacro_t { str name, value; list<str> args; };
	list<macro_t> macros = { {"and","&&" },{"or","||" },{"not","!" } };
	*/
	list<str> types;
	ParseClass() = default;
	ParseClass(const str& _code, bool is_fullcode = true) : code(_code) { parse(); };
	void parse() {
		base_t temp_base;
		var_t temp_mem;
		func_t temp_func;
		const auto& is_in = [](auto v, auto l) { for (const auto& i : l) if (v == i) return true; return false; };
		const auto& is_op = [&](const str& s) { for (const auto& i : ops) if (s == i) return true; return false; };
		const auto& is_ac = [&](const str& s) { return s == "public" or s == "private" or s == "protected"; };
		const auto& is_ptr = [&](const str& s) { return s == "*" or s == "&" or s == "&&"; };
		const auto& f_ac = [&](const str& s) {
			if (s == "public")
				return pub;
			else if (s == "private")
				return priv;
			else if (s == "protected")
				return protect;
			return priv;
		};
		list<str> split = split_code(code), fixed_split;
		str temp_str, temp_classtr;
		for (auto it = split.begin(); it != split.end(); it++) {
			auto& i = *it;
			const auto& it1 = it + 1;
			if (i.starts_with("//") or i.starts_with("/*")) continue;
			else if (it1 != split.end()) {
				auto& i1 = *it1;
				if (i == "::") {
					if (!(is_in(*(it-1), tokens) or is_in(*(it-1), keywords)))
						fixed_split.back() += i + i1;
					else
						fixed_split.push_back(i + i1);
					it++;
				}
				else if (i == "operator" and is_op(i1)) {
					fixed_split.push_back(i + i1);
					it++;
				}
				else
					fixed_split.push_back(i);
			}
			else
				fixed_split.push_back(i);
		};
		split = fixed_split;
		fixed_split.clear();
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
			while (i + 1 != split.end() and *i == ";" and *(i + 1) == ";") i++;
			fixed_split.push_back(*i);
		};
		split = fixed_split;
		fixed_split.clear();
		temp_str.clear();
		enum class durum : unsigned char { no, class_mem };
		using enum durum;
		acces_t acces = priv;
		durum d = no;
		list<str> temp_list;
		const auto& add_t = [&](const str& t) { if (!is_in(t, types)) types.push_back(t); };
		const auto& new_temp = [&]() {
			temp_mem = var_t();
			temp_mem.acces = acces;
			temp_func = func_t();
		};
		const auto& parse_args = [&]() {
			int bracket_c = 0;
			list<arg_t> args;
			arg_t temp_arg;
			bool val_c = false, type_t = false;
			auto j = temp_list.begin();
			while (j != temp_list.end()) {
				auto& i = *j;
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
						if (j != temp_list.end() and (j + 1) != temp_list.end() and !is_in(i, ops) and !is_in(*(j + 1), ops))
							temp_arg.value += ' ';
					}
					else {
						while (j != temp_list.end()) {
							if (!is_in(*j, tokens))
								break;
							temp_arg.tokens.push_back(*j);
							j++;
						}
						while (j != temp_list.end()) {
							if (temp_arg.type.empty() and !is_op(*j)) {
								temp_arg.type += *j;
								j++;
							}
							else if (*j == "::" or (temp_arg.type.ends_with("::") and !is_op(*j))) {
								temp_arg.type += *j;
								j++;
							}
							else
								break;
						};
						while (j != temp_list.end()) {
							if (!(*j == "&" or *j == "*" or *j == "&&"))
								break;
							temp_arg.pointers += *j;
							j++;
						}
						if (j != temp_list.end()) {
							if (*j != "=") {
								temp_arg.name = *j;
							}
							else
								val_c = true;
							j++;
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
		const auto& func_value = [&](auto& it, auto& temp, auto& temp_value, auto& list) {
			if (*it == ";") {
				list.push_back(temp);
				new_temp();
			}
			else if (*it == "=") {
				it++;
				while (*it != ";") {
					temp_value += *it;
					if (*it == "{" or (*it == "}" and *(it+1) != ";") or *(it+1) == "}" or *it == ";" or !is_in(*it, ops) and !is_in(*(it + 1), ops) or is_in(*it, keywords)) temp_value += ' ';
					it++;
				};
				temp_value = trim(temp_value);
				list.push_back(temp);
				new_temp();
			}
			else {
				while (*it != "{" and *it != ";") {
					temp_value += *it;
					if (*it == "{" or (*it == "}" and *(it+1) != ";") or *(it+1) == "}" or *it == ";" or !is_in(*it, ops) and !is_in(*(it + 1), ops) or is_in(*it, keywords)) temp_value += ' ';
					it++;
				};
				if (*it == ";") {
					list.push_back(temp);
					new_temp();
				}
				else {
					it++;
					temp_value = trim(temp_value);
					if (!temp_value.empty())
						temp_value += ' ';
					temp_value += "{ ";
					int bc = 0, cc = 0, sc;
					while (!(bc == 0 and cc == 0 and *it == "}")) {
						if (*it == "(")
							bc++;
						else if (*it == ")")
							bc--;
						else if (*it == "{")
							cc++;
						else if (*it == "}")
							cc--;
						else if (*it == "[")
							sc++;
						else if (*it == "]")
							sc--;
						temp_value += *it;
						if (*it == "{" or (*it == "}" and *(it+1) != ";") or *(it+1) == "}" or *it == ";" or (!is_in(*it, ops) and !is_in(*(it + 1), ops))) temp_value += ' ';
						it++;
					};
					temp_value += '}';
					temp_value = trim(temp_value);
					list.push_back(temp);
					new_temp();
				};
			};
		};
		const auto& arg_parse = [&](auto& it, auto& temp_arg, auto& fn) {
			while (true) {
				if (*it == ")") { fn.args.push_back(temp_arg); temp_arg = arg_t(); break; }
				else if (*it == ",") fn.args.push_back(temp_arg), temp_arg = arg_t(), it++;
				while (is_in(*it, tokens)) temp_arg.tokens.push_back(*it), it++;
				add_t(temp_arg.type = *it);
				it++;
				while (is_ptr(*it)) temp_arg.pointers += *it, it++;
				if (*it == ")") { fn.args.push_back(temp_arg); temp_arg = arg_t(); break; }
				else if (*it != ",") {
					temp_arg.name = *it;
					it++;
					if (*it == "=") {
						it++;
						int bc = 0, cc = 0, sc = 0;
						while (!(bc == 0 and cc == 0 and (*it == "," or *it == ")"))) {
							if (*it == "(")
								bc++;
							else if (*it == ")")
								bc--;
							else if (*it == "{")
								cc++;
							else if (*it == "}")
								cc--;
							else if (*it == "[")
								cc++;
							else if (*it == "]")
								cc--;
							temp_arg.value += *it;
							if (*it == "{" or (*it == "}" and *(it + 1) != ";") or *(it + 1) == "}" or *it == ";" or !is_in(*it, ops) and !is_in(*(it + 1), ops)) temp_arg.value += ' ';
							it++;
						};
						fn.args.push_back(temp_arg);
						temp_arg = arg_t();
						if (*it == ",")
							it++;
						else
							break;
					};
				};
			};
		};
		const auto& end = split.end();
		// for (const auto& i : split) cout << i << '\n';
		for (auto it = split.begin(); it != split.end(); it++) {
			const auto& i = *it;
			if (d == no) {
				if (i == "class") {
					it++;
					className = *it;
					add_t(className);
					it++;
					while (*it != "{") {
						if (is_ac(*it))
							temp_base.acces = f_ac(*it);
						else if (*it == ",")
							baseClasses.push_back(temp_base), add_t(temp_base.name), temp_base=base_t();
						else
							temp_base.name = *it;
						it++;
					};
					if (!temp_base.name.empty()) baseClasses.push_back(temp_base), add_t(temp_base.name), temp_base=base_t();
					d = class_mem;
				};
			}
			else if (d == class_mem) {
				if (i == "using") {
					it++;
					add_t(*it);
					while (*it != ";") it++;
				}
				else if (i == "typedef") {
					it++;
					while (*it != ";") it++;
					add_t(*(it - 1));
				}
				else if (i == "operator") {
					it++;
					if (!is_op(*it) and *(it + 1) == "(" and *(it + 2) == ")") {
						cast_t temp_cast;
						temp_cast.acces = acces;
						temp_cast.tokens = temp_mem.type.tokens;
						temp_cast.type = *it;
						add_t(*it);
						it += 3;
						func_value(it, temp_cast, temp_cast.value, castOperators);
					};
				}
				else if (is_ac(i)) {
					acces = f_ac(i);
				}
				else if (is_in(i, tokens)) {
					temp_mem.type.tokens.push_back(i);
				}
				else if (i == className and *(it + 1) == "(") {
					it++;
					it++;
					cons_t temp_cons;
					temp_cons.tokens = temp_mem.type.tokens;
					temp_cons.acces = acces;
					temp_cons.args;
					arg_t temp_arg;
					if (*it != ")")
						arg_parse(it, temp_arg, temp_cons);
					it++;
					func_value(it, temp_cons, temp_cons.value, constructors);
				}
				else if (!is_op(i)){
					add_t(temp_mem.type.name = i);
				virgulke:;
					it++;
					while (is_ptr(*it)) temp_mem.type.pointers += *it, it++;
					temp_mem.acces = acces;
					if (*it == "operator")
						it++, temp_mem.name = "operator"s + *it;
					else
						temp_mem.name = *it;
					it++;
					if (*it == ";") {
						variables.push_back(temp_mem);
					}
					else if (*it == ",") {
						variables.push_back(temp_mem);
						temp_mem.type.pointers.clear();
						temp_mem.value.clear();
						goto virgulke;
					}
					else if (*it == "=") {
						it++;
						int bc = 0, cc = 0, sc = 0;
						while (not (bc == 0 and cc == 0 and (*it == ";" or *it == ","))) {
							if (*it == "(")
								bc++;
							else if (*it == ")")
								bc--;
							else if (*it == "{")
								cc++;
							else if (*it == "}")
								cc--;
							else if (*it == "[")
								sc++;
							else if (*it == "]")
								sc--;
							temp_mem.value += *it;
							if (*it == "{" or (*it == "}" and *(it+1) != ";") or *(it+1) == "}" or *it == ";" or !is_in(*it, ops) and !is_in(*(it + 1), ops)) temp_mem.value += ' ';
							it++;
						};
						variables.push_back(temp_mem);
						if (*it == ",") {
							temp_mem.type.pointers.clear();
							temp_mem.value.clear();
							goto virgulke;
						};
						new_temp();
					}
					else if (*it == "(") {
						it++;
						temp_func = temp_mem;
						arg_t temp_arg;
						arg_parse(it, temp_arg, temp_func);
						it++;
						func_value(it, temp_func, temp_func.value, functions);
					};
				};
			}
		};
	}
	str code;
	list<ParseClass> classes;
	list<var_t> variables;
	list<func_t> functions;
	list<base_t> baseClasses;
	list<cons_t> constructors;
	list<cast_t> castOperators;
private:
	list<str> split_code(const str& code) {
		static const auto& is_in = [](auto v, auto l)  { for (const auto& i : l) if (v == i) return true; return false; };
		sort(ops.begin(), ops.end(), [](const str& first, const str& second){ return first.size() > second.size(); });
		sort(lits.begin(), lits.end(), [](const auto& first, const auto& second){ return first.beg.size() > second.beg.size(); });
		str temp_str;
		list<str> split;
		static const auto& new_splt = [&]() {
			if (!temp_str.empty()) {
				split.push_back(temp_str);
				temp_str.clear();
			};
		};
		for (auto it = code.begin(); it != code.end(); it++) {
			const auto& i = *it;
			const auto& it_pos = distance(code.begin(), it);
			if (isspace(i)) {
				new_splt();
				continue;
			};
			for (const auto& l : lits) {
				const auto& len = l.beg.size();
				const auto& end_len = l.end.size();
				if (it_pos + len <= code.size()) {
					const auto& s = str(it, it + len);
					if (s == l.beg) {
						new_splt();
						it += len;
						temp_str += l.beg;
						while (true) {
							const str& s = str(it, it + end_len);
							if (s != l.end) {
								temp_str += *it;
								it++;
							}
							else if (l.end.size() == 1 and s == l.end and *(it - 1) == '\\') {
								temp_str.pop_back();
								temp_str += *it;
								it++;
							}
							else break;
						};
						it += end_len - 1;
						split.push_back(temp_str + l.end);
						temp_str.clear();
						goto _exit;
					};
				};
			};
			for (const auto& op : ops) {
				const auto& len = op.size();
				if (it_pos + len <= code.size()) {
					const auto& s = str(it, it + len);
					if (s == op) {
						new_splt();
						it += len - 1;
						split.push_back(s);
						goto _exit;
					};
				};
			};
			temp_str += i;
		_exit:;
		};
		return split;
	};
};
ostream& operator<<(ostream& os, const ParseClass::type_t& type) {
	for (const auto& token : type.tokens)
		os << token << ' ';
	os << type.name << type.pointers;
	return os;
};
ostream& operator<<(ostream& os, const ParseClass::arg_t& arg) {
	for (const auto& token : arg.tokens)
		os << token << ' ';
	os << arg.type << arg.pointers;
	if (!arg.name.empty())
		os << ' ' << arg.name;
	if (!arg.value.empty())
		os << " = " << arg.value;
	return os;
};
ostream& operator<<(ostream& os, const pmr::vector<ParseClass::arg_t>& args) {
	for (const auto& arg : args)
		os << arg  << ", ";
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
ostream& operator<<(ostream& os, const ParseClass::cast_t& cast) {
	os << ((cast.acces == ParseClass::acces_t::pub) ? "pub"s : (cast.acces == ParseClass::acces_t::priv) ? "priv"s : "protect"s);
	for (const auto& token : cast.tokens)
		os << ' ' << token;
	os << " operator " << cast.type << "() ";
	if (!cast.value.empty())
		if (cast.value == "0" or cast.value == "delete" or cast.value == "default" or cast.value == "nullptr")
			os << "= " << cast.value;
		else
			os << cast.value;
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
	if (!parser.castOperators.empty()) {
		os << "Cast Operators:\n";
		for (const auto& cast : parser.castOperators) {
			os << "- " << cast;
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
