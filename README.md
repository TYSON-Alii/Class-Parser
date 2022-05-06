### Class-Parser
Features:
- [x] Class Name
- [x] Base Classes
- [ ] Comments
- [ ] Debug
- [ ] Templates
- [ ] Macros
- [ ] Namespaces
- [ ] C-Style Function Pointers
- [x] using/typedef Specifier
- [x] FunctionOperators/Cast Operators
- [x] Nested Classes
- Constructors
  - [x] Accesbility (public, private or protected)
  - [x] Arguments
  - [x] Function Body
  - [x] Tokens (const, static etc..)
- Variables
  - [x] Accesbility
  - [x] Name
  - [x] Value
  - Type
    - [x] Name
    - [x] Tokens
    - [x] pointers
- Functions
  - [x] Accesbility
  - [x] Name
  - [x] Arguments
  - [x] Function Body
  - Type
    - [x] Name
    - [x] Tokens
    - [x] pointers
#### Example
```cpp
const str& code = R"(
class XsColor : public vex4ub {
public:
	using vex4ub::vex4;
	XsColor() : vex4ub(255) { };
	XsColor(const XsColor&) = default;
	XsColor& operator=(const XsColor& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; };
	inline operator int() const { return (int(r)<<16) + (int(g)<<8) + int(b); }
	inline operator std::string() const {
		return ::str("R: ") + std::to_string(r) +
			", G: " + std::to_string(g) +
			", B: " + std::to_string(b) +
			", A: " + std::to_string(a);
	};
	inline operator vex4f() const { return to_rgb32(); };
	inline operator vex4d() const { return to_rgb64(); };
	bool falanke;
	byte &r = vex4ub::x, &g = vex4ub::y, &b = vex4ub::z, &a = vex4ub::w;
	inline XsColor& hex(int hex_code) { return self = XsColor((hex_code >> 16) & 0xff, (hex_code >> 8) & 0xff, (hex_code) & 0xff); };
	inline XsColor& hex(const ::str& hex_code) { return hex(std::stoi(hex_code, nullptr, 16)); };
	XsColor& hsv(const vex3f& _hsv) { };
	vex3f to_hsv() const { };
	inline ::str to_hex() const { };
	inline vex4f to_rgb32() const { return (vex4f() << self) / 255.f; };
	inline vex4d to_rgb64() const { return (vex4d() << self) / 255.0; };
};
)";

auto main() -> int {
	ParseClass parser(code);
	cout << parser;
	return 0;
};
```
```
OUTPUT:
Class Name: XsColor
Base Classes:
- pub class vex4ub
Constructors:
- pub XsColor() :vex4ub(255) {  }
- pub XsColor(const XsColor&) = default
Cast Operators:
- pub operator int() const { return(int(r)<<16)+(int(g)<<8)+int(b); }
- pub operator std::string() const { return::str("R:")+std::to_string(r)+", G: "+std::to_string(g)+", B: "+std::to_string(b)+", A: "+std::to_string(a); }
- pub operator vex4f() const { return to_rgb32(); }
- pub operator vex4d() const { return to_rgb64(); }
Variables:
- pub bool falanke
- pub byte& r = vex4ub::x
- pub byte& g = vex4ub::y
- pub byte& b = vex4ub::z
- pub byte& a = vex4ub::w
Functions:
- pub XsColor& operator=(const XsColor& v) { x=v.x;y=v.y;z=v.z;w=v.w;return*this; }
- pub inline XsColor& hex(int hex_code) { return self=XsColor((hex_code>>16)&0xff,(hex_code>>8)&0xff,(hex_code)&0xff); }
- pub inline XsColor& hex(const ::str& hex_code) { return hex(std::stoi(hex_code,nullptr,16)); }
- pub XsColor& hsv(const vex3f& _hsv) {  }
- pub vex3f to_hsv() const {  }
- pub inline ::str to_hex() const {  }
- pub inline vex4f to_rgb32() const { return(vex4f()<<self)/255.f; }
- pub inline vex4d to_rgb64() const { return(vex4d()<<self)/255.0; }
Types:
- XsColor
- vex4ub
- bool
- byte
- int
- ::str
- vex3f
- str
- vex4f
- vex4d
```
Another Example:
```cpp
auto main() -> int {
	ParseClass parser(code);
	parser.className;
	auto& base = parser.baseClasses[0];
	base.name;
	base.acces;
	cout << "Constructors:\n";
	for (const auto& cons : parser.constructors) {
		for (const auto& token : cons.tokens)
			cout << ' ' << token;
		cout << parser.className << "();\n";
		cons.acces;
		cons.args;
		cons.tokens;
		cons.value;
	};
	cout << "Variables:\n";
	for (const auto& var : parser.variables) {
		cout << "Type: " << var.type << '\n';
		cout << "Name: " << var.name << '\n';
		cout << "Value: " << var.value << '\n';
		var.acces;
	};
	cout << "Functions:\n";
	for (const auto& func : parser.functions) {
		cout << "Type:" << func.type << '\n';
		cout << "Name: " << func.name << '\n';
		cout << "Args: " << func.args << '\n';
		cout << "Value: " << func.value << '\n';
		func.acces;
	};
	cout << "Used Types:\n";
	for (const auto& type : parser.types)
		cout << type << '\n';
	return 0;
};```

