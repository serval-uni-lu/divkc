//
// Created by oz on 2/3/22.
//

#include<iostream>

#include "var.hpp"
#include "nnf.hpp"
#include "pdac.hpp"

#include <fstream>
#include <cmath>
#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/multiprecision/gmp.hpp>

using namespace boost::random;
using boost::multiprecision::mpf_float;

/**
 * \brief An implementation of an algorithm which computes a lower bound to the true model count.
 * \details
 * \verbatim
   @inproceedings{gomes2007sampling,
      title={From Sampling to Model Counting.},
      author={Gomes, Carla P and Hoffmann, J{\"o}rg and Sabharwal, Ashish and Selman, Bart},
      booktitle={IJCAI},
      volume={2007},
      pages={2293--2299},
      year={2007}
   }
 \endverbatim
 */
mpf_float lmca(PDAC const& pdac, int const N, int const k, double const alpha) {
    mpf_float res = -1;

    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    random_device rng;
    mt19937 mt(rng);
    uniform_int_distribution<mpz_int> ui(1, pc);

    #pragma omp parallel for
    for(int i = 0; i < N; i++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);
        mpf_float mean = 0;

        for(int j = 0; j < k; j++) {
            mpz_int l;
            #pragma omp critical
            l = ui(mt);

            path.clear();
            apnnf.get_path(l, path);
            aunnf.set_assumps(path);
            aunnf.annotate_mc();
            mpf_float ai = (aunnf.mc(ROOT) * pc);
            // ai *= pow(2, alpha);
            mean = mean + ((ai - mean) / (j + 1));
        }
        mean *= pow(2, -1 * alpha);

        #pragma omp critical
        if(res == -1 || mean < res) {
            res = mean;
        }
    }
    return res;
}

int main(int argc, char** argv) {
    if(argc != 5) {
        std::cerr << "usage:\n";
        std::cerr << "lmc <cnf file> <number of tries> <number of samples per try> <alpha>\n";
        exit(0);
    }
    std::string const cnf_path(argv[1]);
    int const N = std::stoi(argv[2]);
    int const k = std::stoi(argv[3]);
    int const alpha = std::stod(argv[4]);

    auto pdac = pdac_from_file(cnf_path);
    // auto res = lmc(pdac, N, alpha);
    auto res = lmca(pdac, N, k, alpha);

    std::cout << res << "\n";

    return 0;
}
