# Algorithms / Interview Prep

## Approach
- One LeetCode problem per session minimum
- Focus: arrays, sliding window, two pointers, heaps, graphs, DP
- For each problem: brute force → optimize → complexity analysis

## Complexity Cheatsheet
- Sorting: O(n log n) comparison, O(n) counting/radix
- Hash map ops: O(1) avg
- Binary search: O(log n)
- BFS/DFS: O(V + E)
- Heap push/pop: O(log n)

## Common Patterns
- Sliding window: two pointers moving same direction
- Two-sum variant: sort + two pointers or hash map
- Monotonic stack: next greater element, largest rectangle
- Union-Find: connectivity, cycle detection
- Trie: prefix search

## C++ Interview Topics
- Rule of 5: destructor, copy ctor, copy assign, move ctor, move assign
- `virtual` dispatch: vtable pointer per object, vtable per class
- `constexpr` vs `const` vs `consteval`
- Template specialization vs overloading
- `std::variant` vs inheritance polymorphism
- `noexcept` and move semantics interaction (vector realloc)

## Weak points
- DP (2D, interval DP, bitmask DP)
- Graph algorithms: Dijkstra, Bellman-Ford, Floyd-Warshall
- Segment tree / BIT (Fenwick tree)
