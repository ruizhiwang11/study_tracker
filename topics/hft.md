# HFT / Low-Latency Systems Notes

## Cache / NUMA
- False sharing: two threads writing different data in the same cache line (64 bytes)
- Fix: `alignas(64)` to pad hot variables to separate cache lines
- NUMA: memory access to remote node is ~2–3x slower; pin threads to NUMA node, allocate local memory
- `numactl --cpunodebind=0 --membind=0 ./app`

## CPU Affinity / Isolation
- `taskset -c 3 ./app` — pin process to core 3
- `isolcpus=3` kernel param — isolate core from scheduler
- `nohz_full=3` — stop timer ticks on isolated core

## Lock-Free Structures
- SPSC queue: safe with just acquire/release on indices
- MPMC: needs CAS loops; ABA problem → version tags or hazard pointers
- `compare_exchange_weak` in loop (spurious failure ok); `_strong` for single attempt

## Kernel Bypass / Networking
- DPDK: poll-mode driver, bypasses kernel network stack
- Solarflare/ef_vi: direct NIC access for ultra-low latency
- UDP multicast typical for market data feeds
- TCP for order entry (FIX / binary protocols)

## Exchange Protocols
- HKEX: proprietary binary protocol, market data + order entry
- Order types: limit, market, IOC, FOK, GTD
- Sequence numbers + gap detection for reliable multicast

## Branchless / Precomputed Processing
- Replace if/else with lookup tables or arithmetic
- `cmov` (conditional move) avoids branch misprediction
- Precompute message field offsets for zero-parse dispatch

## Resume Bullets (to develop)
- HKEX market data layer: ingestion, gap detection, sequence validation
- FPGA integration: interfacing with FPGA-based market data capture
- Branchless/precomputed RX message processing
- Exchange connectivity: order entry + market data (same or separate paths)
- Simulator: order matching logic, protocol replay
- QA rebuild: test harness for exchange protocol compliance

## Weak points
- `io_uring` internals and when it beats epoll
- Memory-mapped files for IPC between trading components
- Latency measurement methodology: `rdtsc`, histogram bucketing
