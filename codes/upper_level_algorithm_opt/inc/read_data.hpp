#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// 误差精度
#define EPSILON 0.0001

// 自定义哈希函数
struct TupleHash {
    std::size_t operator()(const std::tuple<size_t, size_t, size_t>& t) const {
        return std::hash<size_t>{}(std::get<0>(t)) ^ std::hash<size_t>{}(std::get<1>(t)) ^ std::hash<size_t>{}(std::get<2>(t));
    }
};

// 自定义相等性比较函数
struct TupleEqual {
    bool operator()(const std::tuple<size_t, size_t, size_t>& t1, const std::tuple<size_t, size_t, size_t>& t2) const {
        return (std::get<0>(t1) == std::get<0>(t2)) && (std::get<1>(t1) == std::get<1>(t2)) && (std::get<2>(t1) == std::get<2>(t2));
    }
};

/**
 * @brief 获取目录内数据文件并进行分组
 * 
 * @param folder 目录名称
 * @return std::optional<std::unordered_map<std::tuple<size_t, size_t, size_t>, std::vector<std::string>, TupleHash, TupleEqual>> 
 */
std::optional<std::unordered_map<std::tuple<size_t, size_t, size_t>, std::vector<std::string>, TupleHash, TupleEqual>> get_files(const std::string& folder) {
    std::unordered_map<std::tuple<size_t, size_t, size_t>, std::vector<std::string>, TupleHash, TupleEqual> map;
    std::regex pattern("matrix-([0-9]+)-([0-9]+)-([0-9]+)\\.([0-9]+)\\.bin");
    std::smatch matches;
    try {
        std::filesystem::path dir(folder);

        if (std::filesystem::is_directory(folder)) {
            for (const auto& entry: std::filesystem::directory_iterator(folder)) {
                if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                    auto filename = entry.path().filename().string();
                    if (std::regex_search(filename, matches, pattern)) {
                        if (matches.size() == 5) {
                            size_t m = std::stoull(matches[1]);
                            size_t n = std::stoull(matches[2]);
                            size_t p = std::stoull(matches[3]);
                            auto key = std::make_tuple(m, n, p); 
                            map[key].push_back(std::filesystem::absolute(entry));
                        }
                    }
                }
            }
        } else {
            std::cerr << "指定目录 " << folder << " 不是有效目录" << std::endl;
            return std::nullopt;
        }
        return std::optional(map);
    } catch(const std::exception& e) {
        std::cerr << "发生错误" << e.what() << std::endl;
        return std::nullopt;
    }
}

/**
 * @brief 根据数据名获取内部矩阵数据
 * 
 * @param file 
 * @return std::optional<std::tuple<double*, double*, double*>> 
 */
std::optional<std::tuple<double*, double*, double*>> get_matrixs(const std::string& file) {
    std::ifstream datas(file);
    if (datas) {
        size_t msize[3];
        datas.read(reinterpret_cast<char*>(msize), sizeof(msize));
        size_t m = msize[0];
        size_t n = msize[1];
        size_t p = msize[2];
        double* matrix1 = static_cast<double*>(operator new(m * n * sizeof(double)));
        double* matrix2 = static_cast<double*>(operator new(n * p * sizeof(double)));
        double* matrix3 = static_cast<double*>(operator new(m * p * sizeof(double)));
        datas.read(reinterpret_cast<char*>(matrix1), m * n * sizeof(double));
        datas.read(reinterpret_cast<char*>(matrix2), n * p * sizeof(double));
        datas.read(reinterpret_cast<char*>(matrix3), m * p * sizeof(double));
        datas.close();
        return std::optional(std::make_tuple(matrix1, matrix2, matrix3));
    } else {
        std::cerr << "无法打开文件 " << file << std::endl;
        return std::nullopt;
    }
}

void print_matrix_data(size_t m, size_t n, double* matrix) {
    std::cout << std::endl;
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            std::cout << matrix[i * n + j] << " ";
        }
        std::cout << std::endl;
    }
}