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

## Weak points
- `std::memory_order_acq_rel` vs separate acquire/release — when to use each
- Hazard pointers / epoch-based reclamation for MPMC structures
- `[[likely]]` / `[[unlikely]]` branch hints and their codegen effect
