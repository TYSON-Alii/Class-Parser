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
	virtual void Destroy(str falan);
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
	cout << "Class Name: " << parser.className << '\n';
	if (!parser.baseClass.empty()) {
		cout << "Base Classes:\n";
		for (const auto& base : parser.baseClass)
			cout << "- " << enum_name(base.acces) << " class " << base.name << '\n';
	};
	if (!parser.constructors.empty()) {
		cout << "Constructors:\n";
		for (const auto& cons : parser.constructors) {
			cout << "- " << enum_name(cons.acces) << ' ';
			for (const auto& token : cons.tokens)
				cout << token << ' ';
			cout << parser.className << "() = " << cons.value << '\n';
		};
	};
	if (!parser.members.empty()) {
		cout << "Members:\n";
		for (const auto& mem : parser.members) {
			cout << "- " << enum_name(mem.acces) << ' ';
			for (const auto& token : mem.type.tokens)
				cout << token << ' ';
			cout << mem.type.name << mem.type.pointers << ' ';
			if (mem.is_func)
				cout << mem.name << "(" << mem.args << ") = ";
			else
				cout << mem.name << " = ";
			if (!mem.value.empty()) {
				cout << mem.value << '\n';
			}
			else
				cout << "None\n";
		};
	};
	if (!parser.types.empty()) {
		cout << "Types:\n";
		for (const auto& type : parser.types)
			cout << "- " << type << '\n';
	};
};
```
```
OUTPUT:
Class Name: Shape
Base Classes:
- pub class Transform
- priv class Entity
Constructors:
- pub Shape() =
- pub Shape() =
Members:
- pub virtual void Destroy(str falan) = None
- pub const char* tag = nullptr
- pub bool froze = false
- pub inline vex3f& move(const vex3f & xyz) =  const { return pos += xyz ; }
- pub Shape& reset() =  { return ( * this ) = Shape ( ) ; }
- pub constexpr mat4 matrix() =  const { }
Types:
- Shape
- Transform
- Entity
- str
- ptr
- void
- char
- bool
- vex3f
- mat4
```
