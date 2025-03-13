#ifndef PDAC_HPP
#define PDAC_HPP

#include <set>

struct PDAC {
    std::set<Variable> pvar;
    NNF pnnf;
    NNF unnf;
};


PDAC pdac_from_file(std::string const& path);
void appmc(PDAC const& pdac, int const N, double const alpha);
void ksampler(PDAC const& pdac, int const N, int const k);

#endif
