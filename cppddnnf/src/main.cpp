//
// Created by oz on 2/3/22.
//

#include<iostream>
#include <fstream>

#include "nnf.hpp"
#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/multiprecision/gmp.hpp>

#include <boost/math/distributions/normal.hpp>

using namespace boost::random;
using boost::multiprecision::mpf_float;
using boost::math::normal;

struct PDAC {
    std::set<Variable> pvar;
    NNF pnnf;
    NNF unnf;
};

PDAC pdac_from_file(std::string const& path) {
    std::set<Variable> pvar;

    std::ifstream f(path + ".proj.log");
    std::string line;
    while(getline(f, line)) {
        if(line.find("c p show ") == 0) {
            std::stringstream stream(line);
            std::string tag;
            stream >> tag >> tag >> tag;
            int v;

            while((stream >> v)) {
                if(v != 0) {
                    pvar.insert(Variable(v));
                }
            }
        }
    }
    f.close();

    PDAC res;

    res.pnnf.init(path + ".pnnf", true, pvar);
    res.unnf.init(path + ".unnf", false, pvar);
    res.pvar = pvar;

    res.pnnf.annotate_pc();

    return res;
}

int main(int argc, char** argv) {
    int const N = 10000;

    std::string const cnf_path(argv[1]);
    auto pdac = pdac_from_file(cnf_path);

    auto const pc = pdac.pnnf.mc();

    random_device rng;
    mt19937 mt(rng);
    uniform_int_distribution<mpz_int> ui(1, pc);

    std::vector<Literal> path;
    mpf_float rmean = 0;
    mpf_float rm = 0;

    for(mpz_int i = 0; i < N; i++) {
        auto l = ui(mt);

        path.clear();
        pdac.pnnf.get_path(l, path);
        pdac.unnf.set_assumps(path);
        pdac.unnf.annotate_mc();
        auto const ai = pdac.unnf.mc() * pc;

        auto k = i + 1;
        mpf_float n_mean = rmean + ((ai - rmean) / k);
        rm = rm + ((ai - rmean) * (ai - n_mean));
        rmean = n_mean;
    }

    mpf_float S2 = rm / (N - 1);
    mpf_float sd = sqrt(S2) / sqrt(N);

    mpf_float z = quantile(normal(), 1 - 0.01);

    std::cout << "pc " << pc << "\n";
    std::cout << "s " << rmean << "\n";
    std::cout << "sl " << (rmean - z * sd) << "\n";
    std::cout << "sh " << (rmean + z * sd) << "\n";

    return 0;
}
