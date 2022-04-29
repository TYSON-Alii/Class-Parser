### Class-Parser
Features:
- [x] Class Name
- [x] Base Classes
- [ ] Comments
- [ ] Debug
- [ ] Templates
- [x] using/typedef Specifier
- [ ] Nested Classes
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
#include "magic_enum.hpp"
using namespace magic_enum;
const str& code = R"(
class Shape : public Transform, Entity {
public: // remove comments
	using str = string;
	typedef void* ptr;
	Shape() = default;
	Shape(const Shape&) = delete;
	virtual void Destroy(const str& falan, int** filan = nullptr, char);
	const char* tag = nullptr; // why not std::string
	bool froze = false;
	inline vex3f& move(const vex3f& xyz) const { return pos += xyz; }; /* dfsafesd */
	Shape& reset() { return (*this) = Shape(); };
	constexpr mat4 matrix() const { };
	// list<Shape*> child; now not support
};
)";

auto main() -> int {
	ParseClass parser(code);
	cout << parser;
};
```
```
OUTPUT:
Class Name: Shape
Base Classes:
- pub class Transform
- priv class Entity
Constructors:
- pub Shape() = { }
- pub Shape(const Shape&) = { }
Members:
- pub virtual void Destroy(const str& falan, int** filan = nullptr, char) = { }
- pub const char* tag = nullptr
- pub bool froze = false
- pub inline vex3f& move(const vex3f& xyz) =  const {return pos+=xyz; }
- pub Shape& reset() =  {return(*this)=Shape(); }
- pub constexpr mat4 matrix() =  const { }
Types:
- Shape
- Transform
- Entity
- str
- ptr
- void
- int
- char
- bool
- vex3f
- mat4
```
Another Example:
```cpp
auto main() -> int {
	ParseClass parser(code);
	parser.className;
	auto& baseClasses = parser.baseClass;
	auto& base = baseClasses[0];
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
	cout << "Members:\n";
	for (const auto& mem : parser.members) {
		cout << "Type:";
		for (const auto& token : mem.type.tokens)
			cout << ' ' << token;
		cout << mem.type.name << mem.type.pointers << '\n';
		cout << "Name: " << mem.name << '\n';
		cout << "Value: " << mem.value << '\n';
		mem.acces;
		mem.args;
		mem.is_func;
	};
	cout << "Used Types:\n";
	for (const auto& type : parser.types)
		cout << type << '\n';
	return 0;
};```

