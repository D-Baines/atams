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
float   floatVar {1.0F};
int32_t intVar   {0};

std::memcpy(&floatVar, &intVar, sizeof(intVar)); // Compliant
```

- **Non-compliant `reinterpret_cast` Example:**
```cpp
float   floatVar {1.0F};
int32_t intVar   {*reinterpret_cast<std::int32_t*>(&floatVar)}; // Non-compliant
```

- **Non-compliant `union` Example:**
```cpp
union UnionType_t
{ 
  float   floatField; 
  int32_t intField; 
};

UnionTyoe_t unionVar;

unionVar.floatField = 1.0F;

int32_t intVar {unionVar.intField}; // Non-compliant
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
uint8_t numbers[BUFFER_SIZE] = {0U, 0U, 0U}; 
```

### Serialising and Deserialising 
- **Rule:** All byte arrays shared with external systems must be serialised and deserialised explicitly using bit-shifting and masking operations. Do not copy structs or objects directly into buffers.
- **Rationale:** Explicit bit manipulation ensures deterministic layout and handles differences in system endianness automatically. Compiler-dependent padding, alignment, or member ordering from alternative methods can break portability.
- **Example:**
```cpp
constexpr uint8_t  CONTAINER_SIZE   {3U};
constexpr uint8_t  ITEM_A_INDEX     {0U};
constexpr uint8_t  ITEMB_LO_INDEX   {1U};
constexpr uint8_t  ITEMB_HI_INDEX   {2U};
constexpr uint16_t ITEMB_BYTE_MASK  {0xFFU};
constexpr uint8_t  ITEMB_BYTE_SHIFT {8U};

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
std::memcpy(buffer, &container, sizeof(Container_t));

// Non-compliant: Reinterpreting buffer as struct
Container_t* ptr = reinterpret_cast<Container_t*>(buffer); 
```

## 2. Constants, Variables, and Casting

### Reinterpret Casting
- **Rule:** Reinterpret casts shall only be used to cast to type `uint8_t*`.
- **Rationale:** In the current target standard (C++17), reinterpret casting to and from `uint8_t*` is one of the few cases that results in defined and portable behavior. Restricting reinterpret casts to `uint8_t*` prevents unsafe type punning and undefined behavior that can occur when casting between unrelated types.
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

### Object Initialisation
- **Rule:** Always intitialise an object.
- **Rationale:** Avoids used-before-set errors and their associated undefined behavior.
- **Example:** 
```cpp
// Non-compliant
uint8_t testVar;

// Compliant
uint8_t testVar {0U};
```

### Scalar and POD Struct Type Initialisation Method
- **Rule:** Always intitialise scalar types and POD structs using brace-initialisation `{}`.  
- **Rationale:** Ensures compile-time checks catch accidental narrowing conversions.
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

### Class Object Initialisation Method
- **Rule:** Always initialise class objects using parentheses initialisation `()`.
- **Rationale:** Using parentheses for class objects clearly indicates constructor calls and distinguishes object construction from simple value initialisation of scalars and PODs, improving code clarity and intent.
- **Example:**
```cpp
class MyClass
{
  public:

  MyClass(uint8_t a, uint8_t b) : x(a), y(b) {}

  uint8_t x;
  uint8_t y;
};

MyClass obj(1, 2);   // Compliant: class object initialised with constructor
MyClass obj{1, 2};   // Not preferred for class objects
```

### Namespace Constants
- **Rule:** Use `constexpr` for all namespace values that are known at compile time.
- **Rationale:** `constexpr` ensures compile-time evaluation and, at namespace scope, is implicitly `inline` in C++17 and later.

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
- **Rule:** No magic numbers. Literals with values other than `0` or `1` must be defined as named constants.
- **Rationale:** Named constants improve code readability, provide context, and make maintenance safer and easier.

### Pointer Indirection
- **Rule:** Pointer variables should not be declared with more than two levels of indirection.
- **Rationale:** Excessive indirection increases complexity, harms code clarity, and increases the risk of invalid memory access.
```cpp
uint8_t value {0U};

uint8_t   *ptr1 {&value}; // Compliant: one level of indirection
uint8_t  **ptr2 {&ptr1};  // Compliant: two levels of indirection
uint8_t ***ptr3 {&ptr2};  // Non-compliant: more than two levels of indirection
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
  if (inputPtr == nullptr)
  {
    return; // Nullptr guard clause
  }
  
  // Compliant: Both methods of dereferencing safe after nullptr check
  Data_t  localCopy {*inputPtr};   // Direct dereference
  uint8_t value     {inputPtr->x}; // Member access via pointer
}
```

### Nullptr
- **Rule:** Always use `nullptr` to represent null pointers. Do not use `NULL` or `0` for pointer initialisation or comparison. 
- **Rationale:** `nullptr` is the standard C++ keyword for representing a null pointer. It provides type safety and avoids the ambiguities and potential errors associated with `NULL`, which is typically defined as `0` or `((void*)0)`.


## 3. Structs, Classes, and Object Design

### POD Objects 
- **Rule:** POD objects should be defined using structs and should be named with an `_t` suffix. Operator overloads can be defined for POD structs (see Rule xxx).
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
```

### Non-POD Structs 
- **Rule:** Non-POD structs may be defined, but their member variables and functions must be public. If more complex behaviour or private member variables and functions are required, classes should be used. Non-POD structs must exclude the `_t` suffix.
- **Rationale:** This results in a simple struct type that acts as an extension of POD structs. The use of simple public functions operating on public variables can minimise repetition of code blocks. 

### Special Member Functions
- **Rule:** All classes must declare the four special member functions (constructor, destructor, copy constructor, move constructor). Un-defined special member functions should be declared as default or deleted.
- **Rationale:** Prevents generation of compiler-defined functions and limits unexpected object behaviours.

### Class Assignment Operators
- **Rule:** All classes must declare copy assignment, and move assignment operators. Undefined operators should be declared as default or deleted.
- **Rationale:** Prevents generation of compiler-defined functions and limits unexpected object behaviours.

### Public Class Variables
- **Rule:** Non-constexpr public member variables shall not be used. Setters and getters of private variables should be defined instead for more tightly specified object interactions.
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
- **Rule:** Limit virtual overrides to one level. Mark overriding virtual functions with `final` to prevent further accidental overriding.
- **Rationale:** Deep override chains can create hard-to-follow call paths and increase maintenance complexity.
- **Examle:**
```cpp
virtual void functionToOverride override final;
```

### Base Class Virtual Functions
- **Rule:** Declare base class virtual functions as pure if derived classes must override them. If overriding is optional, provide a default implementation and omit `= 0`.
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
- **Rationale:** Greatly decreases the risk of invalid memory access.
- **Example:**
```cpp
// Compliant
void doTask(uint8_t * taskPtr)
{
  if (ptr == nullptr) return; /* Early Return */

  TaskType_t taskType = taskPtr.getTaskType;

  switch (taskType)
  {
    case TASK_1:
      /* Do Something */
      break;
    default:
      /* Do Nothing */
      break;
  }
}
```

### Unused Function Arguments
- **Rule:** A function definition must not contain any unused arguments.
- **Rationale:** Keeps code clean, makes function purpose clear, and suppresses compiler warnings.
- **Exception:** Virtual function arguments.

### Unused Virtual Function Arguments
- **Rule:** If function arguments are not used in the definition of a virtual function, they must be cast to void.
- **Rationale:** Clarifies function intent and suppresses compiler warnings.

### Unused Function Returns
- **Rule:** Function returns must be used and handled appropriattely, or must be cast to void.
- **Rationale:** Explicitly casting unused return values to void clarifies intent, prevents accidental omission of critical results, and suppresses compiler warnings.

### Early Returns
- **Rule:** Early returns that are not guards clauses should be avoided.
- **Rationale:** Guard clauses improve readability by handling error cases early, reducing nesting and clarifying assumptions for the remaining function logic. Using early returns for general control flow can make code harder to follow and maintain.
- **Example:** 
```cpp
// Compliant - Return used as guard clause
void doTask(uint8_t * taskPtr)
{
  if (ptr == nullptr) return; /* Early Return */

  TaskType_t taskType = taskPtr.getTaskType;

  switch (taskType)
  {
    case TASK_1:
      /* Do Something */
      break;
    default:
      /* Do Nothing */
      break;
  }
}

// Non-compliant - Unnecessary nesting
void doTask(uint8_t * taskPtr)
{
  if (ptr != nullptr)
  {
    TaskType_t taskType = taskPtr.getTaskType;
  
    switch (taskType)
    {
      case TASK_1:
        /* Do Something */
        break;
      default:
        /* Do Nothing */
        break;
    }
  }
}

// Non-compliant - Unnecessary use of returns for control flow
bool getValueValid(const uint8_t value)
{
  if (value == 0U)
  {
    return (false);
  }
  else if (value == 1U)
  {
    return (true);
  }
  else
  {
    return (false);
  }
}
```

### `goto` statements
- **Rule:** `goto` statements must not be used.
- **Rationale:** `goto` statements make control flow difficult to follow and maintain. Safer, structured alternatives (such as loops and conditionals) should always be used.

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

### Global and Static Initialisation Order
- **Rule:** Avoid global/static init order reliance.
- **Rationale:** The order in which global and static variables are initialized across different translation units is not guaranteed by the C++ standard. Relying on this order can lead to unpredictable behavior.


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

uint8_t varID {BlockExample::VAR_EXAMPLE};
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
- **Rule:** Any enums that will be packed into buffers for transfer between systems shall have accompanying static asserts to ensure size consistency.
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
- **Rule:** Prefer class enums `(enum class)`. Use non-class enums only when class enums would require casting to integers (e.g., for loop indices and break conditions).
- **Rationale:** Class enums provide strong type safety and prevent accidental implicit conversions, making code more robust and maintainable. Allowing non-class enums in specific cases avoids excessive casting and keeps the codebase readable.

### Class Enum Casting
- **Rule:** Casting to a class enum type is not permitted. If casting from an integer to an enum is required, use a non-class enum instead.
- **Rationale:** Prohibiting casts to class enums enforces type safety and ensures only valid enum values are used throughout the codebase. This removes the need for bounds checking in functions that accept class enums as arguments.
```cpp
enum class Item : uint8_t 
{ 
  FOO = 0U, 
  BAR = 1U, 
};

Item newItem {static_cast<Item>(1U)}; // Non-compliant: casting to class enum is prohibited

Item newItem {Item::BAR};             // Compliant: Explicit type usage
```

### Non-Class Enum Function Arguments
- **Rule:** Where a non-class enum is used as a function argument, the value of it must be range checked at the start of the function definition.
- **Rationale:** Non-class enums can be implicitly converted from integers, which may result in invalid values being passed to functions. Range checking at the start of the function ensures only valid enum values are used for the remainder of the function logic.
```cpp
enum ItemID_t : uint8_t 
{ 
  ITEM_A = 0U, 
  ITEM_B = 1U, 
  NUMBER_OF_ITEMS
};

static ItemType_t itemList[NUMBER_OF_ITEMS];

void setCellValue(ItemID_t itemID, ItemType_t value)
{
  if (itemID >= NUMBER_OF_ITEMS)
  {
    // Handle invalid itemID
    return;
  }

  // Safe to use itemID as a valid enum value
  itemList[itemID] = value;
}
```

## 9. Formatting & Style

### Indentation
- **Rule:** All indentation should use 2 spaces.
- **Rationale:** Improves readability on smaller screens and ensures codebase consistency.

### Namespace Indentation
- **Rule:** Namespace contents should not be indented.
- **Rationale:** Atams namespaces are often declared file wide, the rule therefore avoids having the majority of file contents indented.

### Switch Statement Default Case
- **Rule:** All switch statements must include a `default` case. If it is safe for the `default` case to perform no action, a `/* Do Nothing */` comment must be added to the body of the case. The `default` case must handle error cases correctly if doing nothing is not appropriate.
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

### Switch Statement Fall-through
- **Rule:** A case may only fall-through to the next case if it has an empty body. The case body should be commented with `/* Fall-through */`. If any code is present in the case, an explicit break statement must be used.
- **Rationale:** This avoids accidental fall-through bugs, clarifies developer intent, and improves code readability and maintainability.
- **Example:**
```cpp
switch (condition)
{
  case CONDITION_A: /* Fall-through */  // Compliant - fall-through with empty case and comment
  case CONDITION_B:
    foo();
    break;
  case CONDITION_C:                     // Non-compliant - fall-through without comment
  case CONDITION_D:
    bar();
    break;
  case CONDITION_E:                     // Non-compliant - fall-through with case contents
    foo();
    bar();                              
    /* Fall-through */                 
  default:
    handleError();
    break;
}
```

### Struct and Array Initialiser Lists
- **Rule:** Struct and array-of-struct initialisation must be explicit and in declaration order. All values must be initialised in order, with none omitted. To clarify intent, comment each value with its corresponding member name.
- **Rationale:** Tthe current target standard (C++17) does not support designated initialisers without GNU extensions, so struct members must be initialised in declaration order. Omitting or misordering values can lead to subtle bugs that the compiler may not warn about. Commenting each value with its member name improves code readability, helps reviewers spot mistakes, and compensates for the lack of compiler-checked designated initialisers. Using comments in this way also avoids the high volume of warnings from `-Wpedantic` that occur if GNU designated initialisers are used in standard C++17 code. If the codebase moves to C++20 or later in future, designated initialisers can be used directly and comments can be omitted.
- **Example:**
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

### Logical Operator Alignment
- **Rule:** In multi-clause conditional statements (such as `if`, `while`), each logical clause must start on a new line and be vertically aligned with other clauses at the same logic level. The outer logical operators (`&&`, `||`) joining bracketed clauses must appear at the end of each line. All nested operations must be enclosed in parentheses, and the closing parenthesis for the outer condition must align vertically with the outer logical operator.
- **Rationale:** This formatting makes the logical structure of complex conditions immediately clear, helping reviewers quickly verify correct grouping and intent.
- **Example:**
```cpp
// Compliant:
if ((condA && condB) ||
    (condC && condD) )
{
  foo();
}

// Non-compliant:
if (condA && condB || condC && condD)
{
  foo();
}

// Compliant:
if (((condA && condB) ||
     (condC && condD) ) &&
    ((condE && condF) ||
     (condG && condH) ) ) 
{
  foo();
}

// Compliant:
if ((shortA      && shortB      ) ||
    (muchLongerC && muchLongerD ) )
{
  foo();
}

// Non-compliant:
if ((shortA && shortB) ||
    (muchLongerC && muchLongerD))
{
  foo();
}
```

### Binary Operator Alignment
- **Rule:** In complex binary operations (such as bit-shifting, masking, and bitwise logic), each major operand or clause must be vertically aligned with others at the same logic level. Operators (`<<`, `>>`, `&`, `|`, etc.) should be placed consistently within each clause, and nested operations must be enclosed in parentheses. Alignment of operands and operators is required for readability; newlines are optional but recommended for clarity in multi-clause expressions.
- **Rationale:** Consistent alignment of operands and operators in complex binary expressions makes the structure and intent immediately clear, reduces the risk of logic errors, and simplifies code review. Parentheses clarify precedence, and vertical alignment helps reviewers verify correct grouping and logic at a glance.
- **Example:**
```cpp
// Compliant:
uint32_t datagramHeader {(((header.command << HEADER_SHIFT_COMMAND) & HEADER_MASK_COMMAND) |
                          ((header.varID   >> HEADER_SHIFT_VAR_ID ) & HEADER_MASK_VAR_ID ) )}

// Non-compliant:
uint32_t datagramHeader {(((header.command << HEADER_SHIFT_COMMAND) & HEADER_MASK_COMMAND) |
                          ((header.varID >> HEADER_SHIFT_VAR_ID) & HEADER_MASK_VAR_ID))}
```

### `return` statements
- **Rule:** The value of a `return` statement must be bracketed.  

- **Example:**
```cpp
return result;   // Non-compliant

return (result); // Compliant
```

- **Rule:** Opening and closing curly braces should be placed on new lines. Curly braces must be used if the body of a statement does not sit on the same line as the condition. Curly braces are not required for single line statements.
- **Rationale:** Codebase styling consistency and clarity.
- **Exception:** Single line if statements.  
- **Example:**
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

- **Rule:** Else and else if statements should be placed on new lines. 
- **Rationale:** Codebase styling consistency and clarity.
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

## 10. Naming Conventions

### Naming
- **Rule:** Naming should provide context. Single character and overly shortened names should not be used.
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

### Naming Conventions Summary

| Artifact Type | Convention | Example | Rationale |
|---------------|------------|---------|-----------|
| **Local Variables <br> (Incl. Const Member Variables)** | camelCase  | `itemCount`       | Improves readability and distinguishes from constants and types. |
| **Constexpr Variables**             | SCREAMING_SNAKE_CASE      | `MAX_BUFFER_SIZE` | Makes constants easily identifiable and visually distinct. |
| **Private Class Member Variables**  | camelCase + trailing `_`  | `value_`          | Trailing underscore signals instance-wide effect, clarifying scope within class methods. |
| **Static Variables (All scopes)**   | `s_` prefix               | `s_counter`       | `s_` prefix highlights static storage duration and wider scope effect beyond function/class instance. |
| **POD Struct Types**                | PascalCase + `_t` suffix  | `DataPacket_t`    | `_t` suffix distinguishes POD types, improving clarity and consistency. |
| **Class/Non-POD Struct Types**      | PascalCase                | `MyClass`         | Standard C++ style for types; distinguishes from POD structs and variables. |
| **Enum Class Types**                | PascalCase                | `UpdateState`     | Keeps type names visually consistent with class type definitions and improves readability with `::` access syntax.  |
| **Non-Class Enum Types**            | PascalCase + `_t` suffix  | `Error_t`         | `_t` suffix signals non-class enum, indicating where bounds checking is required.|
| **Files**                           | PascalCase                | `MyFile.cpp`      | Codebase consistency. |
| **Namespaces**                      | PascalCase                | `Atams`           | Codebase consistency. |


#### Examples

```cpp
// Local variable
uint8_t itemCount {0U};

// Constexpr variable
constexpr uint8_t MAX_BUFFER_SIZE {16U};

// Static variable
static uint8_t s_counter {0U};

// Class type + private class member variable
class Example
{
  private:
  uint8_t value_;
};

// POD struct type
struct DataPacket_t
{
  uint8_t  id;
  uint16_t value;
};

// Enum class type
enum class State : uint8_t
{
  INIT = 0U,
  RUN  = 1U
};

// Non-class enum type
enum Error_t : uint8_t
{
  ERROR_NONE  = 0U,
  ERROR_VALUE = 1U
};

// File name: MyFile.cpp

// Namespace
namespace Atams
{
  // ...
}
```

## 11. Error Handling

### Exceptions
- **Rule:** Do not use C++ exceptions. All error conditions must be handled explicitly using return codes, error objects, or guard clauses.
- **Rationale:** Exceptions can disrupt control flow, making it unclear where execution will continue after an error. Catching exceptions only at high levels may leave the program in an unpredictable state. Return codes and guard clauses keep error handling straightforward and maintainable.

## 12. File Format & Structure

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
- **Rationale:** Consistent include order across the codebase improves readability and maintainability. 
- **Example:** 
```cpp
// In Example.cpp

#include "Example.hpp"

#include <stdint.h>
#include <string.h>

#include "OtherProjectFile.hpp"
```

### Class Files
- **Rule:** Classes that are large or intended to be reused between different modules must be defined in their own `.hpp` and `.cpp` file pair.
- **Rationale:** Placing large or reusable classes in dedicated files improves code organisation, makes reuse easier, and reduces coupling between modules. It also simplifies maintenance and testing.

