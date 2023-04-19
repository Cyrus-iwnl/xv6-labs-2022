# Lab Report

## lab1 utilities
### 功能
+ 实现一些简单的Unix程序，例如`sleep, find, xargs`
+ 利用管道进行进程间通信 `pingpong`
+ 一个并发的素数筛选程序 `primes`

### 问题
+ 并发素数程序有点意思，可参考[这个网站](https://swtch.com/~rsc/thread/)

## lab2 system calls
### 功能
实现两个系统调用
+ 追踪系统调用 `trace`
+ 收集系统信息 `sysinfo`

### 问题
+ 用户态的函数可以直接传递参数，而系统调用不能直接传递参数，必须通过寄存器。内核读取寄存器来保存参数(`argint,argaddr`)。
+ 内核函数不能直接访问用户空间，必须通过特殊的方式将内存从内核拷贝到用户空间。例如在如下系统调用中，内核必须通过`copyout()`把`struct sysinfo`拷贝回用户空间。
  ```c
  int sysinfo(struct sysinfo*);
  ```

## lab3 page tables
### 功能
+

### 问题


## lab4 traps

## lab5 copy-on-write fork

## lab6 multithreading

## lab7 network driver

## lab8 locks

## lab9 file system

## lab10 mmap