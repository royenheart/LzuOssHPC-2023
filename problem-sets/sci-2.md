# SCI-2

## Pre

在开始这道题前，我们推荐你先学会基本的 Linux 使用方法，以及拥有阅读简单英文文档的能力。

HPC 中，我们经常使用 [Environment Modules](https://modules.readthedocs.io/en/latest/) 来管理环境变量。

这里有一个简明扼要的关于 Environment Modules 的[教程](https://zhuanlan.zhihu.com/p/50725572)。

## Problems

> 要求：以 C/C++ 语言为例，学会如何去编译简单程序和 C/C++ 项目。

### 编译器基础使用

在 HPC 中，编译器的使用是基础中的基础，但也是很多时候的踩坑点：不同科学计算软件会使用各种各样的依赖库，而软件本身可能是由 Makefile 或者 CMake 等进行自动化构建；做移植时需要你替换依赖库实现更好的性能，但是随之而来的是各式各样的报错；多核 CPU 带来的多核计算程序、异构架构如 CUDA 等、新型国产硬件平台使得各式编译器层出不穷。这些问题在你没掌握基础前都是问题，所以我们希望你能先踩一些比较基础的坑，这样才能应对 **“编译器的黄金时代”** 。

> “编译器的黄金时代” 出自 LLVM 之父 Chris Lattner，这是 [演讲视频链接](https://www.youtube.com/watch?v=4HgShra-KnY)，推荐对编译器感兴趣的同学观看。

进入 [codes/compile_first_steps](../codes/compile_first_steps/README.md) 目录，我们在里面提供了一个 Makefile 的拙劣模仿者 [compile.sh](../codes/compile_first_steps/compile.sh)，并提供了 6 个用例，请你分别测试这 6 个用例，提炼出编译的简要流程、静态库动态库的区别以及其中使用的编译参数的意义。

[codes/compile_first_steps/README.md](../codes/compile_first_steps/README.md) 文件有脚本的使用方法以及一些提示信息。

> [GCC 编译器文档](https://gcc.gnu.org/onlinedocs/gcc/)
> [CLang/LLVM 编译器文档](https://clang.llvm.org/docs/)
> 提示：诸多现代编译器提供了如自动向量化、编译器优化报告等功能，这些功能一般都可以使用其命令行参数启用。
> 提示：需要注意的编译参数： `-L`，`-I`，或者其平替：`LIBRARY_PATH`，`CPATH`。
> 提示：为确保程序正常运行，可能用到的环境变量 `LD_LIBRARY_PATH`。

---

**报告要求：**
**1. 完成 6 个测试用例编译的运行截图**
**2. 6 个测试用例内的程序运行成功的截图**
**3. 编译的简要流程**
**4. 静态库、动态库的区别**
**5. 使用的编译参数的意义**

**思考（非必要）：**
**1. 可不可以更换编译器如 Clang/LLVM 进行编译达到同样效果？**

---

#### 扩展

1. Linux 中动态库和静态库的区别？
    > 提示：Windows 中其实也有动态库和静态库的区别。
3. 你知道哪些用于调试程序、分析二进制可执行文件的工具？如果知道，简单介绍一下。
    > 提示：编译器编译时 `-g` 参数的意义。

---

**报告要求：简要回答以上问题**

---

### 自动化编译工具

[compile.sh](../codes/compile_first_steps/compile.sh) 已经相当臃肿，然而该脚本仅仅只是编译了几个程序而已，正常的 C/C++ 项目可不会这么宽容，几千个源文件的科学计算软件在 HPC 领域是家常便饭。幸好我们的前辈早已认识到 C/C++ 早期设计的缺陷，通过 Makefile、CMake 工具，我们可以更快、更普适地去构建一个大型项目。

#### Makefile

Makefile 是较早提出的针对 C/C++ 这种项目的自动化编译工具，我们在 [codes/compile_makefile](../codes/compile_makefile/Makefile) 目录存放了一个 Makefile 脚本，请你修改这个脚本，使其能够正常自动化编译出程序并执行。

编译的程序是 [codes/compile_first_steps/step2](../codes/compile_first_steps/step2/src/main.cpp) 中的。

Makefile 的教程网上相当多，建议你自行查找解决问题。

---

**报告要求：**
**1. 修改后的脚本源码**
**2. 构建出来的程序运行结果截图**
**3. 修改思路，对该 Makefile 构建程序流程进行解释**

**思考（非必要）：**
**1. 手写 Makefile 是一个痛苦的事情，除了下文提到的 CMake 等自动构建工具可以自动生成 Makefile 中间层，还有一个工具 autoconf 是很多科学计算软件常常使用的。尝试一下使用 autoconf 进行构建的科学计算软件如 netcdf、openmpi 等，看看如何用 autoconf 生成的 configure 脚本查看编译帮助、进行项目构建。谈谈 autoconf 的使用心得。**

---

#### CMake

Makefile 写起来其实也挺怪的，并且需要自行处理依赖关系，因此出现了 CMake，用来自动化生成 Makefile。现如今，CMake 还支持更多中间层比如 ninja 等。

[codes/compile_cmake](../codes/compile_cmake/CMakeLists.txt) 目录存放了一个 CMakeLists.txt 脚本，请你修改这个脚本，使其能够正常自动化编译出程序并执行。

同 Makefile 一样，CMake 的教程网上也很多，建议自行查找解决问题。

---

**报告要求：**
**1. 修改后的脚本源码**
**2. 构建出来的程序运行结果截图**
**3. 修改思路，对该 CMake 构建程序流程进行解释**

---

#### 拓展

1. 把 [codes/compile_first_steps/compile.sh](../codes/compile_first_steps/compile.sh) 改写成等价的 CMake / Makefile 脚本，要求能够覆盖到 compile.sh 中的 6 个测试用例，生成的文件名等不用完全一样。

---

**报告要求：**
**1. 修改后的脚本源码**
**2. 提供成功构建、6种测试用例执行的结果截图**
**3. 修改思路，构建流程的解释**

---

## After

推荐阅读书籍：

1. 《程序员的自我修养：链接、装载与库》