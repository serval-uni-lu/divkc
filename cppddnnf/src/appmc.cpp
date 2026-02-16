//
// Created by oz on 2/3/22.
//

#include<iostream>

#include "nnf.hpp"
#include "pdac.hpp"

#include <boost/program_options.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/math/distributions/normal.hpp>

namespace po = boost::program_options;
using namespace boost::random;
using boost::multiprecision::mpf_float;
using boost::math::normal;

/**
 * \returns The boost datastructure representing the program CLI arguments
 */
po::options_description get_program_options() {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display help message")
        ("verbose", "Display all of the intermediate estimates as well")
        ("cnf", po::value<std::string>(), "path to CNF file")
        ("alpha", po::value<double>()->default_value(0.01), "alpha value for the CLT")
        ("nb", po::value<std::size_t>()->default_value(10'000), "the maximum number of samples to use")
        ("lnb", po::value<std::size_t>()->default_value(0), "the minimum number of samples to use, set to 0 to disable early stopping (see --epsilon)")
        ("epsilon", po::value<double>()->default_value(1.1), "the algorithm stops early (before --nb samples have been reached) if (Y / epsilon) <= Yl and (Y * epsilon) >= Yh");

    return desc;
}

/**
 * \brief The approximate model counting procedure described in
 * DivKC: A Divide-and-Conquer Approach to Knowledge Compilation.
 *
 * \param N The maximum number of samples to use before stopping.
 * \param alpha The alpha value to use to compute the confidence interval with the central limit theorem.
 * \param lN The minimum number of samples to use before considering an early stop. Early stopping is disabled if lN <= 0.
 * \param epsilon If lN > 0 and if at leas lN samples have been used, then the algorithm stop early if (Y / epsilon <= yl) && (Y * epsilon >= yh)
 * \param verbose If true, print the current estimate and confidence interval on each new sample.
 */
void appmc(PDAC const& pdac, std::size_t const N, double const alpha, std::size_t const lN, double const epsilon, bool const verbose) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    random_device rng;
    mt19937 mt(rng);
    uniform_int_distribution<mpz_int> ui(1, pc);

    mpf_float rmean = 0;
    mpf_float rm = 0;

    std::size_t k = 0;
    mpf_float const z = quantile(normal(), 1 - alpha);

    std::cout << "N,Y,Yl,Yh\n";
    std::atomic<bool> done = false;

    #pragma omp parallel for
    for(std::size_t i = 0; i < N; i++) {
        if(done.load()) {
            continue;
        }

        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);
        auto l = ui(mt);

        path.clear();
        apnnf.get_path(l, path);
        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT) * pc;

        #pragma omp critical
        {
            k++;
            mpf_float n_mean = rmean + ((ai - rmean) / k);
            rm = rm + ((ai - rmean) * (ai - n_mean));
            rmean = n_mean;

            if(k > 1) {
                mpf_float S2 = rm / (k - 1);
                mpf_float sd = sqrt(S2) / sqrt(k);

                auto yl = rmean - z * sd;
                auto yh = rmean + z * sd;

                if(verbose || k >= N) {
                    std::cout << k << ", " << rmean << ", " << yl << ", " << yh << "\n";
                }

                if(lN > 0 && k >= lN && rmean / epsilon <= yl && rmean * epsilon >= yh) {
                    done.store(true);

                    if(!verbose) {
                        std::cout << k << ", " << rmean << ", " << yl << ", " << yh << "\n";
                    }
                }
            }
        }
    }

    // std::cout << "pc " << pc << "\n";
    // std::cout << "s " << rmean << "\n";
    // std::cout << "sl " << (rmean - z * sd) << "\n";
    // std::cout << "sh " << (rmean + z * sd) << "\n";
}


int main(int argc, char** argv) {
    try {
        po::options_description desc = get_program_options();

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if(vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        if(! vm.count("cnf")) {
            std::cerr << "ERROR: cnf option not set\n";
            return 1;
        }

        std::string const cnf_path = vm["cnf"].as<std::string>();
        double const alpha = vm["alpha"].as<double>();
        std::size_t const N = vm["nb"].as<std::size_t>();

        std::size_t const lN = vm["lnb"].as<std::size_t>();
        double const epsilon = vm["epsilon"].as<double>();

        bool const verbose = vm.count("verbose") > 0;

        auto pdac = pdac_from_file(cnf_path);

        appmc(pdac, N, alpha, lN, epsilon, verbose);

        return 0;
    }
    catch(std::exception & e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}
