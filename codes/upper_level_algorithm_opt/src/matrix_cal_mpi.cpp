#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cstring>
#include <iterator>
#include "read_data.hpp"
#include "mpi.h"

int main(int argc, char* argv[]) { 
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 先由主进程进行数据初始化
    std::optional<std::unordered_map<std::tuple<size_t, size_t, size_t>, std::vector<std::string>, TupleHash, TupleEqual>> files;
    std::unordered_map<std::tuple<size_t, size_t, size_t>, std::vector<std::string>, TupleHash, TupleEqual> datasets;
    std::unordered_map<std::tuple<size_t, size_t, size_t>, std::vector<std::string>, TupleHash, TupleEqual>::iterator dataset;
    std::vector<std::string>::iterator file;
    size_t m;
    size_t n;
    size_t p;
    std::chrono::_V2::system_clock::time_point start, end;
    bool is_end = false;
    if (rank == 0) {
        std::string folder = "./";
        if (argc >= 2) {
            folder = argv[1];
        }
        files = get_files(folder);
        if (!files.has_value()) {
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        datasets = files.value();
        dataset = datasets.begin();
        // 避免一开始 dataset 就无数据
        if (dataset == datasets.end()) {
            is_end = true;
            std::cout << "计算完毕" << std::endl;
        } else {
            file = dataset->second.begin();
        }
    }
    MPI_Bcast(&is_end, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);

    while (!is_end) {
        /*
            划分思路和 OpenMP 差不多
            创建矩阵
            使用 vector 初始化默认值为 0
        */

        // 主进程迭代每个数据集以及文件，并初始化每个数据的矩阵
        if (rank == 0) {
            if (file == dataset->second.end()) {
                dataset++;
                if (dataset == datasets.end()) {
                    is_end = true;
                    std::cout << "计算完毕" << std::endl;
                } else {
                    file = dataset->second.begin();
                }
            }
            if (!is_end) {
                auto msize = dataset->first;
                m = std::get<0>(msize);
                n = std::get<1>(msize);
                p = std::get<2>(msize);
                if (file == dataset->second.begin()) {
                    std::cout << "测试矩阵规模：" << m << " " << n << " " << p << std::endl;
                    std::cout << "每个数据测量 1 次" << std::endl;
                    if (m % size != 0 || (m * n) % size != 0 || (m * p) % size != 0) {
                        std::cerr << "无法整除，更换一个进程数" << std::endl;
                        is_end = true;
                    }
                }
            }
        }

        MPI_Bcast(&is_end, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
        if (is_end) {
            break;
        }

        // 先由主进程完成 m, n, p 的获取
        // 由主进程广播 m, n, p 到所有进程
        MPI_Bcast(&m, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&n, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
        MPI_Bcast(&p, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

        // A 矩阵需要划分给各个进程，主进程分配，然后由主进程进行分发
        std::vector<double> m1(m * n);
        // 划分后的各个进程拥有的 A 矩阵的部分
        std::vector<double> m1_local(m * n / size);
        // B 矩阵所有进程一样，由主进程广播
        std::vector<double> m2(n * p);
        // 最终答案归约到一起
        std::vector<double> answer(m * p);
        // 各个进程拥有的部分答案
        std::vector<double> answer_local(m * p / size);
        // 正确答案，只需要主进程持有
        std::vector<double> m3(m * p);

        // 再由主进程完成矩阵数据的读取
        if (rank == 0) {
            auto matrixs = get_matrixs(*file);
            if (!matrixs.has_value()) {
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            auto tup = matrixs.value();
            double* rm1 = std::get<0>(tup);
            double* rm2 = std::get<1>(tup);
            double* rm3 = std::get<2>(tup);
            for (size_t i = 0; i < m * n; i++) {
                m1[i] = rm1[i];
            }
            for (size_t i = 0; i < n * p; i++) {
                m2[i] = rm2[i];
            }
            for (size_t i = 0; i < m * p; i++) {
                m3[i] = rm3[i];
            }
            delete[] rm1;
            delete[] rm2;
            delete[] rm3;
            start = std::chrono::high_resolution_clock::now();
        }

        // 由主进程广播 B 矩阵（m2）到所有进程
        MPI_Bcast(&m2[0], n * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // 由主进程分发 m1 到所有进程的 m1_local
        MPI_Scatter(&m1[0], m * n / size, MPI_DOUBLE, &m1_local[0], m * n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // 局部乘法
        for (size_t i = 0; i < m / size; i++) {
            for (size_t j = 0; j < p; j++) {
                for(size_t k = 0; k < n; k++) {
                    answer_local[i * p + j] += m1_local[i * n + k] * m2[k * p + j];
                }
            }
        }

        // 将局部结果按照次序汇总到主进程
        MPI_Gather(&answer_local[0], m * p / size, MPI_DOUBLE, &answer[0], m * p / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // 验证
        if (rank == 0) {
            end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            size_t ok = 0;
            for (; ok < m * p; ok++) {
                if (std::abs(answer[ok] - m3[ok]) >= EPSILON) {
                    break;
                }
            }

            if (ok != m * p) {
                std::cout << *file << "验证失败" << duration.count() / 1000000.0 << " 秒" << std::endl;
                std::cerr << "出错位置：" << ok << std::endl;  
                std::cerr << "output[" << ok << "] = " << answer[ok] << std::endl;
                std::cerr << "answer[" << ok << "] = " << m3[ok] << std::endl;
            } else {
                std::cout << *file << " 验证成功，耗时: " << duration.count() / 1000000.0 << " 秒" << std::endl;
            }
            file++;
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}