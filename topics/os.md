# OS / Systems Notes

## State Machine Mental Model
- Computer = deterministic state machine: (registers, memory) → next state via instruction
- OS = program that manages transitions and resource allocation
- Key insight: everything is state; debugging is state tracing

## x86 Registers (System V AMD64 ABI)
- `rax` — return value / accumulator
- `rbx` — callee-saved (preserved across calls)
- `rcx` — 4th arg / counter
- `rdx` — 3rd arg / high half of 128-bit mul result
- `rsi` — 2nd arg
- `rdi` — 1st arg
- `rsp` — stack pointer (callee must restore)
- `rbp` — frame pointer (callee-saved, optional)
- `rip` — instruction pointer
- `r8–r9` — 5th, 6th args
- `r10–r11` — caller-saved (scratch)
- `r12–r15` — callee-saved

## Boot Process (x86)
- BIOS/UEFI → bootloader (GRUB) → kernel
- Real mode (16-bit) → protected mode (32-bit) → long mode (64-bit)
- CR0, CR3, CR4 control mode transitions and paging

## RISC-V / opensbi
- OpenSBI = supervisor binary interface, M-mode firmware
- See: `my_os/opensbi/`

## Virtual Memory
- Page table walks: CR3 → PML4 → PDPT → PD → PT → physical frame
- TLB caches virtual→physical translations; `invlpg` flushes one entry

## Weak points
- Context switch mechanics in detail (what registers saved where)
- `mmap` internals: when pages are actually allocated (demand paging)
- Difference between kernel stack and user stack
