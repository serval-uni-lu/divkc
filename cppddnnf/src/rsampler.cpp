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

void reservoir_heuristic(PDAC const& pdac, std::size_t const N, std::size_t const k) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    random_device rng;
    mt19937 mt(rng);
    uniform_int_distribution<mpz_int> ui(1, pc);

    std::set<mpz_int> visited;
    std::vector<mpz_int> reservoir;
    mpz_int tmc = 0;

    #pragma omp parallel for
    for(std::size_t i = 0; i < k; i++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);
        mpz_int l = -1;
        #pragma omp critical
        {
            l = ui(mt);
            while(visited.find(l) != visited.end()) {
                l = ui(mt);
            }
        }

        path.clear();
        apnnf.get_path(l, path);
        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT);

        #pragma omp critical
        {
            tmc += ai;

            if(reservoir.size() < N) {
                while(reservoir.size() < N) {
                    reservoir.push_back(l);
                }
            }
            else {
                uniform_int_distribution<mpz_int> lui(1, tmc);
                for(std::size_t i = 0; i < reservoir.size(); i++) {
                    if(lui(mt) <= ai) {
                        reservoir[i] = l;
                    }
                }
            }
        }

    }

    #pragma omp parallel for
    for(std::size_t i = 0; i < reservoir.size(); i++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);
        auto const l = reservoir[i];

        path.clear();
        apnnf.get_path(l, path);
        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT);

        uniform_int_distribution<mpz_int> ui(1, ai);
        mpz_int id = -1;
        #pragma omp critical
        id = ui(mt);

        aunnf.get_solution(id, path);

        #pragma omp critical
        {
            for(auto const& l : path) {
                std::cout << l << " ";
            }
            std::cout << "0\n";
        }
    }
}

void reservoir_exact(PDAC const& pdac, std::size_t const N) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);
    std::size_t const ipc = static_cast<std::size_t>(pc);

    random_device rng;
    mt19937 mt(rng);

    std::vector<mpz_int> reservoir;
    mpz_int tmc = 0;

    #pragma omp parallel for
    for(std::size_t l = 0; l < ipc; l++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);

        path.clear();
        apnnf.get_path(l, path);
        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT);


        #pragma omp critical
        {
            tmc += ai;

            if(reservoir.size() < N) {
                while(reservoir.size() < N) {
                    reservoir.push_back(l);
                }
            }
            else {
                uniform_int_distribution<mpz_int> lui(1, tmc);
                for(std::size_t i = 0; i < reservoir.size(); i++) {
                    if(lui(mt) <= ai) {
                        reservoir[i] = l;
                    }
                }
            }
        }
    }

    #pragma omp parallel for
    for(std::size_t i = 0; i < reservoir.size(); i++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);
        auto const l = reservoir[i];

        path.clear();
        apnnf.get_path(l, path);
        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT);

        uniform_int_distribution<mpz_int> ui(1, ai);

        mpz_int id = -1;
        #pragma omp critical
        id = ui(mt);

        aunnf.get_solution(id, path);

        #pragma omp critical
        {
            for(auto const& l : path) {
                std::cout << l << " ";
            }
            std::cout << "0\n";
        }
    }
}

void rsampler(PDAC const& pdac, std::size_t const N, std::size_t const k) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    if(pc > k) {
        reservoir_heuristic(pdac, N, k);
    }
    else {
        reservoir_exact(pdac, N);
    }
}


int main(int argc, char** argv) {
    if(argc != 4) {
        std::cerr << "usage:\n";
        std::cerr << "sampler <cnf file> <number of samples> <buffer size>\n";
        exit(0);
    }
    std::string const cnf_path(argv[1]);

    std::size_t const N = std::stoull(argv[2]);
    std::size_t const k = std::stoull(argv[3]);

    auto pdac = pdac_from_file(cnf_path);

    rsampler(pdac, N, k);

    return 0;
}
