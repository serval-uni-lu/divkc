#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <stack>
#include <algorithm>

#include <cmath>

#include "patoh.h"
#include "CNF.hpp"

// void compute_hypergraph(CNF const& cnf, std::vector<int> & xpins, std::vector<int> & pins) {
//     std::vector<std::set<std::size_t> > hg(cnf.get_nb_vars());
// 
//     for(std::size_t j = 0; j < cnf.get_nb_clauses(); j++) {
//         for(int i : cnf[j]) {
//             i = std::abs(i) - 1;
//             hg[i].insert(j);
//         }
//     }
// 
//     //for(int i : cnf.get_vars()) {
//     //    bool exists = false;
//     //    int pos = pins.size();
//     //    for(int j = 0; j < static_cast<int>(cnf.get_nb_clauses()); j++) {
//     //        if(cnf[j].find(i) != cnf[j].end() || cnf[j].find(-i) != cnf[j].end()) {
//     //            exists = true;
//     //            pins.push_back(j);
//     //        }
//     //    }
//     //    if(exists) {
//     //        xpins.push_back(pos);
//     //    }
//     //}
//     //xpins.push_back(pins.size());
// 
//     for(auto const& s : hg) {
//         if(!s.empty()) {
//             int pos = pins.size();
//             for(auto p : s) {
//                 pins.push_back(static_cast<int>(p));
//             }
//             xpins.push_back(pos);
//         }
//     }
//     xpins.push_back(pins.size());
// }

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

std::vector<int> split(CNF const& cnf, int const nb_part, int & cost) {
    PaToH_Parameters pargs;
    // D4 does QUALITY if nb hyperedges >= 200
    PaToH_Initialize_Parameters(&pargs, PATOH_CUTPART, PATOH_SUGPARAM_QUALITY);
    // PaToH_Initialize_Parameters(&pargs, PATOH_CONPART, PATOH_SUGPARAM_DEFAULT);
    pargs._k = nb_part;

    std::vector<int> xpins;
    std::vector<int> pins;
    compute_var_hypergraph(cnf, xpins, pins);
    const int _c = cnf.nb_vars();
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

//std::vector<CNF> apply_partition(CNF const& cnf, std::vector<int> const& part, int const nb_part) {
//    std::vector<CNF> res(nb_part, CNF());
//
//    for(int i = 0; i < static_cast<int>(part.size()); i++) {
//        int const partition = part[i];
//        res[partition].add_clause(cnf[i]);
//    }
//
//    return res;
//}
//
//std::set<int> get_conflicts(CNF const& cnf, std::vector<int> const& part, int const nb_part) {
//    std::set<int> res;
//    std::vector<std::set<int> > vars(nb_part);
//
//    for(int i = 0; i < static_cast<int>(part.size()); i++) {
//        int const partition = part[i];
//        for(int l : cnf[i]) {
//            l = std::abs(l);
//            vars[partition].insert(l);
//        }
//    }
//
//    for(std::size_t i = 0; i < vars.size() - 1; i++) {
//        for(std::size_t j = i + 1; j < vars.size(); j++) {
//            for(int l : vars[i]) {
//                if(vars[j].find(l) != vars[j].end()) {
//                    res.insert(l);
//                }
//            }
//        }
//    }
//
//    return res;
//}
//
//std::ostream& print_map(std::ostream & out, std::vector<int> const& map) {
//    out << "c map";
//    for(std::size_t i = 1; i < map.size(); i++) {
//        out << " " << map[i];
//    }
//    out << " 0\n";
//
//    return out;
//}
//
//std::size_t nb_occ(CNF const& cnf, int v) {
//    std::size_t res = 0;
//
//    for(auto const& s : cnf) {
//        if(s.find(v) != s.end() || s.find(-v) != s.end()) {
//            res ++;
//        }
//    }
//
//    return res;
//}
//
//int select_var(CNF const& cnf) {
//    int const nb_part = 2;
//    int cost;
//    auto partvec = split(cnf, nb_part, cost);
//    auto v = get_conflicts(cnf, partvec, nb_part);
//
//    int var = -1;
//    std::size_t score = 0;
//
//    for(int i : v) {
//        std::size_t tmp = nb_occ(cnf, i);
//        if(tmp > score) {
//            var = i;
//            score = tmp;
//        }
//    }
//
//    return var;
//}
//
//std::set<int> select_vars(CNF const& cnf, int nb) {
//    int const nb_part = 2;
//    int cost;
//    auto partvec = split(cnf, nb_part, cost);
//    auto v = get_conflicts(cnf, partvec, nb_part);
//
//    std::set<int> res;
//    int var = -1;
//    std::size_t score = 0;
//
//    for(int k = 0; k < nb; k++) {
//        for(int i : v) {
//            std::size_t tmp = nb_occ(cnf, i);
//            if(tmp > score) {
//                var = i;
//                score = tmp;
//            }
//        }
//        v.erase(var);
//        res.insert(var);
//        var = -1;
//        score = 0;
//    }
//
//    return res;
//}
//
//std::set<CNF::Clause> instanciations(std::set<int> const& atoms) {
//    std::set<CNF::Clause> res;
//
//    for(int i = 0; i < (1 << atoms.size()); i++) {
//        int tmp = i;
//        CNF::Clause c;
//        for(int l : atoms) {
//            c.insert(tmp & 1 ? l : -l);
//            tmp = tmp >> 1;
//        }
//        res.insert(c);
//    }
//
//    return res;
//}

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

    for(int i = 0; i < partvec.size(); i++) {
        std::cout << "v " << (i + 1) << " " << partvec[i] << "\n";
        part_sizes[partvec[i]] += 1;
    }

    for(int i = 0; i < part_sizes.size(); i++) {
        std::cout << "c p " << i << " " << part_sizes[i] << "\n";
    }

    int nb_c = 0;
    for(int i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            auto const& cl = cnf.clause(i);

            std::set<int> tmp;
            for(auto const& l : cl) {
                tmp.insert(partvec[Variable(l).get()]);
            }

            if(tmp.size() > 1) {
                nb_c += 1;
            }
        }
    }

    for(int p = 0; p < nb_part; p++) {
        std::set<int> clsi;

        for(int j = 0; j < cnf.nb_clauses(); j++) {
            if(cnf.is_active(j)) {
                auto const& cl = cnf.clause(j);

                bool tmp = std::any_of(cl.begin(), cl.end(), [&](Literal const& l) {
                        return partvec[Variable(l).get()] != p;
                        });

                if(tmp) {
                    cnf.set_active(j, false);
                    clsi.insert(j);
                }
            }
        }
        std::string tmp_path = path + ".split" + std::to_string(p);
        std::ofstream out(tmp_path);
        out << cnf;
        out.close();
        std::cout << "p " << p << " " << tmp_path << "\n";

        for(int j : clsi) {
            cnf.set_active(j, true);
        }
    }

    std::cout << "s " << cost << "\n";
    std::cout << "s2 " << nb_c << "\n";

    // auto part = apply_partition(cnf, partvec, nb_part);
    // auto v = get_conflicts(cnf, partvec, nb_part);

    // std::cout << "split cost: " << cost << std::endl;
    // std::cout << "split cost: " << v.size() << std::endl << std::endl;
    // for(std::size_t ipart = 0; ipart < part.size(); ipart++) {
    //     auto & icnf = part[ipart];
    //     auto r_path = path + std::to_string(ipart);
    //     std::cout << r_path << std::endl;
    //     std::ofstream out(r_path);

    //     auto map = icnf.rename_vars();
    //     print_map(out, map) << icnf;
    //     out.close();
    // }

    return 0;
}
