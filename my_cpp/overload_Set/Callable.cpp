

#include <concepts>
#include <type_traits>
#include <utility>
#include <functional>
#include <print>

template <typename F, typename ...Ts>
decltype(auto) callable_1(F&& f, Ts&&... args)
{
    return std::forward<F>(f)(std::forward<Ts>(args)...);
}


template <typename F, typename ...Ts>
    requires std::invocable<F, Ts...> 
auto callable_2(F&& f, Ts&&... args) -> std::invoke_result_t<F, Ts...>
{
    return std::invoke(std::forward<F>(f), std::forward<Ts>(args)...);
}


void f(int& a)
{
    std::println("lvalue:  int&");
}

void f(int&& a)
{
    std::println("rvalue:  int&&");
}

void f(int& a, int& b)
{
    std::println("lvalues: int&, int&");
}

void f(int&& a, int&& b)
{
    std::println("rvalues: int&&, int&&");
}


int main()
{
    int x = 5;
    int y = 10;

    // f is an overloaded set — cannot be passed directly as a template argument.
    // wrap in a generic lambda so overload resolution happens inside the lambda.
    // auto call_f = [](auto&&... args) -> decltype(auto) {
    //     return f(std::forward<decltype(args)>(args)...);
    // };
    auto call_f = [](auto&& ...args) -> decltype(auto) {
        return f(std::forward<decltype(args)>(args)...);
    };

    std::println("── callable_1 (perfect forwarding) ──");

    callable_1(call_f, x);              // lvalue  → f(int&)
    callable_1(call_f, 42);             // rvalue  → f(int&&)
    callable_1(call_f, std::move(x));   // rvalue  → f(int&&)
    callable_1(call_f, x, y);           // two lvalues → f(int&, int&)
    callable_1(call_f, 1, 2);           // two rvalues → f(int&&, int&&)

    std::println("── callable_2 (std::invoke) ──");

    callable_2(call_f, x);              // lvalue  → f(int&)
    callable_2(call_f, 42);             // rvalue  → f(int&&)
    callable_2(call_f, std::move(x));   // rvalue  → f(int&&)
    callable_2(call_f, x, y);           // two lvalues → f(int&, int&)
    callable_2(call_f, 1, 2);           // two rvalues → f(int&&, int&&)

    std::println("── non-overloaded lambda ──");

    auto add = [](int a, int b) { return a + b; };
    std::println("{}", callable_2(add, 3, 4));   // 7

    auto * p = new int;
    delete delete delete p;
}
