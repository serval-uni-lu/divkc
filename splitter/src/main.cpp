#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <stack>
#include <algorithm>

#include <cmath>

#include "patoh.h"
#include "CNF.hpp"

#define VIG
//#define CIG

void compute_var_hypergraph(CNF const& cnf, std::vector<int> & xpins, std::vector<int> & pins) {
    for(std::size_t j = 0; j < cnf.nb_clauses(); j++) {
        if(cnf.is_active(j)) {
            auto const& cl = cnf.clause(j);

            if(cl.size() > 1) {
                int pos = pins.size();

                for(auto const& i : cl) {
                    pins.push_back(Variable(i).get());
                }
                xpins.push_back(pos);
            }
        }
    }

    xpins.push_back(pins.size());
}

void compute_clause_hypergraph(CNF const& cnf, std::vector<int> & xpins, std::vector<int> & pins) {
    for(int i = 1; i < cnf.nb_vars() + 1; i++) {
        std::set<int> ids;
        for(int id : cnf.get_idx(Literal(i))) {
            //if(cnf.is_active(id)) {
                ids.insert(id);
            //}
        }

        for(int id : cnf.get_idx(Literal(-1 * i))) {
            //if(cnf.is_active(id)) {
                ids.insert(id);
            //}
        }

        if(ids.size() > 1) {
            int pos = pins.size();

            for(auto const& id : ids) {
                pins.push_back(id);
            }

            xpins.push_back(pos);
        }
    }

    xpins.push_back(pins.size());
}

std::vector<int> split(CNF const& cnf, int const nb_part, int & cost) {
    PaToH_Parameters pargs;
    // D4 does QUALITY if nb hyperedges >= 200
    PaToH_Initialize_Parameters(&pargs, PATOH_CUTPART, PATOH_SUGPARAM_QUALITY);
    // PaToH_Initialize_Parameters(&pargs, PATOH_CONPART, PATOH_SUGPARAM_DEFAULT);
    pargs._k = nb_part;

    std::vector<int> xpins;
    std::vector<int> pins;
#ifdef VIG
    compute_var_hypergraph(cnf, xpins, pins);
    const int _c = cnf.nb_vars();
#endif
#ifdef CIG
    compute_clause_hypergraph(cnf, xpins, pins);
    const int _c = cnf.nb_clauses();
#endif
    const int _n = xpins.size() - 1;
    const int _nconst = 1;

    std::vector<int> nwghts(_n, 1);
    std::vector<int> cwghts(_c * _nconst, 1);

    PaToH_Alloc(&pargs, _c, _n, _nconst, cwghts.data(), NULL, xpins.data(), pins.data());

    std::vector<int> partvec(_c, -1);
    std::vector<int> pw(pargs._k * _nconst, -1);

    int cut;
    PaToH_Part(&pargs, _c, _n, _nconst, 0, cwghts.data(), NULL, xpins.data(), pins.data(), NULL, partvec.data(), pw.data(), &cut);
    cost = cut;


    PaToH_Free();
    return partvec;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        std::cerr << "usage: splitter <path to DIMACS cnf> <split size>\n";
        return 1;
    }
    std::string path(argv[1]);
    //int const s_size = std::stoi(argv[2]);
    int const nb_part = std::stoi(argv[2]);
    int cost;
    CNF cnf(path.c_str());
    //int const nb_part = ceil(cnf.get_nb_vars() / (double)s_size);
//    cnf.simplify();

    auto partvec = split(cnf, nb_part, cost);
    std::vector<int> part_sizes(nb_part, 0);

#ifdef VIG
    for(int i = 0; i < partvec.size(); i++) {
        // std::cout << "v " << (i + 1) << " " << partvec[i] << "\n";
        part_sizes[partvec[i]] += 1;
    }

    // for(int i = 0; i < part_sizes.size(); i++) {
    //     std::cout << "c p " << i << " " << part_sizes[i] << "\n";
    // }

    int nb_c = 0;
    std::set<Variable> vprj;
    for(int i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            auto const& cl = cnf.clause(i);

            std::set<int> tmp;
            for(auto const& l : cl) {
                tmp.insert(partvec[Variable(l).get()]);
            }

            if(tmp.size() > 1) {
                nb_c += 1;
                for(auto const& l : cl) {
                    vprj.insert(Variable(l));
                }
            }
        }
    }

    std::cout << "c p show";
    for(auto const& v : vprj) {
        std::cout << " " << v;
    }
    std::cout << " 0\n";
#endif

#ifdef CIG
    std::map<int, std::set<Variable> > svmap;
    for(int i = 0; i < nb_part; i++) {
        std::set<Variable> sv;

        for(int j = 0; j < cnf.nb_clauses(); j++) {
            if(partvec[j] == i) {
                for(auto const& l : cnf.clause(j)) {
                    sv.insert(Variable(l));
                }
            }
        }
        //svmap[i] = sv;

        for(auto const& v : sv) {
            std::cout << "v " << v << " " << i << "\n";
        }

        std::cout << "c p " << i << " " << sv.size() << "\n";
    }

    for(int i = 0; i < nb_part; i++) {
        for(int j = 0; j < cnf.nb_clauses(); j++) {
            if(partvec[j] == i) {
                cnf.set_active(j, true);
            }
            else {
                cnf.set_active(j, false);
            }
        }

        std::string tmp_path = path + ".split" + std::to_string(i);
        std::ofstream out(tmp_path);
        out << cnf;
        out.close();
        std::cout << "p " << i << " " << "split" << i << "\n";
    }

    std::cout << "s " << cost << "\n";
    std::cout << "s2 " << cost << "\n";
#endif

    return 0;
}
