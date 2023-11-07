# SCI-4

## Pre

在做本题前，你得先完成 [SCI-3](./sci-3.md) 题目。本题将基于上题基础上进行。

上题我们做的是上层算法的优化，本题你将被要求进行进一步的性能分析、优化，使用一些专业的分析工具来帮助你逐步探索性能剖析的奥妙。

本题我们不要求做的多深，大概了解一下也是好的。

## Problems

> 掌握 Intel Vtune / perf 的使用，使用 Nsight Systems 进行 GPU 性能分析，进一步深化编译器、MPI 相关优化方法

首先你得先做好上一题的基本编译步骤，针对你编译出来的程序进行分析。

### CPU 微架构性能分析与优化

在上层算法优化做好后，若你是 CPU 程序，便可以进行 CPU 微架构性能分析和优化。在这之前，我们推荐你安装 `Linux perf` 工具，一个可能的安装方法（Ubuntu）：

```bash
sudo apt install linux-tools-`uname -r` \
    linux-cloud-tools-`uname -r` \
    linux-tools-common \
    linux-cloud-tools-common
```

若使用 perf 时遇到权限问题，根据报错提示来即可。

若你是 Intel CPU，安装 [Intel Vtune](https://www.intel.com/content/www/us/en/developer/tools/oneapi/vtune-profiler.html) 分析工具。

> 提示：可以了解一下 Intel 的 [oneAPI](https://www.intel.com/content/www/us/en/developer/articles/technical/oneapi-what-is-it.html#gs.0jr7t6) 工具集，该工具集包含了 Intel 编译器、MKL 数学库、Vtune 分析工具、人工智能相关加速库等。居家必用👍。

接下来，我希望你通过 `perf stat` `perf record && perf report` 等对各个程序进行初步的分析，比如缓存命中率、执行时间等，并得到程序的热点函数、重点执行路径等。

然后，在你的精力允许的情况下，试用 Intel Vtune 进行可视化全面分析，我们不要求你完全看懂各个指标，给个初步的理解分析即可。

> 提示：使用 perf 时，你可能需要在编译程序时加上 `-g` 参数添加调试信息。
 
> 提示：Intel Vtune 有可视化界面、也可以命令行输出报告。
 
> 提示：[Perf Wiki](https://perf.wiki.kernel.org/index.php/Main_Page)

> 提示：[Intel Vtune 用户手册](https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/2023-0/overview.html)

---

**报告要求：**

**1. 上一题的报告，以及你添加调试信息的编译命令。**

**2. perf、vtune 命令使用解释、各个程序分析时使用的命令。**

**3. 命令执行结果、分析结果截图。**

**4.（可选）根据自己的理解和相关文档的阅读，对分析结果的初步解读。**

**5.（可选）你可以根据分析结果来进行对应的优化，包括源代码、编译器等优化。推荐了解下 TMA 模型分析方法（自顶向下的 CPU 架构性能瓶颈分析方法）。**

**6.（可选）[火焰图](https://github.com/brendangregg/FlameGraph)是一个常见的分析热点函数的工具，该工具可以可视化程序运行过程中的函数调用、运行占用情况。请你了解一下这个工具，并尝试针对一个程序的运行作出对应的火焰图。**

---

### 编译器相关优化

GCC、Clang/LLVM、Intel 的 C/C++ 编译器、华为的 BiSheng 编译器，这些名字若你做了上述题目应该有所耳闻。实际上，在 HPC 领域，通过换用编译器进行加速是家喻户晓的事情。现代编译器也提供了很多加速的可能，例如 PGO 基于剖析文件的编译优化、自动向量化、函数内联等都是现代编译器提供的强大功能。

我们希望你首先在 [SCI-3](./sci-3.md) 题目的基础上，对各个程序编译时分别启用 `-O0` `-O3` 优化选项，然后比较性能。

> 提示：`-O3` 是较为常见的编译优化参数，此外，针对指定平台的优化 `-march` `-mtune`（不同编译器可能选项有些许区别），Clang/LLVM 的 `-flto` 也是一些常见的优化参数，条件允许的情况下可以使用并进行比较分析。

若精力允许，请你使用至少两种编译器（可以是 GCC 和 [Intel C/C++ 编译器](https://www.intel.com/content/www/us/en/developer/tools/oneapi/dpc-compiler.html#gs.0jryeg)）分别编译程序的两个版本，并进行性能比较。

> 提示：Intel C/C++ 编译器老版本是 icc/icpc，新版本则是 icx，需要注意，并不是编译器越新越好，编译器也有 BUG，有些新版本编译器在一些软件中并没有充分被验证。

> 提示：一些编译器可以开启编译器优化报告，比如 Clang/LLVM 通过 `-Rpass` 等输出报告。该报告可以帮助你查看源代码具体如何被优化，并进行进一步分析。

> 提示：有一些在线编译网站，可以方便地帮助你查看编译前和编译后汇编的对应关系，如 [godbolt](https://godbolt.org/)

---

**报告要求：**

**1. 使用的编译器版本、可能的对应下载地址等。**

**2. 开启 `-O0` `-O3` 后的性能分析比较。**

**3.（可选）不同编译器的性能分析比较。**

**4.（可选）查看编译器官方手册，了解更多编译优化参数，尝试使用几个并进行性能分析比较。**

---

### （拓展、可选）GPU 性能分析优化

本题要求不多，使用 NVIDIA 的 [Nsight Systems](https://developer.nvidia.com/nsight-systems) 分析工具对 [SCI-3](./sci-3.md) 的相关 CUDA 程序进行性能分析，简单理解下工具生成的各个报告并写一个简短的理解。

---

**报告要求：**

**1. Nsight Systems 使用的版本**

**2. 生成的分析报告的截图，Nsight Systems 命令使用的截图**

**3. 对分析报告的简短理解**

---

### （拓展、可选）MPI 优化

和编译器相关优化一样，使用不同 MPI 实现（推荐 OPENMPI 和 [Intel MPI](https://www.intel.com/content/www/us/en/developer/tools/oneapi/mpi-library.html)）分别对 [SCI-3](./sci-3.md) 中的 MPI 程序进行编译并进行性能比较。NVIIDA HPC SDK 中也有其特定优化的 MPI 实现（基于 OPENMPI）。

> 提示：Intel MPI 的参数格式和其他 MPI 有较大的差别，建议多看其官方用户手册。

> 提示：mpiicc != mpicc，可以使用 `mpiicc --version` 和 `mpicc --version` 查看区别。

---

**报告要求：**

**1. OPENMPI 和 Intel MPI 的版本，如果 OPENMPI 你是自己编译的，给出编译脚本或者其他可复现的方法描述。**

**2. 不同 MPI 实现编译的性能比较。**

**3.（可选）使用 IPM 工具进行 MPI 运行时分析，IPM 工具通过插桩实现，可以参考 NVIDIA HPC-X 的[教程](https://hpcadvisorycouncil.atlassian.net/wiki/spaces/HPCWORKS/pages/2910060545/Profiling+using+IPM+and+HPC-X)。**

**4.（拓展、可选）如果你有多台互连的机器（可以通过虚拟机实现），尝试一下用 [SCI-3](./sci-3.md) 中的 MPI 程序进行多机运行。**

**5.（拓展、可选）MPI 标准以及其具体实现（如 OPENMPI 等）是 HPC 中不可不品鉴的东西，出题人认为其横跨了整个计算机体系结构，是 HPC 中的 “工业明珠” 之一。我们不要求你对其有相当深入的了解，但请查阅相关资料，简单描述下 MPI 的基本实现思路（可以拿 OPENMPI 为例），软件体系结构，以及其是如何处理多种网络通信的。**

---

## After

推荐阅读书籍：

1. 《CPU 性能分析与优化》