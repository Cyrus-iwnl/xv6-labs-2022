# xv6-labs-2022
labs of ***Operating System Engineering***, MIT 6.1818(Fall 2022)

## Introduction
Design and implementation of operating systems, and their use as a foundation for systems programming. Topics include virtual memory; file systems; threads; context switches; kernels; interrupts; system calls; interprocess communication; coordination, and interaction between software and hardware. A multi-processor operating system for RISC-V, xv6, is used to illustrate these topics. Individual laboratory assignments involve extending the xv6 operating system, for example to support sophisticated virtual memory features and networking.

You may wonder why we are studying xv6, an operating system that resembles Unix v6, instead of the latest and greatest version of Linux, Windows, or BSD Unix. xv6 is big enough to illustrate the basic design and implementation ideas in operating systems. On the other hand, xv6 is far smaller than any modern production O/S, and correspondingly easier to understand. xv6 has a structure similar to many modern operating systems; once you've explored xv6 you will find that much is familiar inside kernels such as Linux.

## Websites
+ schedule: https://pdos.csail.mit.edu/6.828/2022/schedule.html
+ xv6: https://github.com/mit-pdos/xv6-riscv
+ xv6 book: https://pdos.csail.mit.edu/6.828/2022/xv6/book-riscv-rev3.pdf

## Lab Report
+ see [report.md](./report.md)

## How to debug user-level program
> debug ls.c for example
1. set a breakpoint at `main()` of it
2. `make qemu-gdb` and `gdb-multiarch kernel/kernel`
3. pause gdb and type `-exec file ./_ls` in gdb console
4. unpause gdb and invoke `ls` in qemu
5. when running to `main()` of `ls.c`, set breakpoints at any place you want

if you don't do like above, you'll run into `cannot access memory error` when setting breakpoints.

## 中文翻译
> 可能不准确并且过时，仅供参考。
+ 实验和手册： http://xv6.dgs.zone/

+ 讲课： https://mit-public-courses-cn-translatio.gitbook.io/mit6-s081/
