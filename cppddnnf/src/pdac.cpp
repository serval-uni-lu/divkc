#include "var.hpp"
#include "nnf.hpp"
#include "pdac.hpp"

#include <fstream>

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

/**
 * \brief The exact model counting procedure described in
 * DivKC: A Divide-and-Conquer Approach to Knowledge Compilation.
 */
mpz_int exact_mc(PDAC const& pdac) {
    ANNF apnnf = ANNF(pdac.pnnf);
    apnnf.annotate_pc();
    auto const pc = apnnf.mc(NNF::ROOT);

    std::set<Literal> path;
    ANNF aunnf = ANNF(pdac.unnf);
    mpz_int tmc = 0;

    for(mpz_int i = 1; i <= pc; i++) {
        path.clear();
        apnnf.get_path(i, path);

        aunnf.set_assumps(path);
        aunnf.annotate_mc();
        auto const ai = aunnf.mc(NNF::ROOT);

        tmc += ai;
    }

    return tmc;
}

