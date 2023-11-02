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

//#define SPLIT
//#define SPLIT_R 2.2
#define VARCOMM
//#define DERIV
#define COMM_SPLIT

#ifdef SPLIT
    struct CLS_stat {
        std::size_t id;
        std::size_t nb_c;
        std::size_t size;
    };

    bool compare_cls_stat(CLS_stat const& i1, CLS_stat const& i2) {
        return  i1.size < i2.size || ((i1.size == i2.size) && i1.nb_c < i2.nb_c);
    }
#endif

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

#ifdef SPLIT
    std::cout << "c split\n";

    std::vector<CLS_stat> stats;

    for(std::size_t i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            auto const& cls = cnf.clause(i);

            std::set<int> cs;
            for(auto const& l : cls) {
                cs.insert(res.L.c[m.at(Variable(l))]);
            }

            CLS_stat tmp;
            tmp.id = i;
            tmp.nb_c = cs.size();
            tmp.size = cls.size();

            stats.push_back(tmp);
        }
    }

    std::sort(stats.begin(), stats.end(), compare_cls_stat);

    //for(auto const& i : stats) {
    //    std::cout << i.size << ", " << i.nb_c << "\n";
    //}
    
    std::size_t const nb_cls = SPLIT_R * cnf.nb_vars();

    for(std::size_t i = 0; i < stats.size(); i++) {
        if(i < nb_cls) {
            cnf.set_active(stats[i].id, true);
        }
        else {
            cnf.set_active(stats[i].id, false);
        }
    }

    std::ofstream base(path + ".base");
    base << cnf;
    base.close();

    for(std::size_t i = 0; i < cnf.nb_clauses(); i++) {
        if(i < nb_cls) {
            cnf.set_active(stats[i].id, false);
        }
        else {
            cnf.set_active(stats[i].id, true);
        }
    }

    std::ofstream rem(path + ".rem");
    rem << cnf;
    rem.close();
#endif

#ifdef COMM_SPLIT
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
