#include <concepts>
#include <iostream>
#include <print>
#include <type_traits>

template <typename T>
concept NumericType = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <typename T>
concept MoreComplexCondition = requires {
    requires std::is_integral_v<T> || std::is_floating_point_v<T>;
    requires sizeof(T) > 2;
};

template <typename T1, typename T2>
    concept VeryComplexCondition = requires {
    requires std::is_convertible_v<T1, T2>;
    requires std::is_base_of_v<T1, T2>;
    requires std::is_default_constructible_v<T2>;
};




template <typename T>
consteval bool complexCheck() {
    if constexpr (std::is_integral_v<T>)
        return sizeof(T) > 2;
    else if constexpr (std::is_floating_point_v<T>)
        return sizeof(T) > 4;
    else
        return false;
}


template <typename T> requires (NumericType<T>)
void fun()
{
    std::println("do something");
}

template <typename Derived>
class ShapeBase
{
protected:
    Derived* getDerived(){static_cast<Derived*>(this);}
    bool m_configured{false};
    int m_someValue{0};
public: 
    void render()
    {
        getDerived()->render();
    }
    void draw()
    {
        getDerived()->draw();
    }
    bool config(int value)
    {
        m_someValue = value;
        return m_someValue !=0;
    }
};

class Circle : public ShapeBase<Circle>
{
protected:
    using Base = ShapeBase<Circle>;
    double m_radius{0.0};
public:
    bool config(int value, double radius)
    {
        if (not Base::config(value))
        {
            return false;
        }
        m_radius = radius;
        return m_radius > 1e-5;;
    }
    void render()
    {
        std::println("render");
    }
    void draw()
    {
        std::println("draw");
    }
};


template <typename T> 
int foo(T)
requires requires(T t) {requires noexcept(++t);}
{
    return 42;
}

template <typename T>
int bar(T)
requires (noexcept(++std::declval<T&>()))
{
    return 42;
}

// funny abbreviation 

template <typename T>
requires (std::same_as<T, int>)
struct S{};

template <std::same_as<int> T>
struct S1{};


// you can constraint member function, but bad

// template <typename D> struct Foo {
//     bool empty() requires std::ranges::forward_range<D>;
// }

// template <typename T> struct Foo{
//     T val;
//     bool empty() requires requires {val.empty();
//     }
// }

// you can constrain only constructor with constraining type


template <typename T> requires requires(T x ){x.foo();}
struct Foo{
    T t;
    Foo() requires std::default_initializable<T> : t() {}
    Foo(Foo &f) requires std::copyable<T> : t(f.t) {}
};


// concept partial order

template <typename T> concept Ord = requires(T a, T b){a < b;};
template <typename T> concept Inc = requires(T a){ ++a; };
template <typename T> concept Int = std::is_same_v<T, int>;

template <typename  T> requires Ord<T> || Inc<T> || Int<T>
int foo(T x) {return 2;}

template <typename T> requires Ord<T>
int foo(T x) {return 22;}

int foo(int x) {return 42;}

// int main()
// {   
//     fun<decltype(1)>();
//     fun<decltype(2)>();
//     fun<decltype(1.5)>();

//     std::integral auto a = 123;
//     //un<decltype("123")>();

    
//     return 0;
// }



int main()
{   
    fun<decltype(1)>();
    fun<decltype(2)>();
    fun<decltype(1.5)>();

    std::integral auto a = 123;
    //un<decltype("123")>();

    
    return 0;
}