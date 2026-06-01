#include <concepts>
#include <type_traits>


template <typename T>
concept SignedInteger = std::is_integral_v<T> && std::is_signed_v<T>;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <typename T>
concept Matrix = requires(T a, T b){
    {a * b} -> std::same_as<T>;
    {T::identity()} -> std::same_as<T>;
};

namespace details{
    template <typename T>
    T pow_impl(T base, unsigned int exp, T identity)
    {
        T result = identity;
        while(exp > 0)
        {
            if(exp & 1) result = result *base;
            base = base *base;
            exp >>=1;
        }
        return result;
    }
}

template <typename T>
requires SignedInteger<T>
T nth_power(T base, unsigned int exp)
{
    using U = std::make_unsigned_t<T>;
    return static_cast<T>(details::pow_impl(static_cast<U>(base), exp, 1));
}

