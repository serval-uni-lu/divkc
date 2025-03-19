//
// Created by oz on 2/3/22.
//

#include<iostream>

#include "nnf.hpp"
#include "pdac.hpp"


int main(int argc, char** argv) {
    if(argc != 4) {
        std::cerr << "usage:\n";
        std::cerr << "appmc <cnf file> <number of samples> <alpha>\n";
        exit(0);
    }
    std::string const cnf_path(argv[1]);
    int const N = std::stoi(argv[2]);
    double const alpha = std::stod(argv[3]);

    auto pdac = pdac_from_file(cnf_path);

    appmc(pdac, N, alpha);

    return 0;
}
