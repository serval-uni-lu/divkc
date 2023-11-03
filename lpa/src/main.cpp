#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<utility>
#include<omp.h>
#include<algorithm>

#include<ctime>

#include "CNF.hpp"
#include "graph.hpp"

#include "common.hpp"
#include "community.hpp"

#define VARCOMM
//#define DERIV
#define COMM_SPLIT

int main(int argc, char const** argv) {
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
    std::cout << "c var comm:\n";
    for(auto const& i : m) {
        std::cout << "c v " << i.first << " " << res.L.c[i.second] << "\n";
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
    while(merge_commuities_by_level(m, cnf, res.L, lvl, min_c_count)) {
        lvl += 1;
        res.L.reassign();
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

        std::cout << "c c" << i << " : " << cnf.nb_active_clauses() << "\n";

        std::ofstream out(path + "." + std::to_string(i));
        out << cnf;
        out.close();

        for(int j : ids) {
            cnf.set_active(j, true);
        }
    }
#endif

    return 0;
}
