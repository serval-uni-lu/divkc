//
// Created by oz on 2/3/22.
//

#include<iostream>

#include "nnf.hpp"
#include "pdac.hpp"


int main(int argc, char** argv) {
    std::string const cnf_path(argv[1]);
    auto pdac = pdac_from_file(cnf_path);

    //appmc(pdac, 1000, 0.01);
    ksampler(pdac, 10, 10);

    return 0;
}
