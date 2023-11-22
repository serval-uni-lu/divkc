#pragma once

#include<vector>
#include <boost/multiprecision/gmp.hpp>

#include "CNF.hpp"

// https://www.boost.org/doc/libs/1_73_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/gmp_int.html

struct Edge {
    static Variable * freeVars;
    static int64_t szFreeVars, capFreeVars;

    static Literal * unitLits;
    static int64_t szUnitLits, capUnitLits;

    int target;
    int b_cnst, e_cnst;
    int b_free, e_free;
};

using namespace boost::multiprecision;

struct Node {
    int num;
    mpz_int mc;

    std::vector<Edge> children;

    virtual void add_child(uint64_t target, std::vector<Literal> const& consts, std::vector<Variable> const& f);

    virtual void annotate_mc();
};

struct AndNode : Node {
    void add_child(uint64_t target, std::vector<Literal> const& consts, std::vector<Variable> const& f) {
    }

    void annotate_mc() {
    }
};
