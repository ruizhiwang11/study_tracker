// design realistic overload set for generic nth_power
// handles: signed integers, floating point, matrices

#include <type_traits>
#include <concepts>
#include <cmath>
#include <print>

// ── concepts ──────────────────────────────────────────────────────────────────

template <typename T>
concept SignedInteger = std::is_integral_v<T> && std::is_signed_v<T>;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <typename T>
concept Matrix = requires(T a, T b) {
    { a * b } -> std::same_as<T>;
    { T::identity() } -> std::same_as<T>;
};

// ── shared repeated-squaring kernel ───────────────────────────────────────────
// works for any type with operator* and an identity element
// safe for non-commutative types (matrices): keeps result = result * base order

namespace detail {
    template <typename T>
    T pow_impl(T base, unsigned int exp, T identity)
    {
        T result = identity;
        while (exp > 0) {
            if (exp & 1) result = result * base;
            base = base * base;
            exp >>= 1;
        }
        return result;
    }
}

// ── overload [1]: integer base, non-negative exp → stays integer ──────────────
// compute in unsigned to avoid signed overflow UB (C++20: cast back is defined)
auto nth_power(SignedInteger auto x, unsigned int n) -> decltype(x)
{
    using T = decltype(x);
    using U = std::make_unsigned_t<T>;
    return static_cast<T>(detail::pow_impl(static_cast<U>(x), n, U{1}));
}

// ── overload [2]: integer base, signed exp → may be negative → double ─────────
// note: n == INT_MIN is a known edge case (negation overflows); excluded here
auto nth_power(SignedInteger auto x, int n) -> double
{
    unsigned int e = static_cast<unsigned int>(n >= 0 ? n : -n);
    double result = static_cast<double>(nth_power(x, e));
    return n >= 0 ? result : 1.0 / result;
}

// ── overload [3]: integer base, float exp → fractional → std::pow ─────────────
auto nth_power(SignedInteger auto x, FloatingPoint auto n) -> double
{
    return std::pow(static_cast<double>(x), static_cast<double>(n));
}

// ── overload [4]: float base, integer exp → repeated squaring in float type ───
auto nth_power(FloatingPoint auto x, int n) -> decltype(x)
{
    using T = decltype(x);
    bool neg = n < 0;
    unsigned int e = static_cast<unsigned int>(neg ? -n : n);
    T result = detail::pow_impl(x, e, T{1});
    return neg ? T{1} / result : result;
}

// ── overload [5]: float base, float exp → std::pow ────────────────────────────
auto nth_power(FloatingPoint auto x, FloatingPoint auto n) -> decltype(x)
{
    using T = decltype(x);
    return static_cast<T>(std::pow(static_cast<double>(x), static_cast<double>(n)));
}

// ── overload [6]: matrix base, non-negative exp only ──────────────────────────
// negative exp requires matrix inverse which is not generally defined
auto nth_power(Matrix auto x, unsigned int n) -> decltype(x)
{
    using T = decltype(x);
    return detail::pow_impl(x, n, T::identity());
}

// ── example matrix type ───────────────────────────────────────────────────────

struct Mat2 {
    double a, b, c, d;
    Mat2 operator*(const Mat2& o) const {
        return {a*o.a + b*o.c,  a*o.b + b*o.d,
                c*o.a + d*o.c,  c*o.b + d*o.d};
    }
    static Mat2 identity() { return {1, 0, 0, 1}; }
};

// ── demo ──────────────────────────────────────────────────────────────────────

int main()
{
    // [1] integer, non-negative — fast, result stays integer
    std::println("2^10    = {}",  nth_power(2, 10u));     // 1024

    // [2] integer, signed — returns double (handles negative)
    std::println("2^10    = {}",  nth_power(2, 10));      // 1024.0
    std::println("2^-3    = {}",  nth_power(2, -3));      // 0.125

    // [3] integer, float exp — fractional power
    std::println("2^0.5   = {}",  nth_power(2, 0.5));     // 1.41421...

    // [4] float base, integer exp — repeated squaring in float type
    std::println("2.0^10  = {}",  nth_power(2.0, 10));    // 1024.0
    std::println("2.0f^-3 = {}",  nth_power(2.0f, -3));   // 0.125

    // [5] float base, float exp — fractional power
    std::println("2.0^0.5 = {}",  nth_power(2.0, 0.5));   // 1.41421...

    // [6] matrix — [[1,1],[1,0]]^n encodes Fibonacci numbers
    Mat2 fib{1, 1, 1, 0};
    Mat2 r = nth_power(fib, 10u);
    std::println("fib(10) = {}",  r.b);                   // 55
}
