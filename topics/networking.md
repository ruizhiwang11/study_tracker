# Networking Notes

## UDP vs TCP for Trading
- UDP multicast: low latency, no connection overhead, packet loss possible → sequence + gap fill
- TCP: reliable, ordered, but head-of-line blocking; used for order entry

## Multicast
- Join group via `IP_ADD_MEMBERSHIP`
- TTL controls multicast scope; `SO_REUSEPORT` for multiple consumers on same port

## Sockets / epoll
- `epoll_wait` → event-driven I/O, scales to many fds
- Edge-triggered (`EPOLLET`): must drain fd completely on each notification
- Level-triggered (default): notified as long as data available

## Kernel Bypass
- DPDK poll-mode driver: busy-polls NIC ring buffer directly from userspace
- Eliminates: interrupt overhead, context switch, kernel network stack copy
- Solarflare ef_vi / OpenOnload: similar model, different vendor

## TCP Tuning (low-latency)
- `TCP_NODELAY` — disable Nagle algorithm (don't batch small packets)
- `SO_SNDBUF` / `SO_RCVBUF` — tune socket buffer sizes
- Busy-poll: `SO_BUSY_POLL` — spin in kernel before blocking

## Weak points
- RDMA / RoCE (remote DMA for ultra-low latency messaging)
- `io_uring` for async I/O without syscall overhead
- FIX protocol framing and checksum rules
