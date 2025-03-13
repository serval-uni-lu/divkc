#include "var.hpp"
#include "nnf.hpp"
#include "pdac.hpp"

#include <fstream>
#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/multiprecision/gmp.hpp>

#include <boost/math/distributions/normal.hpp>

using namespace boost::random;
using boost::multiprecision::mpf_float;
using boost::math::normal;

PDAC pdac_from_file(std::string const& path) {
    std::set<Variable> pvar;

    std::ifstream f(path + ".proj.log");
    std::string line;
    while(getline(f, line)) {
        if(line.find("c p show ") == 0) {
            std::stringstream stream(line);
            std::string tag;
            stream >> tag >> tag >> tag;
            int v;

            while((stream >> v)) {
                if(v != 0) {
                    pvar.insert(Variable(v));
                }
            }
        }
    }
    f.close();

    PDAC res;

    res.pnnf.init(path + ".pnnf", true, pvar);
    res.unnf.init(path + ".unnf", false, pvar);
    res.pvar = pvar;

    // res.pnnf.annotate_pc();

    return res;
}

void appmc(PDAC & pdac, int const N, double const alpha) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(ROOT);

    random_device rng;
    mt19937 mt(rng);
    uniform_int_distribution<mpz_int> ui(1, pc);

    mpf_float rmean = 0;
    mpf_float rm = 0;

    int k = 0;

    #pragma omp parallel for
    for(int i = 0; i < N; i++) {
        std::vector<Literal> path;
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
        }
    }

    mpf_float S2 = rm / (N - 1);
    mpf_float sd = sqrt(S2) / sqrt(N);

    mpf_float z = quantile(normal(), 1 - alpha);

    std::cout << "pc " << pc << "\n";
    std::cout << "s " << rmean << "\n";
    std::cout << "sl " << (rmean - z * sd) << "\n";
    std::cout << "sh " << (rmean + z * sd) << "\n";
}
