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
        #         -lgmp
        #         -lboost_random
        ${ASAN}
)
add_link_options(
        ${ASAN}
        -pthread
        -O3
        #         -lgmp
        #         -lboost_random
)

set(FL
    src/nnf.hpp
    src/var.hpp

    src/nnf.cpp
    src/var.cpp
)

# add_executable(ddnnf
#         src/main.cpp
#         ${FL})

add_executable(cppddnnf
        src/main.cpp
        ${FL})
target_link_libraries(cppddnnf -static gmp boost_random)
