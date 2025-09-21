## 参考资料

- [linux下强大的文件分析工具 -- nm](https://zhuanlan.zhihu.com/p/363014233#:~:text=nm%E5%91%BD%E4%BB%A4%E6%98%AFlinux%E4%B8%8B%E8%87%AA%E5%B8%A6%E7%9A%84%E7%89%B9%E5%AE%9A%E6%96%87%E4%BB%B6%E5%88%86%E6%9E%90%E5%B7%A5%E5%85%B7%EF%BC%8C%E4%B8%80%E8%88%AC%E7%94%A8%E6%9D%A5%E6%A3%80%E6%9F%A5%E5%88%86%E6%9E%90%20%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%96%87%E4%BB%B6%20%E3%80%81%20%E5%BA%93%E6%96%87%E4%BB%B6%20%E3%80%81%20%E5%8F%AF%E6%89%A7%E8%A1%8C%E6%96%87%E4%BB%B6,%E4%B8%AD%E7%9A%84%20%E7%AC%A6%E5%8F%B7%E8%A1%A8%EF%BC%8C%E8%BF%94%E5%9B%9E%E4%BA%8C%E8%BF%9B%E5%88%B6%E6%96%87%E4%BB%B6%E4%B8%AD%E5%90%84%E6%AE%B5%E7%9A%84%E4%BF%A1%E6%81%AF%E3%80%82%20%E9%A6%96%E5%85%88%EF%BC%8C%E6%8F%90%E5%88%B0%E8%BF%99%E4%B8%89%E7%A7%8D%E6%96%87%E4%BB%B6%EF%BC%8C%E6%88%91%E4%BB%AC%E4%B8%8D%E5%BE%97%E4%B8%8D%E6%8F%90%E7%9A%84%E5%B0%B1%E6%98%AFgcc%E7%9A%84%20%E7%BC%96%E8%AF%91%20%E6%B5%81%E7%A8%8B%EF%BC%9A%20%E9%A2%84%E7%BC%96%E8%AF%91%EF%BC%8C%E7%BC%96%E8%AF%91%EF%BC%8C%20%E6%B1%87%E7%BC%96%EF%BC%8C%E9%93%BE%E6%8E%A5%E3%80%82)
- [nm命令使用详解，让你加快学习速度](https://zhuanlan.zhihu.com/p/587881930)
- [华为nm](https://bbs.huaweicloud.com/blogs/358322)


### 用法

```shell
       nm [-A|-o|--print-file-name]
          [-a|--debug-syms]
          [-B|--format=bsd]
          [-C|--demangle[=style]]
          [-D|--dynamic]
          [-fformat|--format=format]
          [-g|--extern-only]
          [-h|--help]
          [--ifunc-chars=CHARS]
          [-j|--format=just-symbols]
          [-l|--line-numbers] [--inlines]
          [-n|-v|--numeric-sort]
          [-P|--portability]
          [-p|--no-sort]
          [-r|--reverse-sort]
          [-S|--print-size]
          [-s|--print-armap]
          [-t radix|--radix=radix]
          [-u|--undefined-only]
          [-U method] [--unicode=method]
          [-V|--version]
          [-X 32_64]
          [--defined-only]
          [--no-demangle]
          [--no-recurse-limit|--recurse-limit]]
          [--plugin name]
          [--size-sort]
          [--special-syms]
          [--synthetic]
          [--target=bfdname]
          [--with-symbol-versions]
          [--without-symbol-versions]
          [objfile...]

-A 或-o或 --print-file-name：打印出每个符号属于的文件
-a或--debug-syms：打印出所有符号，包括debug符号
-B：BSD码显示
-C或--demangle[=style]：对低级符号名称进行解码，C++文件需要添加
--no-demangle：不对低级符号名称进行解码，默认参数
-D 或--dynamic：显示动态符号而不显示普通符号，一般用于动态库
-f format或--format=format：显示的形式，默认为bsd，可选为sysv和posix
-g或--extern-only：仅显示外部符号
-h或--help：国际惯例，显示命令的帮助信息
-n或-v或--numeric-sort：显示的符号以地址排序，而不是名称排序
-p或--no-sort：不对显示内容进行排序
-P或--portability：使用POSIX.2标准
-V或--version：国际管理，查看版本
--defined-only：仅显示定义的符号，这个从英文翻译过来可能会有偏差，故贴上原文：

Display only defined symbols for each object file
```

# `nm -n -C ./polymorphism` --- : 把编译出来的可执行文件的符号表打印出来了（-n 按地址排序，-C 把 C++ 符号名解码）

把编译出来的可执行文件的**符号表**打印出来了（`-n` 按地址排序，`-C` 把 C++ 符号名解码）。
下面我帮你逐类解析一下这些符号：

---

## 1. 符号类型说明（第二列字母）

* **T/t**：在 `.text`（代码区）的函数符号（`t` 小写通常表示局部或内部）
* **W**：弱符号（weak symbol，可以被覆盖，但也能用）
* **U**：未定义符号（需要外部库提供实现，例如 `libstdc++`、`libc`）
* **R/r**：在 `.rodata`（只读数据段），常量、字符串、vtable、typeinfo 等
* **D/d**：在 `.data` 段（已初始化的全局变量）
* **B/b**：在 `.bss` 段（未初始化的全局变量）
* **V**：弱符号 + 只读对象（vtable、typeinfo 常见）
* **w**：弱符号（无地址或弱引用，可能被优化掉）

---

## 2. 你的输出里的关键内容

### (a) 系统相关（编译器/运行库自动生成）

```
w _ITM_deregisterTMCloneTable
w _ITM_registerTMCloneTable
w __gmon_start__
0000000000001000 T _init
0000000000001100 T _start
00000000000013cc T _fini
```

* 这些和 **GCC 插桩**、**初始化/结束代码**相关，比如事务内存支持 (`_ITM_`)、程序入口 `_start`。

---

### (b) 外部依赖 (U)

```
U std::ostream::operator<<(...)
U std::ios_base::Init::Init()
U operator new(unsigned long)
U __libc_start_main@GLIBC_2.34
```

* `U` 说明这些符号没在当前可执行文件实现，而是链接到 **libstdc++** 和 **glibc**。
* 例如 `operator new`、`std::cout`、`__libc_start_main`。

---

### (c) 你的函数（编译产物）

```
00000000000011e9 T main
0000000000001302 W Base::print()
0000000000001340 W Derived::print()
000000000000137e W Base::Base()
000000000000139c W Derived::Derived()
```

* `main` 是主函数，`T` 表示在 `.text` 段。
* `Base::print` 和 `Derived::print` 是虚函数实现，标成 `W`（弱符号），编译器允许虚函数被“覆盖”。
* 构造函数 `Base::Base`、`Derived::Derived` 也显示出来。

---

### (d) 只读数据（.rodata）

```
0000000000002028 V typeinfo name for Derived
0000000000002031 V typeinfo name for Base
0000000000003d28 V vtable for Derived
0000000000003d40 V vtable for Base
0000000000003d58 V typeinfo for Derived
0000000000003d70 V typeinfo for Base
```

* **typeinfo name**：存放类名的字符串（RTTI 用）。
* **vtable for XXX**：虚函数表，存放在 `.rodata` 段。
* **typeinfo for XXX**：RTTI 运行时类型信息结构体。

> 这就是 C++ 多态的核心：对象的 `vptr` 指向这里的 vtable。

---

### (e) 数据段/全局变量

```
0000000000002000 R _IO_stdin_used
0000000000004000 D __data_start
0000000000004010 B __bss_start
0000000000004040 B std::cout@GLIBCXX_3.4
```

* `_IO_stdin_used` → 标准库 IO 相关符号。
* `__data_start` / `__bss_start` → `.data` 段和 `.bss` 段的起始位置。
* `std::cout` → 全局对象，放在 `.bss` 段。

---

##### (f) 编译器自动生成的初始化逻辑

```
0000000000001293 t __static_initialization_and_destruction_0(int, int)
00000000000012e9 t _GLOBAL__sub_I_main
0000000000004151 b std::__ioinit
```

* `__static_initialization_and_destruction_0`：负责全局对象（如 `std::cout`）的构造与析构。
* `_GLOBAL__sub_I_main`：自动调用静态初始化函数。
* `std::__ioinit`：构造全局 IO 对象的辅助符号。



* 你的 **虚函数表** 在：

  ```
  0x3d28 V vtable for Derived
  0x3d40 V vtable for Base
  ```
* **虚函数实现**在 `.text` 段：

  ```
  0x1302 W Base::print()
  0x1340 W Derived::print()
  ```
* **RTTI 信息**（typeinfo）和 **类名字符串** 在 `.rodata`：

  ```
  typeinfo name for Derived/Base
  typeinfo for Derived/Base
  ```

---

* vtable 放在 **.rodata** 段（只读数据区）。
* 虚函数代码放在 **.text** 段。
* 对象里有个 **vptr** 指向对应的 vtable。


###### 基本的内存示意图

```cpp
堆上对象（p 指向这里）
+------------------+
| vptr ----------+ |   → 指向 .rodata 段里的 Derived vtable
+------------------+
| 其它成员变量     |
+------------------+

.rodata 段（全局共享）
Derived vtable:
+--------------------------+
| typeinfo for Derived     |
+--------------------------+
| &Derived::print          |
+--------------------------+

Base vtable:
+--------------------------+
| typeinfo for Base        |
+--------------------------+
| &Base::print             |
+--------------------------+

```