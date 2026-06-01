#include <print>
#include <utility>

// ── target functions to call ───────────────────────────────────────────────

void show(int& x)  { std::println("lvalue int: {}", x); }
void show(int&& x) { std::println("rvalue int: {}", x); }
void show(double x){ std::println("double: {}", x); }

// ── 1. basic pack: declare and expand ─────────────────────────────────────

template <typename ...Ts>
void print_all(Ts... args)
{
    (std::println("{}", args), ...);  // fold: call println for each arg
}

// ── 2. perfect forwarding: preserve lvalue/rvalue ─────────────────────────

template <typename ...Ts>
void forward_all(Ts&&... args)
//               ^^^  forwarding references
{
    (show(std::forward<Ts>(args)), ...);
    //    ^^^^^^^^^^^^^^^^^^^^  expand both Ts and args together
}

// ── 3. fold expressions ───────────────────────────────────────────────────

template <typename ...Ts>
auto sum(Ts... args)
{
    return (... + args);   // left fold: ((a0 + a1) + a2) + ...
}

template <typename ...Ts>
bool all_positive(Ts... args)
{
    return (... && (args > 0));  // fold with compound expression
}

template <typename ...Ts> auto left_sub(Ts... a)  { return (... - a);  }
template <typename ...Ts> auto right_sub(Ts... a) { return (a - ...);  }

// template <typename ...Ts>
// void print_all(Ts&&... args)
// {
//     (std::println("{}", std::forward<Ts>(args)), ...);  // left fold on ,
// }

// binary fold: init value handles empty pack
template <typename ...Ts>
auto sum_2(Ts... args)
{
    return (0 + ... + args);   // binary left fold, safe for empty pack
}

// ── demo ──────────────────────────────────────────────────────────────────

int main()
{
    // 1. basic expand
    print_all(1, 2.5, "hello");   // 1 / 2.5 / hello

    // 2. forwarding — lvalue vs rvalue preserved
    int x = 10;
    forward_all(x, 42);           // lvalue int: 10 / rvalue int: 42

    // 3. fold
    std::println("{}", sum(1, 2, 3, 4));          // 10
    std::println("{}", all_positive(1, 2, 3));    // true
    std::println("{}", all_positive(1, -1, 3));   // false
}