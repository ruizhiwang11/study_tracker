#include <print>
#include <string_view>

// ══════════════════════════════════════════════════════════════════════════════
// HIDDEN FRIEND FUNCTIONS
//
// A hidden friend is a friend function DEFINED inside the class body.
//
// Three properties:
//   1. Lives in the enclosing namespace (not inside the class)
//   2. Invisible to unqualified lookup — cannot be called as a bare name
//   3. Visible ONLY via ADL — triggered when an argument matches the class type
//
// Name lookup path for a hidden friend:
//   unqualified lookup → not found
//   ADL: argument type Foo → search Foo's enclosing namespace → found ✓
//
// Why use it:
//   - No namespace pollution (fewer overload candidates for unrelated types)
//   - Can access private members (it's a friend)
//   - Prevents accidental implicit conversion on the left argument
//   - Faster compilation (fewer candidates to consider)
// ══════════════════════════════════════════════════════════════════════════════


// ── 1. basic hidden friend ─────────────────────────────────────────────────
//
// operator== and operator< live in the global namespace but are ONLY reachable
// when an argument of type Foo is present (ADL trigger).
//
// Calling operator==(x,y) directly → error: unqualified lookup cannot see it.
// Calling x == y           → ok:    ADL finds it via the Foo arguments.

struct Foo {
    int value;

    friend bool operator==(Foo a, Foo b) { return a.value == b.value; }
    friend bool operator< (Foo a, Foo b) { return a.value <  b.value; }
};


// ── 2. hidden friend accessing private members ─────────────────────────────
//
// Because it is a friend, it can reach private fields directly.
// The caller writes dot(u, v) — free function syntax — without needing
// getters or public fields.
//
// ADL trigger: both arguments are Vec2, so the enclosing namespace is searched.

struct Vec2 {
private:
    float x, y;
public:
    Vec2(float x, float y) : x(x), y(y) {}

    // accesses private x, y — allowed because it is a friend
    friend float dot(Vec2 a, Vec2 b) {
        return a.x * b.x + a.y * b.y;
    }

    // operator<< for std::ostream: ADL fires on BOTH arguments
    //   - os  is std::ostream  → searches std namespace
    //   - v   is Vec2          → searches enclosing namespace (global here)
    // Either path would find this function.
    friend void print(Vec2 v) {
        std::println("Vec2({}, {})", v.x, v.y);
    }
};


// ── 3. non-hidden friend (contrast) ───────────────────────────────────────
//
// Declaring the friend inside but DEFINING outside puts the function in the
// namespace and makes it visible to unqualified lookup.
// This means it appears as a candidate even when Bar is not involved — it
// pollutes the overload set for unrelated calls.

struct Bar {
    int value;
    friend bool operator==(Bar a, Bar b);   // declaration only — not hidden
};

bool operator==(Bar a, Bar b) {             // definition outside → visible to unqualified lookup
    return a.value == b.value;
}

// contrast:
//   Foo::operator== — hidden, only ADL finds it
//   Bar::operator== — visible everywhere, unqualified lookup finds it


// ── 4. HFT-style: Price strong typedef ────────────────────────────────────
//
// Price wraps a tick count as a distinct type (not just a long).
// Hidden friends give it full arithmetic and comparison operators without
// leaking any symbol into the market namespace's unqualified scope.
//
// Callers write:  p1 + p2,  p1 < p2,  print(p1)
// All resolved via ADL on the Price arguments.
//
// Note: market::operator+(p1, p2) would be an ERROR — hidden friends
// cannot be reached by qualified lookup either.

namespace market {

struct Price {
    long ticks;
    explicit Price(long t) : ticks(t) {}

    // arithmetic
    friend Price operator+(Price a, Price b) { return Price{a.ticks + b.ticks}; }
    friend Price operator-(Price a, Price b) { return Price{a.ticks - b.ticks}; }

    // comparison
    friend bool operator==(Price a, Price b) { return a.ticks == b.ticks; }
    friend bool operator< (Price a, Price b) { return a.ticks <  b.ticks; }
    friend bool operator> (Price a, Price b) { return a.ticks >  b.ticks; }

    // named helper — ADL finds this via the Price argument
    friend void print(Price p) { std::println("Price ticks: {}", p.ticks); }
};

} // namespace market


// ── 5. ADL suppression ────────────────────────────────────────────────────
//
// If a local variable has the same name as the function being called,
// unqualified lookup finds the variable and ADL is suppressed entirely.
// The hidden friend becomes completely unreachable.

void adl_suppression_demo() {
    Foo a{1}, b{2};
    (void)(a == b);       // ok: ADL fires on Foo → finds hidden operator==

    // uncommenting this breaks the line below:
    // int operator== = 0;  // local variable named operator== shadows the name
    // (void)(a == b);      // error: ADL suppressed; operator== is an int, not callable
}


// ── 6. operator<< placement: inside vs outside ────────────────────────────
//
// operator<< takes ostream as the first argument (not the class type).
// ADL still fires on the SECOND argument (the class type), so hidden friend
// works just as well as defining it outside.
//
// Common convention: define operator<< outside because the first argument
// (ostream) makes it read as "this is an ostream operation, not a Foo operation."
// Both are equivalent in terms of lookup.
//
//   inside  (hidden friend): found via ADL on Foo argument
//   outside (non-hidden):    found via ADL on Foo argument AND unqualified lookup
//
// For symmetric operators (==, <, +) hidden friend is preferred — it prevents
// the compiler from considering implicit conversions on the LEFT argument.


// ── demo ──────────────────────────────────────────────────────────────────

int main()
{
    // 1. basic — ADL finds hidden operator== and operator<
    Foo x{1}, y{1}, z{2};
    std::println("x==y: {}", x == y);   // true
    std::println("x==z: {}", x == z);   // false
    std::println("x< z: {}", x < z);    // true
    // operator==(x, y);                // would be ERROR — not in unqualified scope

    // 2. hidden friend with private access
    Vec2 u{1.0f, 0.0f}, v{0.0f, 1.0f};
    std::println("dot(u,v): {}", dot(u, v));   // ADL finds dot via Vec2 argument
    print(u);

    // 3. market::Price — full operator set, nothing leaks into unqualified scope
    market::Price p1{100}, p2{200};
    market::Price p3 = p1 + p2;           // ADL: Price argument → searches market ns
    std::println("p1 < p2: {}", p1 < p2); // true
    std::println("p3==300: {}", p3 == market::Price{300});
    print(p3);                             // ADL finds market::print via Price argument
    // market::operator+(p1, p2);          // would be ERROR — cannot qualify hidden friend

    // 4. ADL suppression
    adl_suppression_demo();
}
