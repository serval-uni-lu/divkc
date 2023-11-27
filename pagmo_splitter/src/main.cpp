#include <iostream>

#include <pagmo/algorithm.hpp>
#include <pagmo/algorithms/pso.hpp>
#include <pagmo/population.hpp>
#include <pagmo/problem.hpp>


using namespace pagmo;

struct prob {
    vector_double fitness(const vector_double & v) const {
        return {(1-v[0]) * (1-v[1]) * v[2] + (1-v[0]) * v[1] * (1-v[2]) + v[0] * (1-v[1]) * v[2] + v[0] * v[1] * (1-v[0]) + (1-v[0]) * (1-v[1]) * (1-v[2]) + v[0] * v[1] * v[2]};
    }

    std::pair<vector_double, vector_double> get_bounds() const {
        return {{0, 0, 0}, {1, 1, 1}};
    }

    vector_double::size_type get_nix() const {
        return 3;
    }
};

int main() {
    problem p{prob{}};
    algorithm algo{pso(100)};
    population pop{p, 24};

    pop = algo.evolve(pop);

    std::cout << p ;
    for(int i = 0; i < 3; i++)
        std::cout << pop.champion_x()[i] << "\n";

    return 0;
}
