cmake_minimum_required(VERSION 3.10)

project(dnnf_to_dot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(ASAN -fsanitize=address -fno-omit-frame-pointer -static-libasan)

add_compile_options(
        -Wall
        -Wextra
        -pedantic
        -O3
        -pthread
        -fopenmp
        #         -lgmp
        #         -lboost_random
        ${ASAN}
)
add_link_options(
        ${ASAN}
        -pthread
        -O3
        -fopenmp
        #         -lgmp
        #         -lboost_random
)

set(FL
    src/nnf.hpp
    src/var.hpp
    src/pdac.hpp

    src/nnf.cpp
    src/var.cpp
    src/pdac.cpp
)

# add_executable(ddnnf
#         src/main.cpp
#         ${FL})

add_executable(cppddnnf
        src/main.cpp
        ${FL})
target_link_libraries(cppddnnf gmp boost_random)

add_executable(ksampler
        src/sampler.cpp
        ${FL})
target_link_libraries(ksampler gmp boost_random)
