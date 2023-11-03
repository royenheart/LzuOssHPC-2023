#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <string>
#include <filesystem>
#include <cstring>
extern "C" {
    #include <limits.h>
}

#define DATA_MIN -100.0
#define DATA_MAX 100.0

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <m> <n> <p> [folder]" << std::endl;
        std::cerr << "若不指定 [folder]，则数据默认生成在当前目录" << std::endl;
        return EXIT_FAILURE;
    }
    size_t m = std::stoull(argv[1]);
    size_t n = std::stoull(argv[2]);
    size_t p = std::stoull(argv[3]);
    size_t msize[3] = {m, n, p};
    std::string folder = "";
    if (argc >= 5) {
        folder = argv[4];
        folder += "/";
        std::filesystem::create_directories(folder);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(DATA_MIN, DATA_MAX);

    std::stringstream ss;
    ss << "matrix-" << m << "-" << n << "-" << p;
    auto dataset_base = ss.str();
    // 创建 10 个数据为一组数据集
    for (int t = 0; t < 10; t++) {
        double* matrix1 = static_cast<double*>(operator new(m * n * sizeof(double)));
        double* matrix2 = static_cast<double*>(operator new(n * p * sizeof(double)));
        for (size_t e = 0; e < m * n; e++) {
            matrix1[e] = dis(gen);
        }
        for (size_t e = 0; e < n * p; e++) {
            matrix2[e] = dis(gen);
        }
        // 创建数据
        ss.str("");
        ss << folder << dataset_base << "." << t << ".bin";
        auto dataset = ss.str();
        std::ifstream checkFile(dataset);
        if (checkFile) {
            checkFile.close();
            if (std::remove(dataset.c_str()) != 0) {
                std::cerr << "文件无法删除：" << dataset << std::endl;
                return EXIT_FAILURE;
            }
            std::cout << "删除文件 " << dataset << std::endl;
        }
        std::ofstream createFile(dataset, std::ios::binary);
        if (!createFile) {
            std::cerr << "无法创建文件 " << dataset << std::endl;
            return EXIT_FAILURE;
        }
        // 写入矩阵数据
        createFile.write(reinterpret_cast<const char*>(msize), sizeof(msize));
        createFile.write(reinterpret_cast<const char*>(matrix1), m * n * sizeof(double));
        createFile.write(reinterpret_cast<const char*>(matrix2), n * p * sizeof(double));
        // 计算矩阵正确答案
        double* answer = static_cast<double*>(operator new(m * p * sizeof(double)));
        for (size_t init = 0; init < m * p; init++) {
            answer[init] = 0;
        }
        for (size_t i = 0; i < m; i++) {
            for (size_t j = 0; j < p; j++) {
                for (size_t k = 0; k < n; k++) {
                    answer[i * p + j] += matrix1[i * n + k] * matrix2[k * p + j];
                }
            }
        }
        createFile.write(reinterpret_cast<const char*>(answer), m * p * sizeof(double));
        createFile.close();
        std::cout << "创建文件 " << dataset << std::endl;
        delete[] matrix1;
        delete[] matrix2;
        delete[] answer;
    }

    return EXIT_SUCCESS;
}