### Class-Parser
Features:
- [x] Class Name
- [x] Base Classes
- [ ] Comments
- [ ] Debug
- [ ] Templates
- [x] using/typedef Specifier
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
class Shape : public Transform, Entity {
public: // remove comments
	using str = string;
	typedef void* ptr;
	Shape() = default;
	Shape(const Shape&) = delete;
	Shape(const char* t) : tag(t) { cout << "okii\n"; };
	virtual bool Destroy(const str& falan, int** filan = nullptr, char) {
		cout << "falan filan.. kisaca calismiyor." << '\n';
		return false;
	};
	str tag;
	bool froze = false, show = true;
	inline vex3f& move(const vex3f& xyz) noexcept { return pos += xyz; }; /* dfsafesd */
	Shape& reset() { return (*this) = Shape(); };
	// list<Shape*> child; now not support
	class Nested {
	public:
	bool is_wow = true;
		void wow() {
			if (is_wow)
				cout << "wow!!" << endl;
			else
				cout << "not wow." << endl;
		};
		class SuperNested {
		public:
		bool is_amazing = false;
			void use() {
				if (is_amazing)
					cout << "yesyesyesyyesysyesyesyes!!" << endl;
				else
					cout << "not now." << endl;
			};
		};
	};
protected:
	constexpr mat4 matrix() final;
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
Class Name: Shape
Base Classes:
- pub class Transform
- priv class Entity
Constructors:
- pub Shape() = default
- pub Shape(const Shape&) = delete
- pub Shape(const char* t) :tag(t) { cout<<"okii\n"; }
Variables:
- pub str tag
- pub bool froze = false
- pub bool show = true
Functions:
- pub virtual bool Destroy(const str& falan, int** filan = nullptr, char) { cout<<"falan filan.. kisaca calismiyor."<<' \n ';return false; }
- pub inline vex3f& move(const vex3f& xyz) noexcept { return pos+=xyz; }
- pub Shape& reset() { return(*this)=Shape(); }
- protect constexpr mat4 matrix() final
Types:
- Shape
- Transform
- Entity
- str
- ptr
- char
- bool
- int
- vex3f
- mat4
Nested Classes:
Class Name: Nested
Variables:
- pub bool is_wow = true
Functions:
- pub void wow() { if(is_wow)cout<<"wow!!"<<endl;else cout<<"not wow."<<endl; }
Types:
- Nested
- bool
- void
Nested Classes:
Class Name: SuperNested
Variables:
- pub bool is_amazing = false
Functions:
- pub void use() { if(is_amazing)cout<<"yesyesyesyyesysyesyesyes!!"<<endl;else cout<<"not now."<<endl; }
Types:
- SuperNested
- bool
- void
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

