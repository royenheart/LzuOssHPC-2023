#include <iostream>
#include <string>
#include <chrono>
#include <cstring>
#include "read_data.hpp"
#include "cuda_runtime.h"

__global__ void matrixMultiply(double *m1, double* m2, double *answer, size_t m, size_t n, size_t p) {
    size_t row = blockIdx.y * blockDim.y + threadIdx.y;
    size_t col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < m && col < p) {
        double sum = 0;
        for (size_t k = 0; k < n; k++) {
            sum += m1[row * n + k] * m2[k * p + col];
        }

        answer[row * p + col] = sum;
    }
}

int main(int argc, char* argv[]) {
    std::string folder = "./";
    if (argc >= 2) {
        folder = argv[1];
    }
    auto files = get_files(folder);
    if (files.has_value()) {
        for (const auto& level : files.value()) {
            auto msize = level.first;
            size_t m = std::get<0>(msize);
            size_t n = std::get<1>(msize);
            size_t p = std::get<2>(msize);
            std::cout << "测试矩阵规模：" << m << " " << n << " " << p << std::endl;
            std::cout << "每个数据测量 5 次" << std::endl;
            std::cout << "数据测量交替执行" << std::endl;
            for (int times = 0; times < 5; times++) {
                for (const auto& file : level.second) {
                    auto matrixs = get_matrixs(file);
                    if (matrixs.has_value()) {
                        auto tup = matrixs.value();
                        double* m1 = std::get<0>(tup);
                        double* m2 = std::get<1>(tup);
                        double* m3 = std::get<2>(tup);

                        double* answer = static_cast<double*>(operator new(m * p * sizeof(double)));
                        for (size_t init = 0; init < m * p; init++) {
                            answer[init] = 0;
                        }

                        double *dm1, *dm2, *danswer;
                        //============= CUDA =============
                        auto start = std::chrono::high_resolution_clock::now();

                        cudaMalloc((void**)&dm1, m * n * sizeof(double));
                        cudaMalloc((void**)&dm2, n * p * sizeof(double));
                        cudaMalloc((void**)&danswer, m * p * sizeof(double));

                        cudaMemcpy(dm1, m1, m * n * sizeof(double), cudaMemcpyHostToDevice);
                        cudaMemcpy(dm2, m2, n * p * sizeof(double), cudaMemcpyHostToDevice);

                        dim3 grid((p + 15) / 16, (m + 15) / 16);
                        dim3 block(16, 16);

                        matrixMultiply<<<grid, block>>>(dm1, dm2, danswer, m, n, p);

                        cudaMemcpy(answer, danswer, m * p * sizeof(double), cudaMemcpyDeviceToHost);

                        auto end = std::chrono::high_resolution_clock::now();
                        //============= CUDA =============

                        // 验证
                        size_t ok = 0;
                        for (; ok < m * p; ok++) {
                            if (std::abs(answer[ok] - m3[ok]) >= EPSILON) {
                                break;
                            }
                        }

                        if (ok != m * p) {
                            std::cerr << file << " 第 " << times << " 次验证失败" << std::endl;
                            std::cerr << "出错位置：" << ok << std::endl;  
                            std::cerr << "output[" << ok << "] = " << answer[ok] << std::endl;
                            std::cerr << "answer[" << ok << "] = " << m3[ok] << std::endl;
                            print_matrix_data(m, n, m1);
                            print_matrix_data(n, p, m2);
                            print_matrix_data(m, p, m3);
                            print_matrix_data(m, p, answer);
                        } else {
                            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                            std::cout << file << " 第 " << times << " 次耗时: " << duration.count() / 1000000.0 << " 秒" << std::endl;
                        }

                        delete[] m1;
                        delete[] m2;
                        delete[] m3;
                        delete[] answer;
                        cudaFree(dm1);
                        cudaFree(dm2);
                        cudaFree(danswer);
                    } else {
                        return EXIT_FAILURE;
                    }
                }
            }

        }
        std::cout << "测量结束" << std::endl;
    } else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}