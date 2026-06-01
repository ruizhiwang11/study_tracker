# C++ Notes

## Memory Model
- `std::memory_order_relaxed` — no ordering guarantee, only atomicity
- `std::memory_order_acquire` — no reads/writes in current thread move before this load
- `std::memory_order_release` — no reads/writes in current thread move after this store
- `std::memory_order_seq_cst` — total order across all threads (default, most expensive)
- Acquire-release pairs create happens-before edges between threads

## Move Semantics / Forwarding
- `T&&` in template context = forwarding ref (not rvalue ref)
- `std::forward<T>(x)` preserves value category
- `std::move(x)` unconditionally casts to rvalue
- See: `my_cpp/overload_Set/perfect_fowarding.cpp`

## CRTP (Curiously Recurring Template Pattern)
- Static polymorphism, zero virtual dispatch overhead
- Base<Derived> calls `static_cast<Derived*>(this)->impl()`

## Ring Buffer
- Lock-free SPSC: producer owns write index, consumer owns read index
- Key: indices only move forward; mod capacity for actual slot
- Memory ordering: store with release, load with acquire (on the index)
- See: `my_cpp/ringbuffer/buffer.cpp`

## Overload Resolution

Three phases: **name lookup → overload resolution → access check**

### Name Lookup

**Core subroutine — single scope search:**
- Search one scope for the name
- If that scope is a class or template instantiation, also search its base classes
- Single scope search is preferred — stops as soon as the name is found

**Three modes, all using the same subroutine:**

| Mode | When | How |
|------|------|-----|
| Unqualified | bare name | subroutine applied scope by scope outward; stop at first match |
| ADL | unqualified function call, after unqualified lookup | subroutine repeated over namespaces associated with argument types |
| Qualified | `X::name` | subroutine applied to X only — one scope, done |

ADL is a second pass of the same subroutine, not a separate parallel mechanism.
Qualified lookup skips unqualified and ADL entirely.

**Qualified left-side rule:** in `N::X`, the `N` is looked up for namespace/class names only — ordinary variables are invisible there. So a local `int N` does not shadow namespace `N` in `N::X`.

**Overloaded function names have no single address** — must wrap in a lambda so overload resolution happens inside where argument types are known.

### Viable Function Filter
1. Argument count matches (after defaults/variadic)
2. Implicit conversion sequence (ICS) exists for each argument

### ICS Ranking (best → worst)
1. Exact match (or trivial decay)
2. Qualification conversion (add const/volatile)
3. Numeric promotion (int→long, float→double)
4. Numeric conversion (int→double, lossy ok)
5. User-defined conversion (ctor or operator T())
6. Ellipsis `...` (worst)

Best viable = all args at least as good, one strictly better. Tie → ambiguity error.

### Lvalue/Rvalue Reference Overloading
- `T&` binds lvalues only
- `T&&` binds rvalues only
- `const T&` binds both (universal fallback)
- Forwarding ref (`T&&` in template): deduced as `T=int&` (lvalue) or `T=int` (rvalue)
- Reference collapsing: `T& &&` → `T&`; `T&& &&` → `T&&`

### Templates in Overload Resolution
- Template deduction runs first; then template instantiation competes as a regular function
- Non-template beats template on exact tie (more specialized)

### Concepts — Partial Ordering
- More constrained candidate wins over less constrained
- Subsumption: A subsumes B if A's atomic constraints ⊇ B's
- Non-template > more constrained template > less constrained template
- See: `my_cpp/overload_Set/test.cpp` — `foo(int)` beats `requires Ord<T>` beats `requires Ord||Inc||Int`

### Lambda wrapper pattern (important for HFT dispatch)
```cpp
// cannot pass overloaded set f directly — no single address
auto call_f = [](auto&&... args) -> decltype(auto) {
    return f(std::forward<decltype(args)>(args)...);
};
// lambda has single address; overload resolution happens inside at call time
```
See: `my_cpp/overload_Set/Callable.cpp`

## Weak points
- `std::memory_order_acq_rel` vs separate acquire/release — when to use each
- Hazard pointers / epoch-based reclamation for MPMC structures
- `[[likely]]` / `[[unlikely]]` branch hints and their codegen effect
- Overload resolution with multiple user-defined conversions (ambiguity rules)
- `if constexpr` vs `requires` — when each is the right tool
