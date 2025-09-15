## 一些常用的查看elf文件的命令

- [elf文件解析](https://ctf-wiki.org/executable/elf/structure/basic-info/)

- **基本的虚拟地址空间表**

- ![alt text](../../Picture/image.png)

- **读取可执行文件的ELF头文件信息**

```shell
readelf -h ./dlsym_strlen 
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
    // 文件标识，前四个字节 0x7F 'E' 'L' 'F' 固定，表示 ELF 文件格式
    // 后面的字节表示 ELF 版本、填充字节等

  Class:                             ELF64
    // ELF 文件类型，ELF64 表示 64 位可执行文件

  Data:                              2's complement, little endian
    // 数据存储方式：二进制补码，小端序（低地址存低位）

  Version:                           1 (current)
    // ELF 版本号，当前为 1

  OS/ABI:                            UNIX - System V
    // 目标操作系统/ABI，这里是 UNIX System V（最通用的 Linux/Unix ABI）

  ABI Version:                       0
    // ABI 版本号，一般是 0 表示默认

  Type:                              DYN (Position-Independent Executable file)
    // 文件类型：DYN 表示动态可执行文件（PIE）
    // 即编译时用了 -fPIE，运行时配合 ASLR 可以加载到任意基址

  Machine:                           Advanced Micro Devices X86-64
    // 目标体系结构，这里是 x86-64 架构（AMD64，兼容 Intel 64）

  Version:                           0x1
    // ELF 的内部版本号

  Entry point address:               0x10e0
    // 程序入口地址（_start 函数的虚拟地址）

  Start of program headers:          64 (bytes into file)
    // 程序头表（Program Header Table）的偏移，64 字节处

  Start of section headers:          14488 (bytes into file)
    // 段头表（Section Header Table）的偏移，文件内 14488 字节处

  Flags:                             0x0
    // 特定架构相关的标志位，x86-64 下一般为 0

  Size of this header:               64 (bytes)
    // ELF 头本身大小：64 字节

  Size of program headers:           56 (bytes)
    // 每个 Program Header 条目的大小：56 字节

  Number of program headers:         13
    // Program Header 的数量：13 个

  Size of section headers:           64 (bytes)
    // 每个 Section Header 条目的大小：64 字节

  Number of section headers:         31
    // Section Header 的数量：31 个

  Section header string table index: 30
    // 字符串表在 Section Header 表中的索引（30）
    // 用于保存各段的名字（.text, .data, .bss 等）
```

- **查看ELF 文件程序头表信息**

```shell
readelf -l ./dlsym_strlen 

Elf file type is DYN (Position-Independent Executable file) 
// 文件类型：动态可执行文件（PIE）

Entry point 0x10e0
// 程序入口地址，执行时从这里开始（通常对应 _start）

There are 13 program headers, starting at offset 64
// 总共有 13 个 Program Header，从文件偏移 0x40 开始

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                 0x00000000000002d8 0x00000000000002d8  R      0x8
  INTERP         0x0000000000000318 0x0000000000000318 0x0000000000000318
                 0x000000000000001c 0x000000000000001c  R      0x1
      [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000910 0x0000000000000910  R      0x1000
  LOAD           0x0000000000001000 0x0000000000001000 0x0000000000001000
                 0x0000000000000411 0x0000000000000411  R E    0x1000
  LOAD           0x0000000000002000 0x0000000000002000 0x0000000000002000
                 0x000000000000024c 0x000000000000024c  R      0x1000
  LOAD           0x0000000000002d78 0x0000000000003d78 0x0000000000003d78
                 0x0000000000000298 0x00000000000002b8  RW     0x1000
  DYNAMIC        0x0000000000002d90 0x0000000000003d90 0x0000000000003d90
                 0x0000000000000200 0x0000000000000200  RW     0x8
  NOTE           0x0000000000000338 0x0000000000000338 0x0000000000000338
                 0x0000000000000030 0x0000000000000030  R      0x8
  NOTE           0x0000000000000368 0x0000000000000368 0x0000000000000368
                 0x0000000000000044 0x0000000000000044  R      0x4
  GNU_PROPERTY   0x0000000000000338 0x0000000000000338 0x0000000000000338
                 0x0000000000000030 0x0000000000000030  R      0x8
  GNU_EH_FRAME   0x00000000000020f4 0x00000000000020f4 0x00000000000020f4
                 0x000000000000004c 0x000000000000004c  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x0000000000002d78 0x0000000000003d78 0x0000000000003d78
                 0x0000000000000288 0x0000000000000288  R      0x1
//**
    依次这样排序的:
        PHDR : 就是 Program Header Table 本身，它告诉操作系统和动态链接器如何加载 ELF 的其他部分；同时它被映射到内存中，方便运行时访问。
        INTERP : Program Interpreter 动态链接器的路径 对于静态库来说是没有，对于动态库来说是有的，静态库是直接编译进去了
        LOAD (只读数据段) : 加载到内存的第一个段，包含 ELF header 等元信息
        LOAD (代码段) : 包含代码段 .text（可执行指令）
        LOAD (只读常量) : 包含 .rodata（只读数据）和异常处理表
        LOAD (数据段) : 包含 .data（已初始化数据）、.bss（未初始化数据）、.got（全局偏移表）
        DYNAMIC ：动态链接信息表，告诉动态链接器需要加载哪些库，如何做重定位
        NOTE 段 : 存放编译信息和 ABI 信息
        GNU_PROPERTY : GNU 扩展属性，描述安全特性（如 CET、BTI、PIE 等）
        GNU_EH_FRAME : 异常处理帧表头，用于 C++ 异常、栈回溯
        GNU_STACK : 栈段权限，这里是 RW（可读写），不可执行、 提供 NX（No eXecute）保护，防止栈溢出攻击
        GNU_RELRO : RELocation Read-Only 段、 程序运行时，先 RW 修正 GOT/全局数据地址、 之后变成 RO，防止运行时被恶意篡改    
**//

Section to Segment mapping:
    Segment Sections...
        00     // PHDR 自己
        01     .interp                  // 动态链接器路径
        02     .interp .note.* .dynsym ... .rela.plt   // 动态链接相关
        03     .init .plt .plt.got .plt.sec .text .fini // 代码段
        04     .rodata .eh_frame*       // 只读数据 + 异常帧
        05     .init_array .fini_array .dynamic .got .data .bss  // 数据段
        06     .dynamic                 // 动态链接表
        07     .note.gnu.property       // NOTE 段
        08     .note.gnu.build-id .note.ABI-tag // NOTE 段
        09     .note.gnu.property       // NOTE 段（重复映射）
        10     .eh_frame_hdr            // 异常帧表
        11     // GNU_STACK，不需要 Section
        12     .init_array .fini_array .dynamic .got // RELRO 保护区域

# PHDR → ELF Program Header Table 本身（R）

# INTERP → 动态链接器路径 /lib64/ld-linux-x86-64.so.2

# LOAD (R E) → .text 代码段，可执行

# LOAD (R) → .rodata 常量，只读

# LOAD (RW) → .data + .bss + .got，读写数据

# DYNAMIC → .dynamic 段，动态链接信息表

# GNU_EH_FRAME → .eh_frame_hdr，异常处理信息

# GNU_STACK → 栈权限控制 (RW, 不可执行 NX)

# GNU_RELRO → .got 等表，启动后转为只读保护

```

- **查看ELF文件中所有段**

```cpp
readelf -S ./dlsym_strlen 
There are 31 section headers, starting at offset 0x3898:

Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .interp           PROGBITS         0000000000000318  00000318
       000000000000001c  0000000000000000   A       0     0     1
  [ 2] .note.gnu.pr[...] NOTE             0000000000000338  00000338
       0000000000000030  0000000000000000   A       0     0     8
  [ 3] .note.gnu.bu[...] NOTE             0000000000000368  00000368
       0000000000000024  0000000000000000   A       0     0     4
  [ 4] .note.ABI-tag     NOTE             000000000000038c  0000038c
       0000000000000020  0000000000000000   A       0     0     4
  [ 5] .gnu.hash         GNU_HASH         00000000000003b0  000003b0
       000000000000004c  0000000000000000   A       6     0     8
  [ 6] .dynsym           DYNSYM           0000000000000400  00000400
       00000000000001f8  0000000000000018   A       7     1     8
  [ 7] .dynstr           STRTAB           00000000000005f8  000005f8
       0000000000000130  0000000000000000   A       0     0     1
  [ 8] .gnu.version      VERSYM           0000000000000728  00000728
       000000000000002a  0000000000000002   A       6     0     2
  [ 9] .gnu.version_r    VERNEED          0000000000000758  00000758
       0000000000000050  0000000000000000   A       7     2     8
  [10] .rela.dyn         RELA             00000000000007a8  000007a8
       00000000000000f0  0000000000000018   A       6     0     8
  [11] .rela.plt         RELA             0000000000000898  00000898
       0000000000000078  0000000000000018  AI       6    24     8
  [12] .init             PROGBITS         0000000000001000  00001000
       000000000000001b  0000000000000000  AX       0     0     4
  [13] .plt              PROGBITS         0000000000001020  00001020
       0000000000000060  0000000000000010  AX       0     0     16
  [14] .plt.got          PROGBITS         0000000000001080  00001080
       0000000000000010  0000000000000010  AX       0     0     16
  [15] .plt.sec          PROGBITS         0000000000001090  00001090
       0000000000000050  0000000000000010  AX       0     0     16
  [16] .text             PROGBITS         00000000000010e0  000010e0
       0000000000000323  0000000000000000  AX       0     0     16
  [17] .fini             PROGBITS         0000000000001404  00001404
       000000000000000d  0000000000000000  AX       0     0     4
  [18] .rodata           PROGBITS         0000000000002000  00002000
       00000000000000f4  0000000000000000   A       0     0     8
  [19] .eh_frame_hdr     PROGBITS         00000000000020f4  000020f4
       000000000000004c  0000000000000000   A       0     0     4
  [20] .eh_frame         PROGBITS         0000000000002140  00002140
       000000000000010c  0000000000000000   A       0     0     8
  [21] .init_array       INIT_ARRAY       0000000000003d78  00002d78
       0000000000000010  0000000000000008  WA       0     0     8
  [22] .fini_array       FINI_ARRAY       0000000000003d88  00002d88
       0000000000000008  0000000000000008  WA       0     0     8
  [23] .dynamic          DYNAMIC          0000000000003d90  00002d90
       0000000000000200  0000000000000010  WA       7     0     8
  [24] .got              PROGBITS         0000000000003f90  00002f90
       0000000000000070  0000000000000008  WA       0     0     8
  [25] .data             PROGBITS         0000000000004000  00003000
       0000000000000010  0000000000000000  WA       0     0     8
  [26] .bss              NOBITS           0000000000004010  00003010
       0000000000000020  0000000000000000  WA       0     0     8
  [27] .comment          PROGBITS         0000000000000000  00003010
       000000000000002d  0000000000000001  MS       0     0     1
  [28] .symtab           SYMTAB           0000000000000000  00003040
       0000000000000468  0000000000000018          29    27     8
  [29] .strtab           STRTAB           0000000000000000  000034a8
       00000000000002d4  0000000000000000           0     0     1
  [30] .shstrtab         STRTAB           0000000000000000  0000377c
       000000000000011a  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), l (large), p (processor specific)

```

```md
- 特殊段
    [0] NULL
    占位符，编号 0 永远是空的（无效 section）。
    [30] .shstrtab
    Section 名字的字符串表（Section Header String Table）。所有段名（如 .text, .data）都存在这里。
- 动态链接相关的
    [1] .interp
    存放动态链接器路径（这里是 /lib64/ld-linux-x86-64.so.2），对应 INTERP program header。

    [5] .gnu.hash
    GNU hash 表，加速动态符号查找。

    [6] .dynsym
    动态符号表，保存导入导出函数和变量的符号。

    [7] .dynstr
    动态符号对应的字符串表，保存函数/变量名字。

    [8] .gnu.version
    符号版本信息表，每个动态符号对应一个版本号。

    [9] .gnu.version_r
    依赖库的版本需求表（VERNEED）。

    [10] .rela.dyn
    动态重定位表（针对数据段的地址修正）。

    [11] .rela.plt
    PLT 表的重定位信息（用于函数调用时延迟绑定）。

    [23] .dynamic
    动态链接信息表，告诉动态链接器需要加载哪些库、符号表在哪里。

    [24] .got
    Global Offset Table，全局偏移表，保存运行时重定位后的地址。
- 代码相关
    [12] .init
    程序初始化函数段（执行 main 前调用）。

    [13] .plt / [14] .plt.got / [15] .plt.sec
    Procedure Linkage Table，用于延迟解析外部函数调用。plt.got 结合 GOT 实现懒加载。

    [16] .text
    代码段，存放可执行指令。

    [17] .fini
    程序结束时执行的清理函数段（调用 exit 前）。
- 只读数据 & 异常处理
    [18] .rodata
    只读常量数据（字符串常量、const 变量等）。

    [19] .eh_frame_hdr / [20] .eh_frame
    异常处理（exception handling）栈回溯表，供 C++ 异常机制和 backtrace() 使用。
-  运行时构造/析构函数
    [21] .init_array
    初始化函数数组，存放构造函数指针（程序启动时依次执行）。

    [22] .fini_array
    析构函数数组，存放清理函数指针（程序退出时依次执行）。
-  数据段
    [25] .data
    已初始化的全局变量和静态变量。

    [26] .bss
    未初始化的全局/静态变量，运行时由内核清零
-  调试/符号信息
    [27] .comment
    编译器信息（例如 gcc 版本号）。

    [28] .symtab
    完整的符号表（包含所有函数/变量），主要用于调试。

    [29] .strtab
    符号表用的字符串表，存放符号名（symtab 的名字存放在这里）。
```

- **查看某个特定段的16进制内容或者string形式的内容段**

```bash
-x --hex-dump=<number|name>
                    Dump the contents of section <number|name> as bytes
-p --string-dump=<number|name>
                    Dump the contents of section <number|name> as strings

readelf -p .dynstr ./dlsym_strlen
 // 这个对应的全是二进制部分内容

readelf -x .text ./dlsym_strlen
String dump of section '.dynstr':

  [     1]  __gmon_start__
  [    10]  _ITM_deregisterTMCloneTable
  [    2c]  _ITM_registerTMCloneTable
  [    46]  _ZNSt8ios_base4InitD1Ev
  [    5e]  _ZNSt8ios_base4InitC1Ev
  [    76]  dlerror
  [    7e]  __cxa_finalize
  [    8d]  __libc_start_main
  [    9f]  dlsym
  [    a5]  __cxa_atexit
  [    b2]  strlen
  [    b9]  printf
  [    c0]  libstdc++.so.6
  [    cf]  libc.so.6
  [    d9]  _edata
  [    e0]  _IO_stdin_used
  [    ef]  __data_start
  [    fc]  _end
  [   101]  __bss_start
  [   10d]  GLIBCXX_3.4
  [   119]  GLIBC_2.34
  [   124]  GLIBC_2.2.5
```


## 一些反汇编的命令

```bash

```

## 查看动态链接库与静态链接库的命令

```bash

```