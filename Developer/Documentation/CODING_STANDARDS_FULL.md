# Atams C++ Coding Standards

## Introduction

The **Atams Coding Standards** define the rules for writing safe, portable, and maintainable embedded C++ code used in the Atams library.

Atams targets constrained embedded systems and therefore emphasizes:
- **No heap allocation**: All memory must be statically allocated for predictability and reliability.  
- **Deterministic behavior**: No exceptions, or undefined control flows.  
- **Portability**: Standards-compliant ISO C++ only, with no compiler extensions.  

This document balances style guidelines (to keep the codebase consistent) and safety rules (to prevent errors). Each rule below is a short guideline; brief rationale, exceptions, and examples are included where appropriate.

---

## 1. Memory Management

### Dynamic Memory Allocation: 
- **Rule:** No dynamic memory allocation (`new`, `malloc`, etc.), including `std` library functions that use dynamic allocation (e.g. `vector`, `string`, `map`, etc.) 
- **Rationale:** Reduces risks associated with memory fragmentation and memory leaks, and improves code predictability.

### Reinterpret Casting
- **Rule:** Reinterpret casts shall only be used to cast to type `uint8_t*`.
- **Rationale:** Avoid type punning between unrelated types (`float*` -> `uint32_t*`).
- **Exception:** Casting hardware-defined addresses to pointers to the hardware’s specified type.

### Type Punning
- **Rule:** All type punning shall be performed using std::memcpy. The use of unions or reinterpret_cast for type punning is prohibited.
- **Rationale:** The C++ standard only guarantees defined behavior when accessing an object’s representation through std::memcpy (C++17). Type punning through unions or reinterpret_cast is undefined behavior and may result in non-portable or incorrect code.
- **Complient Example:**
```cpp
float    f = 1.0f;
uint32_t i;

std::memcpy(&i, &f, sizeof(i));
```

- **Non-complient `reinterpret_cast` Example:**
```cpp
float    f = 1.0f;
uint32_t i = *reinterpret_cast<std::uint32_t*>(&f);
```

- **Non-complient `union` Example:**
```cpp
union U_t
{ 
  float    f; 
  uint32_t i; 
};

U_t u;
u.f = 1.0f;
uint32_t i = u.i;
```

## 2. Variable Types, Representation, and Initialisation

### Integer Types
- **Rule:** Use `<cstdint>` for integer types.  
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
- **Rule:** Use the `U` suffix for all unsigned integer literals up to `uint32_t`, including `uint8_t` and `uint16_t`. The use of unsigned types larger than `uint32_t` is to be discouraged.
- **Rationale:** Using the `U` suffix ensures that unsigned literals are explicit and consistent. The `U` suffix yields an `unsigned int`, which safely converts to `uint8_t`, `uint16_t`, and `uint32_t` on 32 bit or larger targets. Narrowing conversions to smaller widths are intentional and safe when using brace-initialization. `int64_t`, `uint64_t`, and `double` are not compatible with Atams communications and are currently avoided in developer code. Suffixes for these types may be included in future guidelines.

### Float Literals
- **Rule:** All float literals must include the decimal and use the `F` suffix (e.g., `1.0F`).  
- **Rationale:** Ensures the literal is explicitly a float and prevents accidental interpretation as an integer or implicit conversion from double.

### Variable Initialisation
- **Rule:** Always intitialise variables using brace-initialisation `{}`.  
- **Rationale:** Ensures compile-time checks catch accidental narrowing conversions, wrap-around, and overflow conditions.
- **Example"**
```cpp
uint8_t testVar1 {300U};      // ! narrowing conversion from unsigned int to uint8_t
uint8_t testVar2 {-1};        // ! narrowing conversion from int to uint8_t
int8_t  testVar3 {128};       // ! narrowing conversion from int to int8_t
uint8_t testVar4 {255U + 1U}; // ! narrowing conversion from unsigned int to uint8_t
uint8_t testVar4 {0.1F};      // ! narrowing conversion from float to uint8_t
```

### Enum Underlying Types
- **Rule:** The underlying type of enums should be explicitly specified.
- **Rationale:** Guarantees deterministic type size and representation, preventing compiler-dependent defaults. This reduces the risk of unexpected behaviour when serialising enums into buffers.

### Developer Defined Types
- **Rule:** Any developer defined types (structs, enums etc.) that will be packed into buffers for transfer between systems shall have accompanying static asserts to ensure size consistency.
- **Rationale:** Ensures consistency of layout and size across compilers and platforms, preventing subtle interoperability bugs.

## 3. Structs, Classes, and Object Design

### POD Objects 
- **Rule:** POD objects should be defined using structs. Classes should be used for anything more complex.
- **Rational:** Codebase simplicity and consistency.

### Special Member Functions
- **Rule:** All non-POD classes must declare the four special member functions (constructor, destructor, copy constructor, move constructor). Un-defined special member functions should be declared as default or deleted.
- **Rationale:** Prevents generation of compiler-defined functions and limits unexpected object behaviours.

### Class Assignment Operators
- **Rule:** All non-POD classes must declare copy assignment, and move assignment operators. Undefined operators should be declared as default or deleted.
- **Rationale:** Prevents generation of compiler-defined functions and limits unexpected object behaviours.

### Composition vs Inheritance
- **Rule:** Prefer composition over inheritance. 
- **Rationale:** Complex inheritance can make a codebase harder to follow.

### Public Class Variables
- **Rule:** Non-constant public member variables shall not be used.
- **Rationale:** Setters and getters of private variables can be defined for more tightly specified object interactions. Concurrency locks can be encapsulated in the member functions to avoid race conditions where appropriate.

### Class Member Constants
- **Rule:** Class member constants should be defined using `static inline constexpr`.
- **Rationale:**  `constexpr` ensures the value is a compile-time constant, `static` avoids unnecessary per-object storage, and `inline` allows the constant to be defined in a header file without violating the One Definition Rule.

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
  void mustOverride(void) override final;
};

// Derived.cpp
void Derived::mustOverride(void)
{
  /* Pure virtual override definition */
}
```

---
---
---
---
---
---
---
---
---
---
---

### Constants
- **Rule:** Use `constexpr` or `inline constexpr` for constant definitions. Only class constants require `static`.
- **Pros:** Compile-time evaluation, no overhead.  
- **Cons:** Not always obvious when to use.  
- **Rationale:** Forces efficient, deterministic code.

### Buffer Parsing
- **Rule:** All buffers shared through external communication must be serialised and deserialised explicitly using bit-shifting and masking operations.
- **Rationale:** Explicit bit manipulation ensures deterministic layout and handles differences in system endianness automatically.
> **Note:** Do not memcpy structs or objects directly into buffers. Compiler-dependent padding, alignment, or member ordering can break portability and produce incorrect results. Always serialise each field individually in a defined order.


### Magic numbers
- **Rule:** No magic numbers (except 0, 1, 2).  
- **Pros:** Improves maintainability.  
- **Cons:** More constants to define.  
- **Rationale:** Improves clarity.  

### Exceptions
- **Rule:** No exceptions.  
- **Pros:** Deterministic execution, no hidden control flow.  
- **Cons:** Must use error codes.  
- **Rationale:** Embedded systems often can’t afford exceptions.

### Null
- **Rule:** Use `nullptr` only.  
- **Pros:** Type safe.  
- **Cons:** None.  
- **Rationale:** Avoids ambiguity.

**Rule 1.0:** 

## 2. Style

**Rule 1.0:** Use 2 spaces for indentation.  

**Rule 1.1:** If statement opening and closing braces must be placed on new lines.  
**Exception:** Single line if statements.  
**Example:**
```cpp
if (condition) foo(); /* Compliant */

if (condition)
{                     /* Compliant */
  foo();             
}

if (condition) {      /* Non-compliant */
  foo();              
}
```

**Rule 1.2:** Else and else if statements should be placed on new lines. 

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


### Switch statements THIS IS NOT STYLE ONLY
**Rule:** Always provide a `default` case. If the default case should do nothing, the body must be commented with: `/* Do Nothing */`. Often times, the default case should contain appropriate error handling.
- **Rationale:** Explicit handling forces programmer to consider all cases.
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

**Rule:** Cases can fall-through only if their body is empty. If the case does fall through, the body must be commented with: `/* Fall-through */`.  
- **Rationale:** Avoids accidental fall-through bugs.

```cpp
switch (condition)
{
  case CONDITION_A: /* Fall-through */
  case CONDITION_B:
    foo();
    break;
  case CONDITION_C:
    bar();           /* !! Non-complient !! */
  default:
    handleError();
    break;
}

switch (condition)
{
  case CONDITION_A: /* Fall-through */ 
  case CONDITION_B: /* Fall-through */
  case CONDITION_A:
    foo();
    break;
  default:
    /* Do Nothing */
    break;
}
```

### `return` statements
- **Rule:** The value of a `return` statement must be bracketed `()`.  

- **Example:**
```cpp
return (result);
```

### Namespaces
- **Rule:** Do not indent namespace contents.  


### Formatting examples
- **Rule:** Provide examples for switch, initializer lists, operators.  
- **Pros:** Prevents style drift; consistent look.  
- **Cons:** Requires updates if style evolves.  
- **Rationale:** Examples provide a living style reference.

```cpp
listExample[4U] = 
{
  /* [0U] = */
  {
    /* .itemA = */ 2,
    /* .itemB = */ 3,
  }
  /* [1U] = */
  {
    /* .itemA = */ 2,
    /* .itemB = */ 3,
  }
}
```

```cpp
if ((shortConditionA  && shortConditionB ) ||
    (longerConditionC && longerConditionD) )
{
  foo();
}
```

---

## 2. Naming

### Variables: 
- **Case:** `camelCase`.  
- **Static Variable Prefix:** `s_`.  
- **Class Member Variable Suffix:**  `_`.  
- **POD Struct Member Variables:** No prefix/suffix.
- **Rationale:** Minimal prefix/suffix definitions keep codebase cleaner. Prefixes for member and static variables alert programmers to potential out-of-function-scope effects. Global variables and public class variables are to be avoided entirely in the Atams codebase (const memoryMap exluded).

### Classes: 
- **Case:** `PascalCase`. 
 
### Filenames: 
- **Case:** `PascalCase`.   
- **Extensions:** `.hpp` for header files. `.cpp` for source files.

### Constants:
- **Case:** `SCREAMING_SNAKE_CASE`. 

### Enums
- **Case:** `PascalCase`.
- **Non-class Enum Suffix:** `_t`;  
**Rule:** Class enums preferred. Non-class enums should be used in cases where heavy casting would muddy the codebase i.e. when used as the initialisation and break check values of an index based for loop.
---

## 3. Literals, Constants & Initialization

### Literal suffixes
- **Rule:** Use `U` for unsigned integers, `F` for floats, all uppercase.   
- **Rationale:** Ensures explicit types, improves clarity. int64_t, uint64_t, and double are not compatible with Atams communications and are currently avoided in developer code.

### Floats
- **Rule:** All float literals must include decimal (e.g., `1.0F`).  
- **Rationale:** Avoids mistaken integer literals.

### Constants
- **Rule:** Use `constexpr` or `inline constexpr` for constant definitions. Only class constants require `static`.
- **Pros:** Compile-time evaluation, no overhead.  
- **Cons:** Not always obvious when to use.  
- **Rationale:** Forces efficient, deterministic code.

### Initialization
- **Rule:** Use brace-initialization `{}` where possible.  
- **Pros:** Avoids narrowing conversions; uniform.  
- **Cons:** Verbose in some cases.  
- **Rationale:** Safer initialization.

---

## 4. Memory & Safety

### Heap allocation
- **Rule:** No dynamic allocation (`new`, `malloc`, etc.).  
- **Pros:** Deterministic memory usage.  
- **Cons:** No growable containers.  
- **Rationale:** Critical for embedded systems.

### Integer types
- **Rule:** Use `<stdint.h>` integer types.  
- **Pros:** Portable, fixed-size.  
- **Cons:** Slightly verbose.  
- **Rationale:** Prevents platform-size mismatches.

### Floats
- **Rule:** Ensure `float` is 4B and IEC 60559.  
- **Pros:** Guarantees portability.  
- **Cons:** Fails on exotic platforms.  
- **Rationale:** Avoids undefined behavior.

### Magic numbers
- **Rule:** No magic numbers (except 0,1,2).  
- **Pros:** Improves maintainability.  
- **Cons:** More constants to define.  
- **Rationale:** Improves clarity.

### Exceptions
- **Rule:** No exceptions.  
- **Pros:** Deterministic execution, no hidden control flow.  
- **Cons:** Must use error codes.  
- **Rationale:** Embedded systems often can’t afford exceptions.

### RTTI
- **Rule:** Avoid RTTI.  
- **Pros:** Smaller binary, faster.  
- **Cons:** Loses runtime type checks.  
- **Rationale:** Keep binary small, deterministic.

### Null
- **Rule:** Use `nullptr` only.  
- **Pros:** Type safe.  
- **Cons:** None.  
- **Rationale:** Avoids ambiguity.

---

## 5. Classes & Structs

### Struct vs class
- **Rule:** Structs = POD only; Classes otherwise.  
- **Pros:** Clear intent.  
- **Cons:** May conflict with existing habits.  
- **Rationale:** Semantic distinction.

### Special members
- **Rule:** Declare all 4 special functions in non-POD. Default/delete explicitly.  
- **Pros:** Prevents accidental compiler behavior.  
- **Cons:** Verbose.  
- **Rationale:** Explicit > implicit.

### Assignment
- **Rule:** Declare assignment ops in non-POD.  
- **Pros:** Explicit ownership handling.  
- **Cons:** Boilerplate.  
- **Rationale:** Prevents surprises.

### Composition/inheritance
- **Rule:** Prefer composition. Inheritance must be public.  
- **Pros:** Safer design.  
- **Cons:** Sometimes verbose.  
- **Rationale:** Avoids misuse of inheritance.

### Operators
- **Rule:** Overloads must be obvious & complete.  
- **Pros:** Prevents misleading code.  
- **Cons:** Restrictive.  
- **Rationale:** Avoid subtle bugs.

### Virtuals
- **Rule:** Use `virtual` keyword in base. Overridden once max, marked `final`. Pure if required.  
- **Pros:** Clear, controlled polymorphism.  
- **Cons:** Restrictive.  
- **Rationale:** Prevents diamond/multiple inheritance bugs.

---

## 6. Functions & Parameters

### Overloading
- **Rule:** No function overloading.  
- **Pros:** No ambiguity.  
- **Cons:** Less expressive.  
- **Rationale:** Avoids confusion.

### Defaults
- **Rule:** No default arguments.  
- **Pros:** Explicit call signatures.  
- **Cons:** Verbose.  
- **Rationale:** Avoids surprises.

### Auto
- **Rule:** No `auto`.  
- **Pros:** Explicit types.  
- **Cons:** Verbose.  
- **Rationale:** Prevents hidden conversions.

### References vs pointers
- **Rule:** Prefer reference.  
- **Pros:** Clarity.  
- **Cons:** Can’t rebind references.  
- **Rationale:** Prevents null usage.

### Const-correctness
- **Rule:** All non-modified args must be `const`.  
- **Pros:** Safer.  
- **Cons:** Verbose.  
- **Rationale:** Prevents accidental mutation.

### Variadic
- **Rule:** No ellipses.  
- **Pros:** Type safety.  
- **Cons:** Less flexibility.  
- **Rationale:** Prevents undefined behavior.

### Unused
- **Rule:** No unused args. If unavoidable, `(void)arg`. Same for unused return values.  
- **Pros:** Clean builds.  
- **Cons:** Boilerplate.  
- **Rationale:** Avoids warnings.

---

## 7. Casting & Type Safety

- **Rule:** No C-style casts (except `(void)`).  
- **Pros:** Safer, explicit.  
- **Cons:** Verbose.  
- **Rationale:** Avoids silent dangerous casts.

- **Rule:** `reinterpret_cast` only to `uint8_t`.  
- **Pros:** Safer punning.  
- **Cons:** Restrictive.  
- **Rationale:** Avoids UB.  

- **Rule:** Use memcpy for type punning, not unions.  
- **Pros:** Defined behavior.  
- **Cons:** Slightly verbose.  
- **Rationale:** Avoids UB.

- **Rule:** Don’t remove const/volatile.  
- **Pros:** Preserves qualifiers.  
- **Cons:** Restrictive.  
- **Rationale:** Avoids subtle UB.

- **Rule:** Max 2 levels of indirection.  
- **Pros:** Improves readability.  
- **Cons:** Restrictive.  
- **Rationale:** Prevents spaghetti pointers.

---

## 8. Preprocessor, Headers & Build

### Pragmas
- **Rule:** Use `#pragma once`. Avoid others.  
- **Pros:** Cleaner than include guards.  
- **Cons:** Non-standard but widely supported.  
- **Rationale:** Simplicity.

### Macros
- **Rule:** Avoid macros. Never function-like macros.  
- **Pros:** Safer.  
- **Cons:** Some repetition.  
- **Rationale:** Avoids macro pitfalls.

### Includes
- **Rule:** Order: own header → stdlib → project headers.  
- **Pros:** Prevents hidden deps.  
- **Cons:** Boilerplate.  
- **Rationale:** Predictable builds.

### Compiler flags
- **Rule:** Must use `-Wall -Wextra -Wpedantic -Wswitch-default -Wunreachable-code -Wformat`.  
- **Pros:** Catches bugs.  
- **Cons:** Strict.  
- **Rationale:** Enforces quality.

### ISO C++
- **Rule:** No compiler extensions.  
- **Pros:** Portable.  
- **Cons:** Restrictive.  
- **Rationale:** Cross-platform reliability.

---

## 9. Comments & Documentation

- **Rule:** `//` comments only temporary, not in committed code.  
- **Pros:** Prevents commented-out code rot.  
- **Cons:** Restrictive.  
- **Rationale:** Keep repo clean.

- **Rule:** `/*...*/` for documentation, not code removal.  
- **Pros:** Encourages proper docs.  
- **Cons:** None.  
- **Rationale:** Clarity.

---

## 10. Globals, Arrays & Scope

- **Rule:** Avoid global scope. Use namespaces/classes.  
- **Pros:** Encapsulation.  
- **Cons:** Verbose.  
- **Rationale:** Avoid hidden deps.

- **Rule:** Arrays must have explicit or inferred size.  
- **Pros:** Prevents errors.  
- **Cons:** Verbose.  
- **Rationale:** Safety.

- **Rule:** No anonymous namespaces.  
- **Pros:** Explicitness.  
- **Cons:** Restrictive.  
- **Rationale:** Clearer linkage.

---

## 11. Miscellaneous

- Avoid `using namespace`.  
- Floats must always use `F` suffix.  
- All literal suffixes uppercase.  
- Unused virtual args must be `(void)`.  
- Provide operator formatting examples.  

---

## 12. Examples

### Operator alignment
```cpp
(((datagramHeader.command << DATAGRAM_HEADER_SHIFT_COMMAND) & DATAGRAM_HEADER_MASK_COMMAND) |
 ((datagramHeader.varID  >> DATAGRAM_HEADER_SHIFT_VAR_ID_HI) & DATAGRAM_HEADER_MASK_VAR_ID_HI))
```

### Switch
```cpp
switch (msg.type) {
case MsgType::Start: {
  handleStart(msg);
  break;
}
case MsgType::Stop: {
  handleStop(msg);
  break;
}
default: {
  /* Do Nothing */
  break;
}
}
```

### Initializer
```cpp
const std::array<int, 3> indices{0, 1, 2};
```

---

## 13. Suggested Additions

- Use explicit status/result objects.  
- Document concurrency/thread-safety.  
- Use constexpr aggressively.  
- Avoid global/static init order reliance.  
- Wrap debug behind `#if ATAMS_DEBUG`.  
- Mark single-arg ctors `explicit`.  

---

## Conclusion

These rules are designed to keep Atams safe, portable, and maintainable for embedded targets. Follow the rules strictly; if an exception is necessary, add a code comment explaining why and document it in the PR description.

**Guiding principles:** clarity, explicitness, and determinism.
