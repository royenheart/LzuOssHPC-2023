#!/bin/bash

#-------------------
# Author: RoyenHeart
# Date: 2023.11.3
#-------------------

# DEBUG
# set -x
set -e

ROOT_PATH=`cd $(dirname $0) && pwd`
SCRIPT_NAME=`basename $0`
SCRIPT=$ROOT_PATH/`basename $0`
cd ${ROOT_PATH}

m0D="${ROOT_PATH}/out/m0"
m1D="${ROOT_PATH}/out/m1"
m2D="${ROOT_PATH}/out/m2"
m3D="${ROOT_PATH}/out/m3"
m4D="${ROOT_PATH}/out/m4"
m5D="${ROOT_PATH}/out/m5"

mkdir -p ${m0D} ${m1D} ${m2D} ${m3D} ${m4D} ${m5D}

function help() {
    echo "${SCRIPT_NAME} clean       - clean all targets"
    echo "             help     - print help page"
    echo "             m0       - make m0"
    echo "             m1       - make m1" 
    echo "             m2       - make m2" 
    echo "             m3       - make m3" 
    echo "             m4       - make m4" 
    echo "             m5       - make m5" 
}

function parse_params() {
    if [[ $# -eq 0 ]]; then
        help
    fi
    while [[ $# -ne 0 ]]; do
        case $1 in
            "help")
                help
                shift
                ;;
            "clean")
                clean
                echo "make clean"
                shift
                ;;
            "m0")
                m0
                echo "make m0"
                shift
                ;;
            "m1")
                m1
                echo "make m1"
                shift
                ;;
            "m2")
                m2
                echo "make m2"
                shift
                ;;
            "m3")
                m3
                echo "make m3"
                shift
                ;;
            "m4")
                m4
                echo "make m4"
                shift
                ;;
            "m5")
                m5
                echo "make m5"
                shift
                ;;
            *)
                echo "$1 target not defined"
                shift
                ;;
        esac
    done
}

function clean() {
    rm -f ${m0D}/* ${m1D}/* ${m2D}/* ${m3D}/* ${m4D}/* ${m5D}/* 
}

function m0() {
    echo "========"
    echo "单文件编译"
    cd ${ROOT_PATH}/step1
    g++ hello.cpp -o ${m0D}/hello
    echo "========"
}

function m1() {
    echo "=============================="
    echo "全部编译成目标文件，最后直接进行链接"
    cd ${ROOT_PATH}/step2
    g++ src/main.cpp -o ${m1D}/main.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ src/particle.cpp -o ${m1D}/particle.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c 
    g++ src/v3.cpp -o ${m1D}/v3.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ ${m1D}/main.o ${m1D}/particle.o ${m1D}/v3.o -o ${m1D}/main
    echo "=============================="
}

function m2() {
    echo "================="
    echo "Statically linked"
    cd ${ROOT_PATH}/step2
    g++ src/main.cpp -o ${m2D}/main.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c 
    g++ src/particle.cpp -o ${m2D}/libparticle.so -shared -fPIC -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native 
    g++ src/v3.cpp -o ${m2D}/libv3.so -shared -fPIC -g -ggdb -O0 -std=c++17 -Wall -I./inc -march=native
    g++ ${m2D}/main.o ${m2D}/libparticle.so ${m2D}/libv3.so -o ${m2D}/main
    echo "================="
}

function m3() {
    echo "==========="
    echo "Shared Libs"
    cd ${ROOT_PATH}/step2
    g++ src/main.cpp -o ${m3D}/main.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ src/particle.cpp -o ${m3D}/particle.o -fPIC -shared -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ src/v3.cpp -o ${m3D}/v3.o -fPIC -shared -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c 
    g++ ${m3D}/v3.o -o ${m3D}/libv3.so -fPIC -shared
    g++ ${m3D}/particle.o -o ${m3D}/libparticle_rpath-link.so -fPIC -shared -Wl,-L${m3D} -Wl,-rpath-link=${m3D} -lv3
    g++ ${m3D}/particle.o -o ${m3D}/libparticle_rpath.so -fPIC -shared -Wl,-L${m3D} -Wl,-rpath=${m3D} -lv3
    g++ ${m3D}/particle.o -o ${m3D}/libparticle.so -fPIC -shared -Wl,-L${m3D} -lv3
    g++ ${m3D}/main.o -o ${m3D}/main_rpath-link -Wl,-L${m3D} -Wl,-rpath-link=${m3D} -lparticle_rpath-link -lv3 
    g++ ${m3D}/main.o -o ${m3D}/main_rpath -Wl,-L${m3D} -Wl,-rpath=${m3D} -lparticle_rpath -lv3 
    g++ ${m3D}/main.o -o ${m3D}/main -Wl,-L${m3D} -lparticle -lv3 
    echo "==========="
}

function m4() {
    echo "==========="
    echo "Static Libs"
    cd ${ROOT_PATH}/step2
    g++ src/main.cpp -o ${m4D}/main.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ src/particle.cpp -o ${m4D}/particle.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ src/v3.cpp -o ${m4D}/v3.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c 
    ar crv ${m4D}/libv3.a ${m4D}/v3.o
    ar crv ${m4D}/libparticle.a ${m4D}/particle.o
    ranlib ${m4D}/libv3.a
    ranlib ${m4D}/libparticle.a
    g++ ${m4D}/main.o -o ${m4D}/main -static -Wl,-L${m4D} -lparticle -lv3 
    g++ ${m4D}/main.o -o ${m4D}/main_1 -Wl,-L${m4D} -lparticle -lv3
    echo "==========="
}

function m5() {
    echo "==============================="
    echo "Use both Shared and Static Libs"
    cd ${ROOT_PATH}/step3
    g++ src/mixed_main.cpp -o ${m5D}/mixed_main.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ src/mixed_a.cpp -o ${m5D}/mixed_a.o -fPIC -shared -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ src/mixed_b.cpp -o ${m5D}/mixed_b.o -g -ggdb -O0 -std=c++17 -I./inc -Wall -march=native -c
    g++ ${m5D}/mixed_a.o -o ${m5D}/libmixed_a.so -fPIC -shared
    ar crv ${m5D}/libmixed_b.a ${m5D}/mixed_b.o
    ranlib ${m5D}/libmixed_b.a
    g++ ${m5D}/mixed_main.o -o ${m5D}/mixed_main -Wl,-L${m5D} -lmixed_a -lmixed_b
    g++ ${m5D}/mixed_main.o -o ${m5D}/mixed_main_v1 -Wl,-L${m5D} -lmixed_a -Bstatic -lmixed_b 
    g++ ${m5D}/mixed_main.o -o ${m5D}/mixed_main_v2 -Wl,-L${m5D} -lmixed_a -l:libmixed_b.a 
    echo "==============================="
}

parse_params $@