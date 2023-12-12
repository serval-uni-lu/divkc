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
                    res += pow(2, 4 - cl.size());
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
    auto ts = std::chrono::steady_clock::now();

    std::string const path(argv[1]);
    CNF cnf(argv[1]);

    Prob prob;
    prob.cnf = cnf;

    problem p{prob};

    //algorithm algo{sga(1000)};

    auto g = gaco{10000};
    algorithm algo{g};

    population pop{p, 1000};

    pop = algo.evolve(pop);

    //std::cout << p ;
    int64_t sum = 0;
    for(int i = 0; i < cnf.nb_vars(); i++) {
        std::cout << "v " << (i + 1) << " " << pop.champion_x()[i] << "\n";
        sum += pop.champion_x()[i];
    }

    std::cout << "c c 2\n";
    std::cout << "c p 0 " << cnf.nb_vars() - sum << "\n";
    std::cout << "c p 1 " << sum << "\n";

    for(int64_t i = 0; i < 2; i++) {
        std::vector<int> ids;

        for(int64_t j = 0; j < cnf.nb_clauses(); j++) {
            if(cnf.is_active(j)) {
                auto const& cl = cnf.clause(i);

                bool toggle = std::any_of(cl.begin(), cl.end(), [&](auto const& l) {
                        return pop.champion_x()[Variable(l).get()] != i;
                        });

                if(toggle) {
                    cnf.set_active(j, false);
                    ids.push_back(j);
                }
            }
        }

        std::string tmp_path = path + ".split" + std::to_string(i);
        std::ofstream out(tmp_path);
        out << cnf;
        out.close();
        std::cout << "p " << i << " " << "split" << i << "\n";

        for(int j : ids) {
            cnf.set_active(j, true);
        }
    }


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

    std::cout << "s2 " << pop.champion_f()[0] << "\n";
    std::cout << "s " << res << "\n";

    std::chrono::duration<double> dur = std::chrono::steady_clock::now() - ts;
    std::cout << "c t " << dur.count() << "\n";

    return 0;
}
