# Atams C++ Coding Standards

## Introduction

The **Atams Coding Standards** define the rules for writing safe, portable, and maintainable embedded C++ code used in the Atams library.

This document balances style guidelines to keep the codebase consistent, and safety rules to increase software reliability. Brief rationale, exceptions, and examples are included with rules where appropriate.

---

## 1. Memory Management

### Dynamic Memory Allocation: 
- **Rule:** Do not use dynamic (heap) memory allocation (`new`, `malloc`, etc.), including standard library containers or functions that allocate memory dynamically (e.g., `vector`, `string`, `map`).
- **Rationale:** Avoids memory fragmentation, memory leaks, and unpredictable behavior. Improves code predictability and determinism.

### Type Punning
- **Rule:** All type punning shall be performed using std::memcpy. The use of unions or direct use of reinterpret_cast for type punning is prohibited.
- **Rationale:** The C++ standard only guarantees defined behavior when sharing an object’s representation through std::memcpy (C++17). Type punning through unions or reinterpret_cast is undefined behavior and may result in non-portable code. This applies both to conversions between variables and to interpreting data received from external buffers.
- **Compliant Example:**
```cpp
float   f = 1.0F;
int32_t i;

std::memcpy(&i, &f, sizeof(i));
```

- **Non-compliant `reinterpret_cast` Example:**
```cpp
float    f = 1.0F;
uint32_t i = *reinterpret_cast<std::uint32_t*>(&f);
```

- **Non-compliant `union` Example:**
```cpp
union U_t
{ 
  float    f; 
  uint32_t i; 
};

U_t u;

u.f = 1.0F;

int32_t i = u.i;
```

### Array Declaration and Initialisation
- **Rule:** All array declarations must specify the array size explicitly or allow the compiler to infer the size from an initialiser list. If an array is declared without an explicit initialiser and its contents may affect future program behaviour before being assigned, the array should be explicitly filled with a known value after declaration.
- **Rationale:** Explicit or inferred array sizes prevent accidental buffer overruns and clarify intent. Size inference from an initialiser list guarantees full initialisation, while explicit size with no initialiser leaves the array uninitialised. The initial contents of an uninitialised array are not defined by the C++ standard and can often contain undefined values. Explicitly filling such arrays with a known value ensures predictable behaviour and prevents bugs caused by reading uninitialised data.
- **Example:**
```cpp
constexpr uint8_t BUFFER_SIZE {4U};

// Compliant: Explicit size, uninitialised (but must be filled before access)
uint8_t buffer[BUFFER_SIZE];
std::memset(buffer, 0, sizeof(buffer)); /* Example fill */

// Compliant: Size inferred from initialiser list (fully initialised)
uint8_t values[] = {0U, 0U, 0U, 0U};

// Non-compliant: No size or initialiser
uint8_t data[]; 

// Non-compliant: Size mismatch between declaration and initialiser list
uint8_t numbers[BUFFER_SZIE] = {0U, 0U, 0U}; 
```

### Serialising and Deserialising 
- **Rule:** All byte arrays shared with other systems (external communications, non-volatile storage etc.) must be serialised and deserialised explicitly using bit-shifting and masking operations. Do not copy structs or objects directly into buffers.
- **Rationale:** Explicit bit manipulation ensures deterministic layout and handles differences in system endianness automatically. Compiler-dependent padding, alignment, or member ordering in objects can break portability.
- **Example:**
```cpp
constexpr uint8_t  CONTAINER_SIZE   = 3U;
constexpr uint8_t  ITEM_A_INDEX     = 0U;
constexpr uint8_t  ITEMB_LO_INDEX   = 1U;
constexpr uint8_t  ITEMB_HI_INDEX   = 2U;
constexpr uint16_t ITEMB_BYTE_MASK  = 0xFFU;
constexpr uint8_t  ITEMB_BYTE_SHIFT = 8U;

struct Container_t
{
  uint8_t  itemA {0U};
  uint16_t itemB {0U};
};

Container_t container;
uint8_t buffer[CONTAINER_SIZE];

// Compliant: Serialise
buffer[ITEM_A_INDEX]   = container.itemA;
buffer[ITEMB_LO_INDEX] = static_cast<uint8_t>(container.itemB & ITEMB_BYTE_MASK);
buffer[ITEMB_HI_INDEX] = static_cast<uint8_t>((container.itemB >> ITEMB_BYTE_SHIFT) & ITEMB_BYTE_MASK);

// Compliant: Deserialise
Container_t result;
result.itemA = buffer[ITEM_A_INDEX];
result.itemB = ((static_cast<uint16_t>(buffer[ITEMB_LO_INDEX] & ITEMB_BYTE_MASK)                    ) |
                (static_cast<uint16_t>(buffer[ITEMB_HI_INDEX] & ITEMB_BYTE_MASK) << ITEMB_BYTE_SHIFT) );

// Non-compliant: Copying struct directly into buffer
std::memcpy(buffer, &container, sizeof(Container_t)); // Not allowed

// Non-compliant: Reinterpreting buffer as struct
Container_t* ptr = reinterpret_cast<Container_t*>(buffer); // Not allowed
```


## 2. Constants, Variables, and Casting

### Reinterpret Casting
- **Rule:** Reinterpret casts shall only be used to cast to type `uint8_t*`.
- **Rationale:** Avoids type punning between unrelated types (`float*` -> `uint32_t*`).
- **Exception:** Casting hardware-defined addresses to pointers to the hardware’s specified type.

### Integer Types
- **Rule:** Use `<stdint.h>` for integer types.  
- **Rationale:** Ensures integer type sizes are maintained between platforms. Avoids undefined behavior when passing integer types between systems.

### Float Representation
- **Rule:** Ensure `float` has a size of 4 bytes and has IEC 60559 representation.  
- **Rationale:** Ensures float type size and bit representation is maintained between platforms. Avoids undefined behavior when passing float types between different systems.
- **Example Assertions:**
```cpp
static_assert(sizeof(float) == Atams::MAX_TYPE_SIZE, "Platform float size incompatible with Atams");
static_assert(std::numeric_limits<float>::is_iec559, "Platform float representation incompatible with Atams");
```

### Integer Literals
- **Rule:** Use a capital `U` suffix for all unsigned integer literals up to `uint32_t`, including `uint8_t` and `uint16_t`. The use of unsigned types larger than `uint32_t` is to be discouraged.
- **Rationale:** Using the `U` suffix ensures that unsigned literals are explicit and consistent. The `U` suffix yields an `unsigned int`, which safely converts to `uint8_t`, `uint16_t`, and `uint32_t` on 32 bit or larger targets. Narrowing conversions to smaller widths are intentional and safe when using brace-initialization. `int64_t`, `uint64_t`, and `double` are not compatible with Atams communications and are currently not used in developer code. Suffixes for these types may be included in future guidelines.

### Float Literals
- **Rule:** All float literals must include the decimal and use a capital `F` suffix (e.g., `1.0F`).  
- **Rationale:** Ensures the literal is explicitly a float and prevents accidental interpretation as an integer or implicit conversion from double.

### Variable Initialisation
- **Rule:** Always intitialise variables using brace-initialisation `{}`.  
- **Rationale:** Ensures compile-time checks catch accidental narrowing conversions, wrap-around, and overflow conditions.
- **Example:**
```cpp
// Non-compliant
uint8_t testVar = 15U;

// Compliant
uint8_t testVar {15U};
```

- **Compile-time Warning Examples:**
```cpp
uint8_t testVar1 {300U};      // ! narrowing conversion from unsigned int to uint8_t
uint8_t testVar2 {-1};        // ! narrowing conversion from int to uint8_t
int8_t  testVar3 {128};       // ! narrowing conversion from int to int8_t
uint8_t testVar4 {255U + 1U}; // ! narrowing conversion from unsigned int to uint8_t
uint8_t testVar4 {0.1F};      // ! narrowing conversion from float to uint8_t
```

### Namespace Constants
- **Rule:** Use `constexpr` for all namespace values that are known at compile time.
- **Rationale:** `constexpr` ensures compile-time evaluation and, at namespace scope, is implicitly `inline` in C++17 and later. This avoids multiple definition errors.

### Class Shared Constants
- **Rule:** Use `static constexpr` for class-wide constants that are the same for all instances.
- **Rationale:**  The C++ standard only allows `constexpr` class member variables if they are also `static`. In C++17 and later, these are implicitly `inline` when defined inside the class.

### Class Instance Constants
- **Rule:** Use `const` for values that are set at construction and do not change for the object's lifetime, but may differ between instances.
- **Rationale:** Ensures immutability per object, but allows different values for different instances.

### Auto Keyword
- **Rule:** The `auto` keyword shall not be used.
- **Rationale:** Explicit type declarations improve code readability, maintainability, and type safety.

### Magic Numbers
- **Rule:** No magic numbers. Literals with values other than `0`, `1`, or `2`, must be defined as named constants.
- **Rationale:** Named constants improve code readability, provide context, and make maintenance safer and easier.

### Cast Style
- **Rule:** No C-style casts. Use C++ named casts.
- **Rationale:** C-style casts are not explicit about the type of conversion performed. They can easily introduce unsafe or undefined behavior. C++ named casts make intent clear and restrict dangerous conversions.

### Qualification Removal Casts
- **Rule:** A cast shall not remove any `const` or `volatile` qualification.
- **Rationale:** Removing these qualifiers can break immutability or safe access guarantees, leading to undefined behavior. 

### Pointer Indirection
- **Rule:** Pointer variables should not be declared with more than two levels of indirection.
- **Rationale:** Excessive indirection increases complexity, harms code clarity, and increases the risk of invalid memory access.
```cpp
uint8_t value {0U};

uint8_t   *ptr1 = &value; // Compliant: one level of indirection
uint8_t  **ptr2 = &ptr1;  // Compliant: two levels of indirection
uint8_t ***ptr3 = &ptr2;  // Non-compliant: more than two levels of indirection
```

### Dereferencing Pointers
- **Rule:** A pointer must be checked for nullptr before it is dereferenced, whether accessing the value it points to or passing the pointed-to object by reference to another function.
- **Rationale:** Prevents invalid memory access by ensuring pointers are not nullptr before dereferencing.
- **Example:** 
```cpp
struct Data_t 
{ 
  uint8_t x {0U}; 
};

void processData(Data_t *inputPtr)
{
  if (dataPtr == nullptr)
  {
    return; // Nullptr guard clause return
  }
  
  // Compliant: Both methods of dereferencing safe after nullptr check
  Data_t  localCopy = *inputPtr;   // Direct dereference
  uint8_t value     = inputPtr->x; // Member access via pointer
}
```

### Nullptr
- **Rule:** Always use `nullptr` to represent null pointers. Do not use `NULL` or `0` for pointer initialisation or comparison. 
- **Rationale:** `nullptr` is the standard C++ keyword for representing a null pointer. It provides type safety and avoids the ambiguities and potential errors associated with `NULL`, which is typically defined as `0` or `((void*)0)`.


## 3. Structs, Classes, and Object Design

### POD Objects 
- **Rule:** POD objects should be defined using structs and should be named with an `_t` suffix. Classes should be used for anything more complex.
- **Rational:** Codebase simplicity and consistency.
```cpp
// Compliant: POD object defined as a struct
struct Data_t
{
  uint8_t  id;
  uint16_t value;
};

//Non-compliant: Name missing '_t' suffix
struct Data
{
  uint8_t  id;
  uint16_t value;
};

// Non-compliant: POD object defined as a class
class Data_t
{
  public:
  uint8_t  id;
  uint16_t value;
};

// Non-compliant: Non-POD object defined as a struct
struct Counter_t
{
  void     increment() { count_++;      }
  uint16_t get()       { return count_; }
  uint16_t count_;
};
```

### Special Member Functions
- **Rule:** All non-POD classes must declare the four special member functions (constructor, destructor, copy constructor, move constructor). Un-defined special member functions should be declared as default or deleted.
- **Rationale:** Prevents generation of compiler-defined functions and limits unexpected object behaviours.

### Class Assignment Operators
- **Rule:** All non-POD classes must declare copy assignment, and move assignment operators. Undefined operators should be declared as default or deleted.
- **Rationale:** Prevents generation of compiler-defined functions and limits unexpected object behaviours.

### Composition vs Inheritance
- **Rule:** Prefer composition over inheritance. 
- **Rationale:** Complex inheritance can make a codebase harder to maintain.

### Public Class Variables
- **Rule:** Non-constant public member variables shall not be used. Setters and getters of private variables should be defined instead for more tightly specified object interactions.
- **Rationale:** Clarifies expected class usage, and concurrency locks can be encapsulated in setter/getter functions to avoid race conditions.

### Operator Overloading
- **Rule:** Overloads must be obvious & complete.  
- **Rationale:** Ensures overloads behave as developer expects.
- **Example:**
```cpp
struct Data_t
{
  uint32_t a;
  uint16_t b;
  uint8_t  c;

  bool operator==(const Data_t& other) const
  {
      return ((a == other.a) &&
              (b == other.b) &&
              (c == other.c) ); // Compliant
  }

  bool operator==(const Data_t& other) const 
  {
      return (a == other.a);    // Non-compliant - ignores b and c
  }
};
```

### Derived Class Virtual Functions
- **Rule:** Overriding virtual functions in derived classes must be declared using the `virtual` keyword and marked with `override`.
- **Rationale:** Marks intent, improves code clarity, and allows the compiler to catch accidental non-overrides.
- **Examle:**
```cpp
virtual void functionToOverride override;
```

### Virtual Function Chains
- **Rule:** Limit virtual overrides to one level where possible. Mark overriding virtual functions with `final` to prevent further accidental overriding.
- **Rationale:** Deep override chains can create hard-to-follow call paths and increase maintenance complexity.
- **Examle:**
```cpp
virtual void functionToOverride override final;
```

### Base Class Virtual Functions
- **Rule:** Declare base class virtual functions as pure `(= 0)` if derived classes must override them. If overriding is optional, provide a default implementation and omit `= 0`.
- **Rationale:** Enforces required overrides at compile time while allowing safe default behaviour for optional overrides.
- **Example:**
```cpp
// Base.hpp 
class Base 
{
  virtual void mustOverride(void) = 0; // pure: must be overriden
  virtual void optionalOverride(void); // optional: safe default definition required
};

//Base.cpp
void Base::optionalOverride()
{
  /* Safe default definition */
}

// Derived.hpp
class Derived : 
public Base 
{
  virtual void mustOverride(void) override final;
};

// Derived.cpp
void Derived::mustOverride(void)
{
  /* Pure virtual override definition */
}

```
### Explicit Single-Argument Constructors
- **Rule:** All constructors that take a single argument must be marked explicit, unless implicit conversions are specifically required and justified.
- **Rationale:** Marking single-argument constructors as explicit prevents unintended implicit conversions, making code safer and intent clearer. Implicit conversions can introduce subtle bugs and reduce code readability.
- **Example:**
```cpp
class MyClass
{
  public:

  explicit MyClass(uint8_t value); // Compliant: explicit prevents implicit conversion
};

void foo(MyClass obj);

foo(0U);           // Non-compliant: would compile if MyClass(uint8_t) is not explicit
foo(MyClass(0U));  // Compliant: explicit construction

```


## 4. Functions and Function Arguments

### Function Overloading
- **Rule:** Function overloading shall not be used.
- **Rationale:** Avoids ambiguity and makes code easier to maintain.

### Default Function Arguments
- **Rule:** Default function arguments shall not be used.
- **Rationale:** Ensures all argument values are explicit at call sites, making code behavior clear and preventing accidental bugs caused by overlooked or unintended default values.

### Const Function Arguments
- **Rule:** All function arguments that are not modified within the function body must be declared `const`. This is especially important for reference and pointer arguments.
- **Rationale:** Clarifies function intent and avoids bugs caused by unintended changes to input data - especially when arguments refer to objects outside the function scope.
- **Const Pointer Examples:**
```cpp
// Pointer to const data (data cannot be modified through the pointer)
void processBuffer(const uint8_t *ptr);

// Const pointer (pointer cannot be changed, but data can be modified)
void updateValue(uint8_t * const ptr);

// Const pointer to const data (neither pointer nor data can be modified)
void readBuffer(const uint8_t * const ptr);
```

### Ellipsis Notation
- **Rule:** Function arguments must not use the ellipsis (`...`) notation.
- **Rationale:** Ellipsis notation bypasses compiler type checking, increasing the risk of undefined behavior.

### Pointer Function Arguments
- **Rule:** Functions with pointers arguments must check whether the pointer arguments are nullptr at the start of the function.
- **Exception:** If nullptr checks should be performed by the calling functions to avoid repetitive checks and improve efficiency, the function should be commented with a warning.
- **Rationale:** 
- **Example:**

### Unused Function Arguments
- **Rule:** A function definition must not contain any unused arguments.
- **Rationale:** Keeps code clean, makes function purpose clear, and suppresses compiler warnings.
- **Exception:** Virtual function arguments.

### Unused Virtual Function Arguments
- **Rule:** If function arguments are not used in the definition of a virtual function, they must be cast to void.
- **Rationale:** Clarifies function intent and suppresses compiler warnings.

### Unused Function Returns
- **Rule:** There should be a preference to use function returns appropriately. Any unused function returns must be cast to void.
- **Rationale:** Explicitly casting unused return values to void clarifies intent, prevents accidental omission of critical results, and suppresses compiler warnings.

### Early Returns
- **Rule:** Early returns that are not guards clauses should be avoided.
- **Rationale:** Guard clauses improve readability by handling error cases early, reducing nesting and clarifying assumptions for the remaining function logic. Using early returns for general control flow can make code harder to follow and maintain.
- **Example:** 
```cpp

```

### `goto` statements
- **Rule:** `goto` statements must not be used.
- **Rationale:** `goto` statements make control flow difficult to follow and maintain. Safer, structured alternatives (such as loops and conditionals) should always be used.

### Template Function
- **Rule:** If a template function should only support a specific set of types, define its implementation in the `.cpp` file. Explicitly instantiate the template for each supported type immediately after the function definition.
- **Rationale:** Defining template functions in the `.cpp` file and explicitly instantiating them for required types prevents accidental use with unsupported types. It also makes the set of supported types clear and maintainable.
- **Note:** If the template is used with a type that is not explicitly instantiated, the code will compile but fail to link, resulting in a linker error rather than a compiler error.
```cpp
// Example.hpp
static constexpr uint8_t BUFFER_SIZE = 2U;

template<typename T>
void serialiseToBuffer(const T& value, uint8_t &buffer[BUFFER_SIZE]);

// Example.cpp
template<typename T>
void serialiseToBuffer(const T& value, uint8_t &buffer[BUFFER_SIZE]);
{
  std::memcpy(buffer, &value, sizeof(T));
}

template void serialiseToBuffer<uint8_t> (const uint8_t&,  uint8_t &buffer[BUFFER_SIZE]);
template void serialiseToBuffer<int8_t>  (const int8_t&,   uint8_t &buffer[BUFFER_SIZE]);
template void serialiseToBuffer<uint16_t>(const uint16_t&, uint8_t &buffer[BUFFER_SIZE]);
template void serialiseToBuffer<int16_t> (const int16_t&,  uint8_t &buffer[BUFFER_SIZE]);

// Usage
uint8_t buffer[BUFFER_SIZE];

uint16_t val16 {0x1234U};
float    valF  {1.23F};
uint32_t val32 {0x12345678U};

serialiseToBuffer(val16, buffer); // OK: uint16_t is explicitly instantiated
serialiseToBuffer(val32, buffer); // Linker error: uint32_t is not instantiated

```

## 5. Casting Rules

### Cast Style
- **Rule:** No C-style casts. Use C++ named casts.
- **Rationale:** C-style casts are not explicit about the type of conversion performed. They can easily introduce unsafe or undefined behavior. C++ named casts make intent clear and restrict dangerous conversions.

### Qualification Removal Casts
- **Rule:** A cast shall not remove any `const` or `volatile` qualification.
- **Rationale:** Removing these qualifiers can break immutability or safe access guarantees, leading to undefined behavior. 


## 6. Language and Compiler Restrictions

### C++ Standard Version
- **Rule:** The codebase must be compatible with C++17.
- **Rationale:** C++17 offers a strong set of modern language features with broad support, stability, and compatibility with a wide range of codebases and tools.

### ISO C++ Compliance
- **Rule:** Only ISO C++ is permitted; language extensions are not allowed.
- **Rationale:** Guarantees portability and avoids compiler-specific behavior.

### Preprocessor Macros
- **Rule:** Preprocessor macros should be avoided except for header guards or conditional compilation. All constant values must be defined using `constexpr` or `const` variables.
- **Rationale:** Macros can obscure developer intent, bypass type checking, and can make code difficult to debug.

### Function-like Macros
- **Rule:** Function-like macros must not be used. Use `inline` functions or templates instead.
- **Rationale:** Function-like macros bypass type checking and can cause unexpected behaviour. `inline` functions and templates provide type safety, better error checking, and clearer intent.

### Use of Pragmas
- **Rule:** Avoid use of `#pragma` directives.
- **Rationale:** Non-standard pragmas may not be supported by all compilers and reduce portability.

### Compiler Warning Flags
- **Rule:** The following compiler flags must be enabled:
`-Wall -Wextra -Wpedantic -Wswitch-default -Wunreachable-code -Wformat`
- **Rationale:** Enables strict compiler checks to catch errors and enforce code quality.


## 7. Scope Resolution and Ownership

### `using namespace`
- **Rule:** Do not use `using namespace` at global scope in header files (`.hpp`). Limit its use at file scope in source files (`.cpp`). Prefer explicit namespace qualification.
- **Exception:** `using namespace` may be used for a specific nested namespace inside a function or at file scope in a `.cpp` file (never in a header), to simplify access to deeply nested enums or types. This is permitted only if it does not risk name conflicts and does not bring all symbols from a broad namespace into the global scope.
- **Rationale:** Explicit namespace qualification prevents ambiguity and name clashes. The exception allows for concise code when accessing deeply nested objects, while still avoiding global namespace pollution.
- **Example:**
```cpp
namespace Atams { namespace MapExample { namespace BlockExample {

enum VarID_t : uint8_t 
{
  VAR_EXAMPLE = 0U
};

} } }; /* End Namespace - Atams::MapExample::BlockExample */

// Allowed in a .cpp source file or function:
using namespace Atams::MapExample;

uint8_t varID = BlockExample::VAR_EXAMPLE;
```

### Symbol Scope
- **Rule:** All constants, variables, functions, and types must be defined within a namespace or class. Avoid defining any symbols at the global scope.
- **Rationale:**
Defining symbols within a namespace or class prevents name collisions, improves code organization, and avoids polluting the global namespace. 


## 8. Enums

### Enum Underlying Types
- **Rule:** The underlying type of enums should be explicitly specified.
- **Rationale:** Guarantees type size, preventing compiler-dependent defaults. This reduces the risk of unexpected behaviour when serialising enums into buffers.
```cpp
//Compliant
enum class Item : uint8_t
{
    Foo = 0U,
    Bar = 1U,
    Baz = 2U
};

//Non-compliant
enum class Item
{
    Foo = 0U,
    Bar = 1U,
    Baz = 2U
};
```

### Enum Asserts
- **Rule:** Any developer enums that will be packed into buffers for transfer between systems shall have accompanying static asserts to ensure size consistency.
- **Rationale:** Provides compile-time enforcement of enum size, catching platform or developer errors that could harm interoperability.
```cpp
enum MessageType_t: uint8_t
{
  MESSAGE_UNKNOWN  = 0U,
  MESSAGE_REQUEST  = 1U,
  MESSAGE_RESPONSE = 2U,
};
static_assert(sizeof(MessageType_t) == 1U, "MessageType_t size invalid");
```

### Enum Definition
- **Rule:** Prefer class enums `(enum class)`. Use non-class enums only when class enums would require excessive casting that harms code readability (e.g., for loop indices or break conditions).
-- **Rationale:** Class enums provide strong type safety and prevent accidental implicit conversions, making code more robust and maintainable. Allowing non-class enums in specific cases avoids excessive casting and keeps the codebase readable.

### Class Enum Casting
- **Rule:** Casting to a class enum type is not permitted. If casting from an integer to an enum is required, use a non-class enum instead.
- **Rationale:** Prohibiting casts to class enums enforces type safety and ensures only valid enum values are used throughout the codebase. This removes the need for bounds checking in functions that accept class enums as arguments.
```cpp
enum class Item : uint8_t 
{ 
  FOO = 0U, 
  BAR = 1U, 
};

Item i {static_cast<Item>(1U)}; // Non-compliant: casting to class enum is prohibited

Item i {Item::BAR};             // Compliant: Explicit type usage
```

### Non-Class Enum Function Arguments
- **Rule:** Where a non-class enum is used as a function argument, the value of it must be range checked at the start of the function definition.
- **Rationale:** Non-class enums can be implicitly converted from integers, which may result in invalid values being passed to functions. Range checking at the start of the function ensures only valid enum values are used for the remainder of the function logic.
```cpp
enum CellID_t : uint8_t 
{ 
  FOO = 0U, 
  BAR = 1U, 
  NUMBER_OF_CELLS 
};

static uint8_t cellList[NUMBER_OF_CELLS];

void setCellValue(CellID_t cellID, uint8_t value)
{
  if (cellID >= NUMBER_OF_CELLS)
  {
    // Handle invalid cell ID
    return;
  }

  // Safe to use cellID as a valid enum value
  cellList[cellID] = value;
}
```

## 9. Switch Statements
- **Rule:** All switch statements must include a `default` case. If it is safe for the `default` case to perform no action, a `/* Do Nothing */` comment must be added to the body of the case. The `default` case should handle errors correctly if doing nothing is not appropriate.
- **Rationale:**  Including a default case ensures all possible values are handled, making the code robust to future changes, and clarifying developer intent for unhandled or unexpected cases.
- **Example:**
```cpp
switch (condition)
{
  case CONDITION_A:
    foo();
    break;
    
  default:
    /* Do Nothing */
    break;
}
```

- **Rule:** A case may only fall-through to the next case if it has an empty body. The case body should be commented with `/* Fall-through */`. If any code is present in the case, an explicit break statement must be used.
- **Rationale:** This avoids accidental fall-through bugs, clarifies developer intent, and improves code readability and maintainability.
- **Example:**
```cpp
switch (condition)
{
  case CONDITION_A: /* Fall-through */  // Compliant
  case CONDITION_B:
    foo();
    break;
  case CONDITION_C:                     // Non-compliant
    bar();                              
    /* Fall-through */                 
  case CONDITION_D:                     // Non-compliant
    foo();
    bar();
  default:
    handleError();
    break;
}
```

## 10. Formatting & Style

### Indentation
- **Rule:** All indentation should use 2 spaces.
- **Rationale:** Improves readability on smaller screens and ensures codebase consistency.

### Namespace Indentation
- **Rule:** Namespace contents should not be indented.
- **Rationale:** As Atams namespaces are file wide, this avoids having the majority of file contents indented.

### Switch Statements

### Struct and Array Initialiser Lists
- **Rule:** Struct and array-of-struct initialisation must be explicit and in declaration order. All values must be initialised in order, with none omitted. To clarify intent, comment each value with its corresponding member name.
- **Rationale:** C++17 (the current target standard) does not support designated initialisers without GNU extensions, so struct members must be initialised in declaration order. Omitting or misordering values can lead to subtle bugs that the compiler may not warn about. Commenting each value with its member name improves code readability, helps reviewers spot mistakes, and compensates for the lack of compiler-checked designated initialisers. Using comments in this way also avoids the high volume of warnings from `-Wpedantic` that occur if GNU designated initialisers are used in standard C++17 code. In C++20 or later, designated initialisers can be used directly and comments may be omitted.
```cpp
enum ContainerId_t : uint8_t
{
  CONTAINER_ID_0 = 0U,
  CONTAINER_ID_1 = 1U,
  NUMBER_OF_CONTAINERS
};

struct Container_t 
{
  uint8_t itemA {0U};
  uint8_t itemB {0U};
};

itemList[NUMBER_OF_CONTAINERS] = 
{
  /* [CONTAINER_ID_0] = */
  {
    /* .itemA = */ 1,
    /* .itemB = */ 2,
  }
  /* [CONTAINER_ID_1] = */
  {
    /* .itemA = */ 3,
    /* .itemB = */ 4,
  }
}
```

### Operator Alignment

```cpp
if ((shortConditionA  && shortConditionB ) ||
    (longerConditionC && longerConditionD) )
{
  foo();
}

(((datagramHeader.command << DATAGRAM_HEADER_SHIFT_COMMAND  ) & DATAGRAM_HEADER_MASK_COMMAND  ) |
 ((datagramHeader.varID   >> DATAGRAM_HEADER_SHIFT_VAR_ID_HI) & DATAGRAM_HEADER_MASK_VAR_ID_HI) )
```

### `return` statements
- **Rule:** The value of a `return` statement must be bracketed.  

- **Example:**
```cpp
return result;   // Non-compliant

return (result); // Compliant
```

**Rule:** Opening and closing curly braces should be placed on new lines. Curly braces must be used if the body of a statement does not sit on the same line as the condition. Curly braces are not required for single line statements.
**Exception:** Single line if statements.  
**Example:**
```cpp
if (condition) foo(); /* Compliant */

if (condition)        /* Compliant */
{                     
  foo();             
}

if (condition)        /* Non-compliant */
  foo();

if (condition) {      /* Non-compliant */
  foo();              
}
```

**Rule:** Else and else if statements should be placed on new lines. 

- **Example:**
```cpp
if (conditionA)
{
  foo(); 
}
else if (conditionB) /* Compliant */
{
  bar();
} else               /* Non-compliant */
{
  baz();
}
```


## 11. Comments

### Commenting Out Code

### Descriptive Comments


## 12. Naming and Name Format Conventions
Minimal prefix/suffix definitions keep codebase cleaner. Prefixes for member and static variables alert programmers to potential out-of-function-scope effects. Multi-file scope global variables and public class variables are to be avoided entirely in the Atams codebase (excluding constant memoryMap globals).

### Naming
- **Rule:** Naming should provide context. Single character and overly shorted names should not be used.
- **Rationale:** Descriptive names make code easier to read, understand, and maintain.
- **Example:**
```cpp
uint8_t itemCount {0U}; // Compliant: Explicit naming
uint8_t itmCnt    {0U}; // Non-compliant: Excessive abbreviation
uint8_t c         {0U}; // Non-compliant: Single character name with no context

uint8_t getItemCount(void); // Compliant: Function name indicates action/context
uint8_t itemCount(void);    // Non-compliant: Function name does not indicate action
uint8_t cnt(void);          // Non-compliant: Excessive abbreviation
```

### Variables Name Format
camelCase

### Class Name Format
ThisCase

### File Name Format
ThisCase

### Constexpr Name Format
THIS_CASE

### Class Enum Name Format
This

### Non-class Enum Name Format
This_t

## 13. Error Handling

### Explicit Status & Result Objects

### Exceptions
- **Rule:** No exceptions.  
- **Pros:** Deterministic execution, no hidden control flow.  
- **Cons:** Must use error codes.  
- **Rationale:** Embedded systems often can’t afford exceptions.


## 14. File Format & Structure

### Single Namespace per File
- **Rule:** Only one namespace (or nested namespace) shall be used per pair of files (`.hpp` and `.cpp`). All code in a file must be contained within this namespace.
- **Rationale:** Limiting each file to a single namespace improves code organisation, prevents symbol conflicts, and makes code easier to maintain.

### Header Include Guards
- **Rule:** All header files must use include guards.
- **Rationale:** Prevents multiple inclusion and redefinition errors during compilation.

```cpp
#ifndef MY_HEADER_HPP
#define MY_HEADER_HPP

// Header file contents go here

#endif // MY_HEADER_HPP
```

### Include Order
- **Rule:** Files must be included in groups and in the following group order: Associated header file (in .cpp file) -> Library headers -> Project headers.
- **Rationale:** TODO
- **Example:** 
```cpp
// Example.cpp

#include "Example.hpp"

#include <stdint.h>
#include <string.h>

#include "OtherProjectFile.hpp"
```

## Init Orders
- **Rule:** Avoid global/static init order reliance.  

### Class Files
- **Rule:** Classes that are large or intended to be reused between different modules must be defined in their own `.hpp` and `.cpp` file pair.
- **Rationale:** Placing large or reusable classes in dedicated files improves code organisation, makes reuse easier, and reduces coupling between modules. It also simplifies maintenance and testing.

