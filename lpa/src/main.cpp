#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<utility>
#include<omp.h>
#include<algorithm>
#include<chrono>

#include<ctime>
#include<cmath>

#include "CNF.hpp"
#include "graph.hpp"

#include "common.hpp"
#include "community.hpp"

//#define VARCOMM
//#define DERIV
#define COMM_SPLIT
//#define COMM_SUB

int main(int argc, char const** argv) {
    auto ts = std::chrono::steady_clock::now();

    srand(time(0));

    std::string const path(argv[1]);
    CNF cnf(path.c_str());
    //cnf.simplify();
    //cnf.subsumption();

    const int nb = std::stoi(argv[2]);

    auto m = cnf.get_reduced_mapping();
    Graph g = build_graph(cnf, m);

    //std::cout << "GFA\n";
    //gfa(cnf);
    LPA_res res = lpa(cnf, m, g);

#pragma omp parallel for
    for(int i = 1; i < nb; i++) {
        auto tmp = lpa(cnf, m, g);

        if(tmp.mod > res.mod) {
#pragma omp critical
            {
                if(tmp.mod > res.mod) {
                    res = tmp;
                }
            }
        }
    }

    std::cout << "c q " << res.mod << "\n";
    std::cout << "c c " << res.L.nb_communities() << "\n";

#ifdef VARCOMM
    std::vector<int> part_sizes(res.L.c.nb_communities(), 0);

    for(auto const& i : m) {
        std::cout << "v " << i.first << " " << res.L.c[i.second] << "\n";
        part_sizes[res.L.c[i.second]] += 1;
    }

    for(int i = 0; i < part_sizes.size(); i++) {
        std::cout << "c p " << i << " " << part_sizes[i] << "\n";
    }
#endif

#ifdef DERIV
    std::cout << "c deriv\nc #c q dq ; cls\n";
    for(std::size_t i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            auto const& cls = cnf.clause(i);

            std::set<int> cs;
            for(auto const& l : cls) {
                cs.insert(res.L.c[m.at(Variable(l))]);
            }

            remove_clause_from_graph(g, cls, m);
            double q = modularity(g, res.L);
            add_clause_to_graph(g, cls, m);

            std::cout << "c w " << cs.size() << " " << q << " " << q - res.mod << " ; " << cls << "\n";
        }
    }
#endif

#ifdef COMM_SPLIT
    const int min_c_count = std::stoi(argv[3]);

    int lvl = 1;
    while(merge_commuities_by_level(m, cnf, res.L, lvl, min_c_count) && lvl <= cnf.nb_vars()) {
        lvl += 1;
        //res.L.reassign();
    }

    res.L.reassign();

    std::vector<int> part_sizes(res.L.nb_communities(), 0);

    for(auto const& i : m) {
        std::cout << "v " << i.first << " " << res.L.c[i.second] << "\n";
        part_sizes[res.L.c[i.second]] += 1;
    }

    for(int i = 0; i < part_sizes.size(); i++) {
        std::cout << "c p " << i << " " << part_sizes[i] << "\n";
    }

    for(int i = 0; i < res.L.nb_communities(); i++) {
        std::vector<int> ids;

        for(int j = 0; j < cnf.nb_clauses(); j++) {
            if(cnf.is_active(j)) {
                auto const& cls = cnf.clause(j);

                bool toggle = std::any_of(cls.begin(), cls.end(), [&](auto const& l) {
                        return res.L.c[m.at(Variable(l))] != i;
                        });

                if(toggle) {
                    cnf.set_active(j, false);
                    ids.push_back(j);
                }
            }
        }

        //std::cout << "c c" << i << " : " << cnf.nb_active_clauses() << "\n";

        std::string tmp_path = path + ".split" + std::to_string(i);
        std::ofstream out(tmp_path);
        out << cnf;
        out.close();
        std::cout << "p " << i << " " << "split" << i << "\n";

        for(int j : ids) {
            cnf.set_active(j, true);
        }
    }

    std::set<Variable> v_overlap;
    std::set<int> c_rev;

    int nb_c = 0;
    int w_c = 0;
    for(int i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            auto const& cl = cnf.clause(i);

            std::set<int> tmp;
            for(auto const& l : cl) {
                //tmp.insert(partvec[Variable(l).get()]);
                tmp.insert(res.L.c[m.at(Variable(l))]);
            }

            if(tmp.size() > 1) {
                nb_c += 1;
                w_c += static_cast<int>(pow(2, 4 - cl.size()));

                for(auto const& l : cl) {
                    v_overlap.emplace(l);
                }
            }
            else {
                c_rev.insert(i);
                cnf.set_active(i, false);
            }
        }
    }

    for(auto const& vi : v_overlap) {
        std::cout << "v cross " << vi << "\n";
    }

    std::cout << "c cross " << v_overlap.size() << "\n";

    std::string tmp_path = path + ".rem";
    std::ofstream out(tmp_path);
    out << cnf;
    out.close();

    for(auto const& i : c_rev) {
        cnf.set_active(i, true);
    }

    std::cout << "s " << nb_c << "\n";
    std::cout << "s2 " << w_c << "\n";
#endif

#ifdef COMM_SUB
    long double const min_prob = 1.5878347497057898e-06;
    long double current_prob = 1.0;

    std::map<int, std::vector<int> > smap;

    for(int i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            auto const& cl = cnf.clause(i);

            std::set<int> tmp;
            for(auto const& l : cl) {
                //tmp.insert(partvec[Variable(l).get()]);
                tmp.insert(res.L.c[m.at(Variable(l))]);
            }

            if(tmp.size() > 1) {
                auto it = smap.find(tmp.size());

                if(it == smap.end()) {
                    std::vector<int> v;
                    v.push_back(i);

                    smap[tmp.size()] = v;
                }
                else {
                    it->second.push_back(i);
                }
            }
        }
    }

    for(auto & e : smap) {
        std::sort(e.second.begin(), e.second.end(), [&](int a, int b) {
                    return cnf.clause(a).size() > cnf.clause(b).size();
                });
    }
    int nb_rm = 0;

    for(int i = res.L.nb_communities(); i > 0 && current_prob > min_prob; i--) {
        auto it = smap.find(i);

        if(it != smap.end()) {
            for(int j : it->second) {
                long double tpow = pow(2, cnf.clause(j).size());
                tpow = (tpow - 1) / tpow;
                tpow *= current_prob;

                if(tpow >= min_prob) {
                    cnf.set_active(j, false);
                    current_prob = tpow;
                    nb_rm += 1;
                }
                else {
                    break;
                }
            }
        }
    }

    std::string tmp_path = path + ".sub";
    std::ofstream out(tmp_path);
    out << cnf;
    out.close();

    std::cout << "c rm " << nb_rm << "\n";
#endif

    std::chrono::duration<double> dur = std::chrono::steady_clock::now() - ts;
    std::cout << "c t " << dur.count() << "\n";

    return 0;
}
