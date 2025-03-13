#include<iostream>

#include "nnf.hpp"
#include "pdac.hpp"


int main(int argc, char** argv) {
    if(argc != 4) {
        std::cerr << "usage:\n";
        std::cerr << "sampler <cnf file> <N> <k>\n";
        exit(0);
    }
    std::string const cnf_path(argv[1]);
    auto pdac = pdac_from_file(cnf_path);

    int const N = std::stoi(argv[2]);
    int const k = std::stoi(argv[3]);

    ksampler(pdac, N, k);

    return 0;
}
