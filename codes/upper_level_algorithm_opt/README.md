# upper_level_algorithm_opt

上层算法优化

## 解决问题

经典矩阵乘法问题：

设矩阵 A：

$$
A = \left(
    \begin{array}{ccc}
        a_{11} & a_{12} & ... & a_{1n} \\
        a_{21} & a_{22} & ... & a_{2n} \\
        ... & ... & ... & ... \\
        a_{m1} & a_{m2} & ... & a_{mn} \\
    \end{array}
\right)
$$

设矩阵 B：

$$
B = \left(
    \begin{array}{ccc}
        b_{11} & b_{12} & ... & b_{1p} \\
        b_{21} & b_{22} & ... & b_{2p} \\
        ... & ... & ... & ... \\
        b_{n1} & b_{n2} & ... & b_{np} \\
    \end{array}
\right)
$$

A 矩阵是 $m * n$ 大小的矩阵，B 矩阵是 $n * p$ 大小的矩阵，两矩阵乘法结果是一个新矩阵 C，维度是 $m * p$，即 $m$ 行、$p$ 列。

矩阵乘法数学公式表示为：

$$
C_{mp} = \sum_{n=1}^n A_{mn} \cdot B_{np} 
$$

## 使用方法

* 所有源代码均使用 `C++17` 标准编写，编译时请注意。 

所有矩阵存放方式均为行优先，内部元素类型为 `double`，生成的数据集文件 `matrix-m-n-p.x.bin` 为二进制文件，包含：

1. 指定内部矩阵大小的 m, n, p 三个元素（`size_t` 类型）。
2. A 矩阵元素（`double` 类型）。
3. B 矩阵元素（`double` 类型）。
4. 串行算法计算得来的 A B 矩阵乘法的结果，用作正确答案便于后续进行验证，其元素个数为 $m * p$，类型为 `double`。

`inc` 目录存放了头文件，`src` 目录存放了源代码文件。进入 `src` 目录，首先编译 `datagen.cpp` 文件用于生成数据集，`datagen.cpp` 编译后使用方法为：

> ./datagen \<m\> \<n\> \<p\> [folder]

> m : A 矩阵行数。
> n : A 矩阵列数，B 矩阵行数。
> p : B 矩阵列数。
> folder: 可选，指定数据集生成的目录。未指定时默认生成至当前目录。默认生成一个数据集，包含 10 个指定矩阵规模的随机数据（即总共 10 组 A、B 矩阵），按照生成顺序反映在文件 `matrix-m-n-p.x.bin` 的 `x` 编号上。

指定矩阵规模为 `1000 1000 1000` 能看到较好的比较结果，你可以根据自己的机器配置和比较需求选择合适的矩阵规模。

然后在 `src` 目录下分别编译各个 `matrix_cal_xxx` 源文件，`xxx` 表示矩阵乘法解决方式，例如 `mpi_cuda` 表示使用了 MPI + CUDA。这些文件编译后使用方法相同：

> ./matrix_cal_xxx [folder]

> folder: 可选，指定使用数据集存放的目录。未指定时默认遍历当前目录下名称为 `matrix-m-n-p.x.bin` 的数据集。默认会将同一类（即判断 m，n，p 是否相同）的数据作为一个数据集集中测试，每次测试一个数据集后接到下一个。

### 提示

1. MPI 应用需要前面加上 `mpirun` 以及其他 MPI 应用启动信息（如 `-np` 等）使用。
2. 链接、查看动态库时可以使用 `ldd` 命令进行初步分析。
3. 如 GCC、LLVM 等编译器可以对源文件进行依赖分析，即用到了什么依赖（头文件）等。
4. 出题人使用的 MPI 为 OPENMPI、CUDA 使用的 NVIDIA HPC SDK 套件，OPENBLAS 为 GCC 编译版本。编译时根据自己机器的配置和需求选择合适的编译套件、依赖库进行。
5. MPI 是一个标准，有很多实现，常见的有：OPENMPI、MPICH、华为的 HyperMPI。不同实现在基本命令行参数（进程数指定等）上大致相同，但拓展指令则各有千秋。
6. OPENMPI 是一个套皮编译器，你可以使用 `mpicc --showme` 命令发现新世界。使用环境变量 `OMPI_CC` 和 `OMPI_CXX` 来改变其套皮的编译器。 
7. nvcc 和 nvc++ 两者并不完全相同，nvcc 采用的是 `split compilation model`，会将代码拆分成主机和设备版本，主机版本使用其套皮的编译器（通过 `-ccbin` 指定）编译。而 nvc++ 是一个统一的主机和设备编译器，并不需要单独的主机编译器。CUDA 程序的后缀名为 `.cu`
8. BLAS 库（基本线性代数子程序）也有很多版本，OPENBLAS、NVIDIA CUDA 的 cuBlas、LAPACK、Intel 的 MKL 都是 BLAS 库的不同实现。你可以使用动态链接库的知识编译使用 blas 库解决矩阵问题的代码的不同版本，并查看其性能。
9. 在运行 MPI、OpenMP 程序时，进程、线程亲和度绑定是一个比较重要的概念，OpenMP 的亲和度绑定环境变量为 `OMP_PLACES`、`OMP_PROC_BIND`，MPI 是 `mpirun` 的参数 `--bind-to`、`--rank-by`、`--map-by` 等。

注意，由于出题人来不及（摆了）解决 BUG，在启动 MPI 程序时，使用的数据集的 m, n, p 应满足 $m$、$m * n$、$m * p$ 能够整除进程数 $size$，否则矩阵将无法完全划分。