#include<iostream>

#include "nnf.hpp"
#include "pdac.hpp"

#include <boost/program_options.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace po = boost::program_options;
using namespace boost::random;

/**
 * \returns The boost datastructure representing the program CLI arguments
 */
po::options_description get_program_options() {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display help message")
        ("cnf", po::value<std::string>(), "path to CNF file")
        ("k", po::value<std::size_t>()->default_value(50), "buffer size to use")
        ("nb", po::value<std::size_t>()->default_value(10), "the number of solutions to generate");

    return desc;
}

/**
 * \brief The classical uniform sampling algorithm form d-DNNFs.
 *
 *
 * \param N The number of models to generate
 */
void exact_uniform_sampling(PDAC const& pdac, std::size_t const N) {
    std::cout << "c true uniformity\n";

    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);
    std::size_t const ipc = static_cast<std::size_t>(pc);

    // auto const tmc = exact_mc(pdac);
    mpz_int tmc = 0;
    std::vector<mpz_int> pmc(ipc, 0);

    #pragma omp parallel for
    for(std::size_t i = 1; i <= ipc; i++) {
        ANNF aunnf = ANNF(pdac.unnf);
        std::set<Literal> path;
        apnnf.get_path(i, path);

        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(ROOT);

        #pragma omp critical
        {
            tmc += ai;
        }
        pmc[i - 1] = ai;
    }


    random_device rng;
    mt19937 mt(rng);
    uniform_int_distribution<mpz_int> ui(1, tmc);

    #pragma omp parallel for
    for(std::size_t i = 0; i < N; i++) {
        mpz_int id = 0;
        #pragma omp critical
        id = ui(mt);

        bool ch = false;

        std::set<Literal> lpath;
        ANNF launnf = ANNF(pdac.unnf);

        for(int pid = 1; 1 <= ipc; pid++) {
            if(id <= pmc[pid - 1]) {
                lpath.clear();
                apnnf.get_path(pid, lpath);
                launnf.set_assumps(lpath);
                launnf.annotate_mc();
                launnf.get_solution(id, lpath);
                // std::sort(lpath.begin(), lpath.end());
                #pragma omp critical
                {
                    for(auto const& l : lpath) {
                        std::cout << l << " ";
                    }
                    std::cout << "0\n";
                }
                ch = true;
                break;
            }
            else {
                id -= pmc[pid - 1];
            }

        }

        if(!ch) {
            #pragma omp critical
            std::cerr << "c ERROR\n";
        }
    }
}

/**
 * \brief The heuristic-based sampling algorithm as described in
 * DivKC: A Divide-and-Conquer Approach to Knowledge Compilation.
 *
 *
 * \param N The number of models to generate
 * \param k The buffer size to use for the heuristic-based algorithm
 */
void heuristic_uniform_sampling(PDAC const& pdac, std::size_t const N, std::size_t const k) {
    std::cout << "c heuristic based uniformity\n";

    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    random_device rng;

    #pragma omp parallel for
    for(std::size_t i = 0; i < N; i++) {
        std::set<Literal> path;
        ANNF aunnf = ANNF(pdac.unnf);

        mt19937 mt;
        #pragma omp critical
        mt.seed(rng);

        uniform_int_distribution<mpz_int> ui(1, pc);

        std::set<mpz_int> pids;
        while(pids.size() < k) {
            pids.insert(ui(mt));
        }

        std::vector<mpz_int> vpids(pids.begin(), pids.end());
        std::vector<mpz_int> mc_vpids;
        mpz_int tmc = 0;
        for(std::size_t j = 0; j < vpids.size(); j++) {
            path.clear();
            apnnf.get_path(vpids[j], path);
            aunnf.set_assumps(path);
            aunnf.annotate_mc();
            auto const& ai = aunnf.mc(ROOT);

            mc_vpids.push_back(ai);
            tmc += ai;
        }

        uniform_int_distribution<mpz_int> lui(1, tmc);
        mpz_int id = lui(mt);
        bool ch = false;

        for(std::size_t j = 0; j < vpids.size(); j++) {
            if(id <= mc_vpids[j]) {
                path.clear();
                apnnf.get_path(vpids[j], path);
                aunnf.set_assumps(path);
                aunnf.annotate_mc();

                aunnf.get_solution(id, path);
                // std::sort(path.begin(), path.end());
                #pragma omp critical
                {
                    for(auto const& l : path) {
                        std::cout << l << " ";
                    }
                    std::cout << "0\n";
                }

                ch = true;
                break;
            }
            else {
                id -= mc_vpids[j];
            }
        }

        if(!ch) {
            #pragma omp critical
            std::cerr << "c ERROR\n";
        }
    }
}

/**
 * \brief Counts the number of paths, if the number of paths is smaller than k,
 * then the exact algorithm is used. Otherwise the heuristic-based algorithm is used.
 *
 * \param N The number of models to generate
 * \param k The buffer size to use for the heuristic-based algorithm
 */
void ksampler(PDAC const& pdac, std::size_t const N, std::size_t const k) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    if(pc <= k) {
        exact_uniform_sampling(pdac, N);
    }
    else {
        heuristic_uniform_sampling(pdac, N, k);
    }
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
        std::size_t const N = vm["nb"].as<std::size_t>();
        std::size_t const k = vm["k"].as<std::size_t>();


        auto pdac = pdac_from_file(cnf_path);

        ksampler(pdac, N, k);

        return 0;
    }
    catch(std::exception & e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}
