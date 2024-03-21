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

        if(res == 0 || res == v.size()) {
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
        return {static_cast<double>(-1 * res) / nb_v};
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

    //std::cout << p ;
    int64_t sum = 0;

    {
        std::string tmp_path = path + ".fpv";
        std::ofstream out(tmp_path);
        for(int i = 0; i < cnf.nb_vars(); i++) {
            std::cout << "v " << (i + 1) << " " << pop.champion_x()[i] << "\n";
            sum += pop.champion_x()[i];

            if(pop.champion_x()[i] == 1) {
                out << (i + 1) << "\n";
            }
        }
        out.close();
    }

    std::cout << "c c 2\n";
    std::cout << "c p 0 " << cnf.nb_vars() - sum << "\n";
    std::cout << "c p 1 " << sum << "\n";

    std::vector<int> ids;

    // // comm 1 (projection)
    // for(int64_t j = 0; j < cnf.nb_clauses(); j++) {
    //     if(cnf.is_active(j)) {
    //         auto const& cl = cnf.clause(j);

    //         bool toggle = false;
    //         for(auto const & l : cl) {
    //             toggle = toggle || pop.champion_x()[Variable(l).get()] != 1;
    //         }

    //         if(toggle) {
    //             cnf.set_active(j, false);
    //             ids.push_back(j);
    //         }
    //     }
    // }

    // {
    // std::string tmp_path = path + ".split1";
    // std::ofstream out(tmp_path);
    // out << cnf;
    // out.close();
    // std::cout << "p 1 split1\n";
    // }

    // for(int j : ids) {
    //     cnf.set_active(j, true);
    // }
    // ids.clear();

    // comm 0 (upper bound)
    for(int64_t j = 0; j < cnf.nb_clauses(); j++) {
        if(cnf.is_active(j)) {
            auto const& cl = cnf.clause(j);

            bool toggle = false;
            for(auto const & l : cl) {
                toggle = toggle || pop.champion_x()[Variable(l).get()] != 1;
            }

            if(!toggle) {
                cnf.set_active(j, false);
                ids.push_back(j);
            }
        }
    }

    {
    std::string tmp_path = path + ".up";
    std::ofstream out(tmp_path);
    out << cnf;
    out.close();
    std::cout << "p 0 split0\n";
    }

    for(int j : ids) {
        cnf.set_active(j, true);
    }
    ids.clear();

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
