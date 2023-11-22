#pragma once

#include<vector>
#include <boost/multiprecision/gmp.hpp>

// https://www.boost.org/doc/libs/1_73_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/gmp_int.html

struct Edge {
    int target;
    int b_cnst, e_cnst;
    int b_free, e_free;
};

struct Node {
    int num;
    //mc
};
