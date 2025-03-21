#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include "nnf.hpp"
#include "pdac.hpp"

#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/binomial_distribution.hpp>

#include <boost/multiprecision/gmp.hpp>

using namespace boost::random;
using boost::multiprecision::mpf_float;
using boost::multiprecision::mpz_int;

void reservoir(PDAC const& pdac, int const N, int const k) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    random_device rng;
    mt19937 mt(rng);
    uniform_int_distribution<mpz_int> ui(1, pc);

    std::set<mpz_int> visited;
    std::vector<mpz_int> reservoir;
    mpz_int tmc = 0;

    for(int i = 0; i < k && visited.size() < pc; i++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);
        auto l = ui(mt);
        while(visited.find(l) != visited.end()) {
            l = ui(mt);
        }

        path.clear();
        apnnf.get_path(l, path);
        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT);
        tmc += ai;

        if(reservoir.size() < N) {
            while(reservoir.size() < N) {
                reservoir.push_back(l);
            }
        }
        else {
            // binomial_distribution<int, long double> binom(N, static_cast<long double>(ai / tmc));
            // int const tn = binom(mt);
            uniform_int_distribution<mpz_int> lui(1, tmc);
            for(int i = 0; i < reservoir.size(); i++) {
                if(lui(mt) <= ai) {
                    reservoir[i] = l;
                }
            }
            // std::random_shuffle(reservoir.begin(), reservoir.end());
            // for(int i = 0; i < tn; i++) {
            //     reservoir[i] = l;
            // }
        }

    }

    for(int i = 0; i < reservoir.size(); i++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);
        auto const l = reservoir[i];

        path.clear();
        apnnf.get_path(l, path);
        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT);

        uniform_int_distribution<mpz_int> ui(1, ai);
        auto const id = ui(mt);
        aunnf.get_solution(id, path);
        for(auto const& l : path) {
            std::cout << l << " ";
        }
        std::cout << "0\n";
    }
}


int main(int argc, char** argv) {
    if(argc != 4) {
        std::cerr << "usage:\n";
        std::cerr << "sampler <cnf file> <number of samples> <buffer size>\n";
        exit(0);
    }
    std::string const cnf_path(argv[1]);

    int const N = std::stoi(argv[2]);
    int const k = std::stoi(argv[3]);

    auto pdac = pdac_from_file(cnf_path);

    reservoir(pdac, N, k);

    return 0;
}
