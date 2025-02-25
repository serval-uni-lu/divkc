#include <iostream>
#include <fstream>
#include <limits>
#include <chrono>
#include <algorithm>

#include <cmath>

#include <pagmo/algorithm.hpp>
#include <pagmo/algorithms/sga.hpp>
#include <pagmo/algorithms/gaco.hpp>
#include <pagmo/bfe.hpp>
#include <pagmo/batch_evaluators/default_bfe.hpp>
#include <pagmo/population.hpp>
#include <pagmo/problem.hpp>

#include "CNF.hpp"


using namespace pagmo;

struct Prob {
    CNF cnf;

    vector_double fitness(const vector_double & v) const {
        int res = 0;

        for(auto const& c : v) {
            res += c;
        }

        if(res == 0 || res >= 0.7 * v.size()) {
            return {std::numeric_limits<double>::max()};
        }

        int nb_v = res;

        res = 0;

        for(int64_t i = 0; i < cnf.nb_clauses(); i++) {
            if(cnf.is_active(i)) {
                int tmp = 0;
                auto const& cl = cnf.clause(i);

                for(auto const& l : cl) {
                    Variable vl(l);

                    tmp += v[vl.get()];
                }

                if(tmp == cl.size()) {
                    res += 1;
                }
            }
        }
        // return {static_cast<double>(-1 * res) / nb_v};
        return {static_cast<double>(-1 * res)};
    }

    std::pair<vector_double, vector_double> get_bounds() const {
        return {vector_double(cnf.nb_vars(), 0), vector_double(cnf.nb_vars(), 1)};
    }

    vector_double::size_type get_nix() const {
        return cnf.nb_vars();
    }
};

int main(int argc, char** argv) {
    if(argc != 4) {
        std::cout << "usage: splitter <cnf> <generations> <population>\nexiting\n";
        return 0;
    }

    std::string const path(argv[1]);
    CNF cnf(argv[1]);

    int const nb_gen = std::stoi(argv[2]);
    int const pop_size = std::stoi(argv[3]);

    Prob prob;
    prob.cnf = cnf;

    problem p{prob};

    algorithm algo{sga(nb_gen)};

    // auto g = gaco{nb_gen};
    // algorithm algo{g};

    population pop{p, pop_size};

    pop = algo.evolve(pop);

    {
        std::cout << "c p show ";
        for(int i = 0; i < cnf.nb_vars(); i++) {
            if(pop.champion_x()[i] == 1) {
                std::cout << (i + 1) << " ";
            }
        }
        std::cout << "0";
    }

    return 0;
}
