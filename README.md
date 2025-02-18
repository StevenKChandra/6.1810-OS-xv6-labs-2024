# xv6 labs excersice
This is a lab excercise repository for the MIT's 6.1810 Operating System course. I found the initial repository from [course homepage](https://pdos.csail.mit.edu/6.1810/) and followed the lab instruction.

## Things I implemented and learned:
#### 1. Utilities:
- sleep.c - receiving arguments from shell input and using sleep system call
- pingpong.c - building pipe and passing message to other process
- primes.c - using fork() to create child process recursively and building pipe to pass messages
- find.c - pointer arithmatic practice, understanding file system, and reading directory using system call
- xargs.c - understanding exec system call and practicing stiring parsing (fun experience :))

## **About xv6**

<p align="justify">
xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.
</p>

## **ACKNOWLEDGMENTS**

<p align="justify">
xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)).  See also https://pdos.csail.mit.edu/6.1810/, which provides
pointers to on-line resources for v6.
</p>

<p align="justify">
The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.
</p>

<p align="justify">
We are also grateful for the bug reports and patches contributed by
Takahiro Aoyagi, Marcelo Arroyo, Silas Boyd-Wickizer, Anton Burtsev,
carlclone, Ian Chen, Dan Cross, Cody Cutler, Mike CAT, Tej Chajed,
Asami Doi,Wenyang Duan, eyalz800, Nelson Elhage, Saar Ettinger, Alice
Ferrazzi, Nathaniel Filardo, flespark, Peter Froehlich, Yakir Goaron,
Shivam Handa, Matt Harvey, Bryan Henry, jaichenhengjie, Jim Huang,
Matúš Jókay, John Jolly, Alexander Kapshuk, Anders Kaseorg, kehao95,
Wolfgang Keller, Jungwoo Kim, Jonathan Kimmitt, Eddie Kohler, Vadim
Kolontsov, Austin Liew, l0stman, Pavan Maddamsetti, Imbar Marinescu,
Yandong Mao, Matan Shabtay, Hitoshi Mitake, Carmi Merimovich, Mark
Morrissey, mtasm, Joel Nider, Hayato Ohhashi, OptimisticSide,
phosphagos, Harry Porter, Greg Price, RayAndrew, Jude Rich, segfault,
Ayan Shafqat, Eldar Sehayek, Yongming Shen, Fumiya Shigemitsu, snoire,
Taojie, Cam Tenny, tyfkda, Warren Toomey, Stephen Tu, Alissa Tung,
Rafael Ubal, Amane Uehara, Pablo Ventura, Xi Wang, WaheedHafez,
Keiichi Watanabe, Lucas Wolf, Nicolas Wolovick, wxdao, Grant Wu, x653,
Jindong Zhang, Icenowy Zheng, ZhUyU1997, and Zou Chang Wei.
</p>

<p align="justify">
The code in the files that constitute xv6 is Copyright 2006-2024 Frans Kaashoek, Robert Morris, and Russ Cox.
</p>

**ERROR REPORTS**

<p align="justify">
Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu).  The main purpose of xv6 is as a teaching
operating system for MIT's 6.1810, so we are more interested in
simplifications and clarifications than new features.
</p>

**BUILDING AND RUNNING XV6**

<p align="justify">
You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu.  Once they are installed, and in your shell
search path, you can run "make qemu".
</p>