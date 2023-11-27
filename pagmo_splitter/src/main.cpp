#include <iostream>
#include <limits>

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
        double res = 0;

        for(auto const& c : v) {
            res += c;
        }

        if(res == 0 || res == v.size()) {
            return {std::numeric_limits<double>::max()};
        }

        res = 0;

        for(int64_t i = 0; i < cnf.nb_clauses(); i++) {
            if(cnf.is_active(i)) {
                int tmp = 0;
                auto const& cl = cnf.clause(i);

                for(auto const& l : cl) {
                    Variable vl(l);

                    tmp += v[vl.get()];
                }

                if(tmp != 0 && tmp != cl.size()) {
                    //res += 1; // some non-zero value
                    res += pow(2, 5 - cl.size());
                }
            }
        }
        return {res};
    }

    std::pair<vector_double, vector_double> get_bounds() const {
        return {vector_double(cnf.nb_vars(), 0), vector_double(cnf.nb_vars(), 1)};
    }

    vector_double::size_type get_nix() const {
        return cnf.nb_vars();
    }
};

int main(int argc, char** argv) {
    CNF cnf(argv[1]);

    Prob prob;
    prob.cnf = cnf;

    problem p{prob};

    //algorithm algo{sga(1000)};

    auto g = gaco{1000};
    algorithm algo{g};

    population pop{p, 100000};

    pop = algo.evolve(pop);

    std::cout << p ;
    int64_t sum = 0;
    for(int i = 0; i < cnf.nb_vars(); i++) {
        std::cout << "v " << (i + 1) << " " << pop.champion_x()[i] << "\n";
        sum += pop.champion_x()[i];
    }
    std::cout << "c p 0 " << cnf.nb_vars() - sum << "\n";
    std::cout << "c p 1 " << sum << "\n";

    std::cout << "s2 " << pop.champion_f()[0] << "\n";

    int64_t res = 0;
    for(int64_t i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            int64_t tmp = 0;
            auto const& cl = cnf.clause(i);

            for(auto const& l : cl) {
                Variable vl(l);

                tmp += pop.champion_x()[vl.get()];
            }

            if(tmp != 0 && tmp != cl.size()) {
                res += 1;
            }
        }
    }

    std::cout << "s " << res << "\n";

    return 0;
}
