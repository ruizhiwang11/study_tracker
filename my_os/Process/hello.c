#include <unistd.h>

int main()
{
    while(1) {
        write(1, "Hello, World!\n", 13);
    }
}

// ══════════════════════════════════════════════════════════════════════════
// PROGRAM vs PROCESS
// ══════════════════════════════════════════════════════════════════════════

// PROGRAM — static description of a state machine
//   - just a file on disk (ELF binary)
//   - contains: instructions (.text), data (.rodata, .bss), metadata
//   - has no state, no execution, no life
//   - same program can spawn many processes simultaneously

// PROCESS — program in execution (the state machine running)
//   - kernel loads the ELF and brings it to life via fork() + exec()
//   - has its own private state:
//
//   PROGRAM STATE (from the ELF, changes at runtime):
//     registers  — rip (program counter), rsp (stack), rax, rdi, ...
//     memory     — address space: stack, heap, .text, .data mapped in
//
//   OS EXTRA STATE (kernel-owned, read-only to the process):
//     pid        — process identity (see /proc/$PID/status)
//     ppid       — parent process id
//     file descriptor table — open files (fd 0=stdin, 1=stdout, 2=stderr)
//     signal handlers
//     scheduling state — R(running) S(sleeping) D(uninterruptible) Z(zombie)
//     cgroups / namespaces (containers)

// ══════════════════════════════════════════════════════════════════════════
// WHAT THE KERNEL SEES FOR THIS PROCESS
// ══════════════════════════════════════════════════════════════════════════

// Readable from /proc/$PID/ while this runs:
//
//   /proc/$PID/status   → name, state, pid, ppid, memory (VmRSS, VmSize)
//   /proc/$PID/syscall  → current syscall: "1 0x1 0x... 0xd" = write(fd=1, len=13)
//   /proc/$PID/fd/      → fd 0,1,2 all → /dev/pts/0 (terminal)
//   /proc/$PID/maps     → virtual address layout:
//                           [.text]  r-xp  — executable code (this file)
//                           [stack]  rw-p  — user stack
//                           [libc]   r-xp  — C runtime
//   /proc/$PID/stat     → utime (user ticks), stime (kernel ticks)
//   /proc/$PID/schedstat → time on CPU, time in runqueue, timeslices

// strace -p $PID shows:
//   write(1, "Hello, World!\n", 13) = 13   ← repeated forever

// ══════════════════════════════════════════════════════════════════════════
// WHAT HAPPENS PER ITERATION
// ══════════════════════════════════════════════════════════════════════════

// userspace                         kernel
// ─────────────────                 ──────────────────────
// while(1) {
//   write(1,"Hello...",13)
//     │
//     │  syscall instruction
//     ▼
//                                   sys_write()
//                                   → fd 1 → terminal
//                                   → copy 13 bytes
//                                   → return 13
//     ▲
//     │  return from syscall
// }
//
// Every write() is a mode switch: userspace → kernel → userspace
// Process spends most time in kernel (sys_write), not in the while loop
