# xv6-labs-2022
labs of ***Operating System Engineering***, MIT 6.1818(Fall 2022)

## websites
+ schedule: https://pdos.csail.mit.edu/6.828/2022/schedule.html

+ xv6: https://github.com/mit-pdos/xv6-riscv

## translation
> maybe out of date
+ lab and doc translation: http://xv6.dgs.zone/

+ lecture translation: https://mit-public-courses-cn-translatio.gitbook.io/mit6-s081/

## how to debug user-level program
> debug ls.c for example
1. set a breakpoint at `main()` of it
2. `make qemu-gdb` and `gdb-multiarch kernel/kernel`
3. pause gdb and type `-exec file ./_ls` in gdb console
4. unpause gdb and invoke `ls` in qemu
5. when running to `main()` of `ls.c`, set breakpoints at any place you want

if you don't do like above, you'll run into `cannot access memory error` when setting breakpoints.
