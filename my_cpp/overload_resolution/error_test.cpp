// ══════════════════════════════════════════════════════════════════════════════
// OVERLOAD RESOLUTION — WHAT IS AND IS NOT A VALID OVERLOAD
//
// Two declarations are the same function (redeclaration) if they have the same
// name and the same parameter types after stripping top-level cv-qualifiers.
// Two declarations are an invalid overload if return types differ but params match.
// ══════════════════════════════════════════════════════════════════════════════


// ── 1. return type alone cannot distinguish overloads ─────────────────────
//
// Overload resolution selects a function from arguments only — the return type
// is not part of the signature used for selection.
// Declaring two functions with the same name + params but different return types
// is an error (not an overload).
#include <utility>
#include <cassert>

int b();
// const int b();   // ERROR: return type differs, params identical — not a valid overload


// ── 2. top-level const on parameters is ignored ───────────────────────────
//
// Top-level const on a by-value parameter is stripped before comparing signatures.
// int f(int) and int f(const int) are the SAME declaration — the const only
// affects the local copy inside the function body, not the caller.
// Name mangling treats them identically.

int f(int);
int f(const int);   // ok: redeclaration of f(int), NOT a new overload


// ── 3. noexcept does not create an overload ───────────────────────────────
//
// noexcept is part of the function TYPE but not the signature for overloading.
// You cannot have two overloads differing only by noexcept.

int a();
// int a() noexcept;   // ERROR: same signature, cannot overload on noexcept alone


// ── 4. static vs non-static member — not a valid overload ─────────────────
//
// Static and non-static members with the same name and params cannot coexist.
// Ref-qualifiers (&, &&) CAN create overloads — they are part of the signature.

struct S {
    // static void c();
    // void c();        // ERROR: static and non-static with same name+params

    int d() &;          // called on lvalue objects
    int d() &&;         // called on rvalue objects — valid overload via ref-qualifier
};


// ── 5. typedef does not create a new type ─────────────────────────────────
//
// typedef int Int makes Int an alias for int — same type, same mangling.
// void e(int) and void e(Int) are the same declaration.

typedef int Int;
void e(int);
// void e(Int);     // ERROR: redeclaration of e(int), Int IS int


// ── 6. name hiding in class hierarchy ────────────────────────────────────
//
// When a derived class declares a member with the same name as a base member,
// ALL base overloads of that name are hidden — not just the matching signature.
// Overload resolution never reaches the base candidates.
//
// d.f(0):
//   unqualified lookup finds D::f (single scope search stops at D)
//   B::f is hidden — never considered
//   D::f(const char*) is the only candidate → 0 converts to const char* (int→ptr)
//   result: 2
//
// Fix: add 'using B::f;' to bring B's overloads into D's scope

struct B {
    int f(int) { return 1; }
};

struct D : B {
    using B::f;          // without this, B::f(int) is hidden and d.f(0) calls D::f
    int f(const char*) { return 2; }
};


// ── 7. namespace lookup + using ───────────────────────────────────────────
//
// 'using namespace X' brings X's names into scope for unqualified lookup,
// but a local 'using X::name' declaration takes precedence.
//
// Inside A::f():
//   'using namespace B1' is in A's scope — B1::x visible as x
//   'using C::x' is a local declaration — shadows B1::x for bare 'x'
//
//   A::x = 1  → qualified lookup into A → A has 'using namespace B1' → sets B1::x
//   x = 2     → unqualified lookup → local 'using C::x' wins → sets C::x

namespace B1 { int x = 0; }
namespace C  { int x = 0; }

namespace A {
    using namespace B1;
    void f() {
        using C::x;
        A::x = 1;   // qualified into A → follows using namespace B1 → B1::x = 1
        x = 2;      // unqualified → local using C::x wins → C::x = 2
    }
}


// ── 8. local variable does not shadow namespace in qualified lookup ────────
//
// Unqualified lookup: 'int N = 2' hides namespace N for bare name N
// Qualified lookup:   'N::' looks only for namespace/class names — ignores variables
//
// So in 'N += N::X':
//   left  N  → unqualified → finds local int N = 2
//   right N:: → qualified, namespace-only lookup → finds namespace N
// Both resolve correctly — no conflict.

namespace N { int X = 1; }


// ── 9. ADL ambiguity ──────────────────────────────────────────────────────
//
// process(m, 42):
//   unqualified lookup finds ::process(ex::Msg, int)
//   ADL on ex::Msg finds ex::process(Msg, int)
//   both are exact matches → no tiebreaker → AMBIGUITY ERROR
//
// Fix: qualify the call — ::process or ex::process

namespace ex {
    struct Msg {};
    void process(Msg, int);
}

void process(ex::Msg, int);   // global — creates ambiguity with ex::process via ADL


// ── 10. template parameter shadows enclosing name ─────────────────────────
//
// Inside O<T>::f(), lookup for T:
//   1. local scope of f()   — not found
//   2. class scope of O     — template parameter T found → stops
//   3. enclosing namespace  — never reached
//
// O<P>: T = P → 'T t' creates a P object → t.x = P::x = 2
// M::T (x=1) is never reachable — template parameter wins in class scope

struct M { struct T { int x = 1; }; };
struct P { int x = 2; };

template <typename T>
struct O : P {
    int f() { T t; return t.x; }
    // note: base class P is non-dependent, but t.x accesses T (dependent) → resolved at instantiation
};


// ── 11. explicit constructor removes a candidate from the viable set ──────
//
// f1({{}, {}}):
//   Two candidates: f1(Pair<E,E>) and f1(Pair<S1,S1>)
//   Argument {{}, {}} must construct the Pair — each inner {} constructs one element.
//
//   Inner {} is in a copy-initialization context (function argument from braced-init).
//   explicit constructors are EXCLUDED from copy-initialization.
//
//   Pair<E,E>:    E() is explicit → {} cannot implicitly construct E → NOT VIABLE
//   Pair<S1,S1>:  S1() is non-explicit → {} can construct S1 → VIABLE ✓
//
//   Only one viable candidate → f1(Pair<S1,S1>) wins → returns 2.
//
// Key rule: explicit is not just a style hint — it directly controls which
// overloads survive the viable function filter.

template <class T1, class T2> struct Pair {
    template <class U1 = T1, class U2 = T2> Pair(U1&&, U2&&) {}
};

struct S1 { S1() = default; };
struct E  { explicit E() = default; };   // explicit blocks copy-init from {}
int f1(Pair<E,E>)   { return 1; }        // not viable for f1({{},{}})
int f1(Pair<S1,S1>) { return 2; }        // viable — wins


// ─────────────────────────────────────────────────────────────────────────────

int main()
{
    // 6. name hiding — D::f hides B::f; using B::f restores it
    D d;
    d.f(0);          // with 'using B::f': calls B::f(int) → 1
                     // without it:        calls D::f(const char*) → 2

    // 8. local N does not block N::X
    int N = 2;
    N += N::X;       // N (local int) += namespace N's X (1) → N = 3

    // 9. ADL ambiguity — must qualify to resolve
    ex::Msg m;
    ::process(m, 42);    // ok: qualified → skips ADL, uses global process
    // process(m, 42);   // ERROR: ambiguous — ::process and ex::process both match

    // 10. template param T wins over M::T
    O<P> o;
    o.f();           // T = P → t.x = P::x = 2

    assert(f1({{}, {}}) == 2);
}
