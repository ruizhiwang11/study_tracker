
// Indistingushable declarations
int b();
const int b(); // error overloading prohibited



int f(int);
int f(const int); // ok re-declears f(int)

// the logic behind this process related o the name mangling rules

int a();
int a() noexcept;

struct S{
    static void c();
    void c();

    int d() &;
    int d();
};


typedef int Int;

void e(int);
void e(Int);

struct B{
    int f(int) {return 1;}
};

struct D : B{
    // using B::f; // if enabled this will enable the scope, so result will be 1
    int f(const char*) {return 2;}
};

namespace B1{int x = 0;}
namespace C{int x = 0;}

namespace A{
    using namespace B1;
    void f()
    {
        using C::x;
        A::x = 1; // set B1::x 
        x = 2;  //  set C::x
    } 
}

namespace N{
    int X =1;
}


namespace ex {
    struct Msg {};
    void process(Msg, int);
}

void process(ex::Msg, int);   // global overload


// rules of base look up
struct M{
    struct T {int x = 1;};
};

struct P { int x = 2;};

template <typename T>
struct O : P{
    int f() {T t; return t.x;}
};

//   Inside f(), single scope search for T:
//   1. Local scope of f() — not found
//   2. Class scope of O — template parameter T found here
//   3. Never reaches enclosing namespace — stops

//   So T = template parameter = P (at instantiation O<P>).

//   T t creates a local P object. t.x = P::x = 2.

//   M::T is never involved — it has no path into scope. The template parameter T shadows any outer name T the moment lookup finds it in class scope.

//   ---
//   The trick: two things named T exist in the program (M::T and the template param), but the template parameter wins because class scope is found before the
//   enclosing namespace in the subroutine. 




int main()
{
    D d;
    d.f(0); // it will be 2 as the name is hidden

    int N =2; // simple look up, find the N local N first, then namespace
    N += N::X;
    
    ex::Msg m;
    ::process(m, 42);   // which process wins?

    O<P> O;
    O.f(); // which one

}