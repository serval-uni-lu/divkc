#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CLAUSE_TEST
#include <boost/test/unit_test.hpp>

#include <set>
#include <random>

#include "var.hpp"
#include "CNF.hpp"

BOOST_AUTO_TEST_CASE(creation_test)
{
    int const nb_var = 50;
    std::size_t const N = 1000;
    std::size_t const k = 100;

    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_int_distribution<> vard(1, nb_var);
    std::uniform_int_distribution<> kd(1, k);
    std::uniform_int_distribution<> sd(0, 1);

    for(std::size_t nb = 0; nb < N; nb++) {
        std::set<Literal> data;
        Clause cl;

        std::size_t const lk = kd(gen);
        for(std::size_t ki = 0; ki < lk; ki++) {
            Literal l(vard(gen) * (sd(gen) * 2 - 1));

            data.insert(l);
            cl.push(l);
        }

        for(Literal const& l : data) {
            BOOST_CHECK(cl.contains(l));
        }

        for(Literal const& l : cl) {
            BOOST_CHECK(data.find(l) != data.end());
        }

        for(std::size_t i = 1; i < cl.size(); i++) {
            BOOST_CHECK(cl[i - 1] < cl[i]);
        }

        std::size_t const lk2 = kd(gen);
        Literal l(vard(gen) * (sd(gen) * 2 - 1));
        cl.push(l);
        std::size_t const cl_size = cl.size();
        for(std::size_t ki = 0; ki < lk2; ki++) {
            cl.push(l);
            BOOST_CHECK(cl.size() == cl_size);
            BOOST_CHECK(cl.contains(l));
        }

        std::size_t const lk3 = kd(gen);
        for(std::size_t ki = 0; ki < lk3; ki++) {
            cl.remove(l);
            BOOST_CHECK(cl.size() == cl_size - 1);
            BOOST_CHECK(! cl.contains(l));
        }
    }
}

BOOST_AUTO_TEST_CASE(creation2_test)
{
    int const nb_var = 50;
    std::size_t const N = 1000;
    std::size_t const k = 100;

    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_int_distribution<> vard(1, nb_var);
    std::uniform_int_distribution<> kd(1, k);
    std::uniform_int_distribution<> sd(0, 1);

    for(std::size_t nb = 0; nb < N; nb++) {
        Clause cl;

        std::size_t const lk = kd(gen);
        Literal l(vard(gen) * (sd(gen) * 2 - 1));
        for(std::size_t ki = 0; ki < lk; ki++) {
            cl.push(l);
        }

        BOOST_CHECK(cl.size() == 1);
    }
}
