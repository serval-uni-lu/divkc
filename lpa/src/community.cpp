#include "community.hpp"

#include<iostream>
#include<algorithm>

void Community::merge(int c1, int c2) {
    int ma = std::max(c1, c2);
    int mi = std::min(c1, c2);
    for(int i = 0; i < c.size(); i++) {
        if(c[i] == ma) {
            c[i] = mi;
        }
    }
}

std::set<int> Community::get_communities() const {
    std::set<int> r;
    for(auto const& i : c) {
        r.insert(i);
    }
    return r;
}

std::map<int, std::size_t> Community::community_sizes() const {
    std::map<int, std::size_t> res;

    for(auto const& i : c) {
        res[i] = 0;
    }

    for(auto const& i : c) {
        res[i] += 1;
    }

    return res;
}

std::size_t Community::max_comm_size() const {
    std::size_t r = 0;
    for(auto const& e : community_sizes()) {
        r = std::max(r, e.second);
    }
    return r;
}

int Community::max_comm_id() const {
    int id = 0;
    std::size_t r = 0;
    for(auto const& e : community_sizes()) {
        if(e.second > r) {
            id = e.first;
            r = std::max(r, e.second);
        }
    }
    return id;
}

std::size_t Community::min_comm_size() const {
    std::size_t r = max_comm_size();
    for(auto const& e : community_sizes()) {
        r = std::min(r, e.second);
    }
    return r;
}

double Community::avg_comm_size() const {
    std::size_t r = 0;
    std::size_t nb = nb_communities();
    for(auto const& e : community_sizes()) {
        r += e.second;
    }
    return r / (double)nb;
}

std::vector<int> Community::get_community_nodes(int comm) const {
    std::vector<int> r;

    for(int i = 0; i < c.size(); i++) {
        if(c[i] == comm) {
            r.push_back(i);
        }
    }

    return r;
}

bool Community::is_connected(Graph const& g, int i, int comm) const {
    for(auto const& j : g.g[i]) {
        if(c[j.first] == comm) {
            return true;
        }
    }
    return false;
}

void Community::reassign() {
    auto cms = get_communities();
    // maps current id -> new id
    std::map<int, int> m;
    int next_comm = 0;

    for(auto const& i : cms) {
        m[i] = next_comm;
        next_comm++;
    }

    for(int i = 0; i < c.size(); i++) {
        c[i] = m[c[i]];
    }
}

void Community::reassign(Community const& cms) {
    for(int i = 0; i < c.size(); i++) {
        c[i] = cms.c[c[i]];
    }
}

void Community::most_freq_label(int v, std::vector<int> const& N, int & rc, int & rf) {
    if(N.empty()) {
        rf = 0;
        rc = c[v];
        return;
    }
    std::map<int, int> freq;

    for(int vi : N) {
        auto it = freq.find(c[vi]);
        if(it != freq.end()) {
            it->second++;
        }
        else {
            freq[c[vi]] = 1;
        }
    }

    std::vector<int> tie_vals;
    int c_max = -1;
    int f_max = 0;

    for(auto const& fi : freq) {
        if(fi.second > f_max) {
            c_max = fi.first;
            f_max = fi.second;
            tie_vals.clear();
            tie_vals.push_back(c_max);
        }
        else if(fi.second == f_max) {
            tie_vals.push_back(fi.first);
        }
    }

    if(tie_vals.size() == 1) {
        rc = c_max;
        rf = f_max;
    }
    else if(tie_vals.size() > 1) {
        rf = f_max;
        for(int i = 0; i < tie_vals.size(); i++) {
            if(tie_vals[i] == c[v]) {
                rc = c[v];
                return;
            }
        }
        rc = tie_vals[b_rand(tie_vals.size())];
    }
    else if(tie_vals.empty()) {
        std::cout << "error: tie_vals empty (" << c_max << ", " << f_max << ", " << N.size() << ")\n";
    }
}

double modularity(Graph const& g, Community const& c) {
    double deg_sum = 0;
    double w_sum = 0;
    for(int i = 0; i < g.size(); i++) {
        deg_sum += g.deg(i);
        //for(int j = i + 1; j < g.size(); j++) {
        for(int j = 0; j < g.size(); j++) {
            w_sum += g.get_edge(i, j);
        }
    }
    deg_sum *= deg_sum;

    double res = 0;
    for(int comm : c.get_communities()) {
        auto v = c.get_community_nodes(comm);

        double c_deg_sum = 0;
        double c_w_sum = 0;

        for(int i = 0; i < v.size(); i++) {
            c_deg_sum += g.deg(v[i]);

            //for(int j = i + 1; j < v.size(); j++) {
            for(int j = 0; j < v.size(); j++) {
                c_w_sum += g.get_edge(v[i], v[j]);
            }
        }

        c_deg_sum *= c_deg_sum;

        res += (c_w_sum / w_sum) - (c_deg_sum / deg_sum);
    }

    return res;
}

Community oneLevel(Graph const& g) {
    Community c(g.size());

    bool changes;
    int count = 0;
    double const sum_deg = g.deg_sum();

    do {
        changes = false;

        for(int i = 0; i < g.size(); i++) {
            double bestinc = 0;
            int c_orig = c.c[i];

            for(int comm : c.get_communities()) {
                //if(c.c[i] != comm && c.is_connected(g, i, comm)) {
                if(c.is_connected(g, i, comm)) {
                    double inc = 0;
                    double c_sum_deg = 0;
                    //double sum_deg = 0;
                    double w_sum = 0;
                    double const deg_i = g.deg(i);
                    //double const deg_i = g.arity(i);

                    auto v = c.get_community_nodes(comm);

                    for(int j = 0; j < v.size(); j++) {
                        //if(g.has_edge(i, v[j])) {
                            //w_sum += 1;
                        //}
                        //c_sum_deg += g.arity(v[j]);
                        w_sum += g.get_edge(i, v[j]);
                        c_sum_deg += g.deg(v[j]);
                    }

                    //for(int j = 0; j < g.size(); j++) {
                        //sum_deg += g.deg(j);
                        //sum_deg += g.arity(j);
                    //}

                    inc = w_sum - deg_i * c_sum_deg / sum_deg;

                    if(inc > bestinc) {
                        c.c[i] = comm;
                        bestinc = inc;
                        changes = comm != c_orig;
                    }
                }
            }
        }
        count++;
        std::cout << "oneLevel it " << count << "\n";
    } while(changes);

    return c;
}

Graph fold(Graph const& g1, Community const& cms) {
    int nb_c = cms.nb_communities();
    Graph g2(nb_c);

    for(int c1 = 0; c1 < nb_c; c1++) {
        for(int c2 = c1 + 1; c2 < nb_c; c2++) {
            double sum = 0;
            for(int i : cms.get_community_nodes(c1)) {
                for(int j : cms.get_community_nodes(c2)) {
                    sum += g1.get_edge(i, j);
                }
            }
            g2.add_edge(c1, c2, sum);
        }
    }

    return g2;
}

Graph build_graph(CNF const& cnf, std::map<Variable, std::size_t> const& m) {
    Graph g(m.size());

    for(std::size_t k = 0; k < cnf.nb_clauses(); k++) {
        if(cnf.is_active(k)) {
            auto const& cls = cnf.clause(k);

            if(cls.size() >= 2) {
                double const cw = 2.0 / (cls.size() * (cls.size() - 1));

                for(int i = 0; i < cls.size(); i++) {
                    std::size_t ii = m.at(Variable(cls[i]));
                    for(int j = i + 1; j < cls.size(); j++) {
                        std::size_t ij = m.at(Variable(cls[j]));
                        double const tmp = cw + g.get_edge(ii, ij);
                        g.add_edge(ii, ij, tmp);
                    }
                }
            }
        }
    }

    return g;
}

void remove_clause_from_graph(Graph & g, Clause const& cls, std::map<Variable, std::size_t> const& m) {
    if(cls.size() >= 2) {
        double const cw = 2.0 / (cls.size() * (cls.size() - 1));

        for(int i = 0; i < cls.size(); i++) {
            std::size_t ii = m.at(Variable(cls[i]));
            for(int j = i + 1; j < cls.size(); j++) {
                std::size_t ij = m.at(Variable(cls[j]));
                double const tmp = g.get_edge(ii, ij) - cw;
                g.add_edge(ii, ij, tmp);
            }
        }
    }
}

void add_clause_to_graph(Graph & g, Clause const& cls, std::map<Variable, std::size_t> const& m) {
    if(cls.size() >= 2) {
        double const cw = 2.0 / (cls.size() * (cls.size() - 1));

        for(int i = 0; i < cls.size(); i++) {
            std::size_t ii = m.at(Variable(cls[i]));
            for(int j = i + 1; j < cls.size(); j++) {
                std::size_t ij = m.at(Variable(cls[j]));
                double const tmp = g.get_edge(ii, ij) + cw;
                g.add_edge(ii, ij, tmp);
            }
        }
    }
}

void gfa(CNF const& cnf) {
    auto m = cnf.get_reduced_mapping();
    Graph g1 = build_graph(cnf, m);
    Graph g2 = g1;
    Community L1(g1.size());

    Community L2 = oneLevel(g2);
    L2.reassign();
    while(modularity(g1, L1) < modularity(g2, L2)) {
        g1 = g2;
        L1 = L2;
        g2 = fold(g2, L2);
        L2 = oneLevel(g2);
        L2.reassign();
    }

    std::cout << "q " << modularity(g1, L1) << "\n";
    std::cout << "s " << L1.nb_communities() << "\n";
}

LPA_res lpa(CNF const& cnf, std::map<Variable, std::size_t> const& m, Graph const& g) {
    // auto m = cnf.get_reduced_mapping();
    // Graph g = build_graph(cnf, m);
    Community L(g.size());
    std::vector<int> freq(g.size(), 0);

    std::vector<int> nodes(g.size());
    for(int i = 0; i < nodes.size(); i++) {
        nodes[i] = i;
    }

    bool changes = false;
    int count = 0;

    do {
        std::random_shuffle(nodes.begin(), nodes.end());
        changes = false;

        for(int i : nodes) {
            int l = -1;
            int f = -1;

            L.most_freq_label(i, g.neighbors(i), l, f);
            //changes = changes || f > freq[i];
            //L.c[i] = l;
            //freq[i] = f;
            //
            if(l != L.c[i]) {
                L.c[i] = l;
                changes = true;
            }
        }
        count++;
    } while(changes);

    L.reassign();

    // auto tmp = L.max_comm_id();
    // std::set<Variable> vars;

    // for(auto const& e : m) {
    //     if(L.c[e.second] == tmp) {
    //         vars.insert(Variable(e.first));
    //     }
    // }

    // auto cl_by_vars = cnf.get_clauses_by_vars(vars, 3);
    // auto cl_by_vars_w = cnf.get_clauses_by_vars_wide(vars, 3);

    double const modul_q = modularity(g, L);

    // std::cout << "i " << count << "\n";
    // std::cout << "q " << modul_q << "\n";
    // std::cout << "nb comm " << L.nb_communities() << "\n";
    // std::cout << "smallest comm size " << L.min_comm_size() << "\n";
    // std::cout << "biggest comm size " << L.max_comm_size() << "\n";
    // std::cout << "average comm size " << L.avg_comm_size() << "\n";
    // std::cout << "biggest comm #c (narrow) " << cl_by_vars.size() << "\n";
    // std::cout << "biggest comm #c (wide) " << cl_by_vars_w.size() << "\n";
    // std::cout << "biggest comm #v " << vars.size() << "\n";

    // std::cout << "\n";


    // std::cout << "c id, #v, #c, #cw\n";
    // for(auto i = 0; i < L.nb_communities(); i++) {
    //     vars.clear();

    //     for(auto const& e : m) {
    //         if(L.c[e.second] == i) {
    //             vars.insert(Variable(e.first));
    //         }
    //     }
    //     double const X1 = 500;
    //     double const Y1 = 0;
    //     double const X2 = 510;
    //     double const Y2 = 0.01;

    //     double const a = (Y1 - Y2) / (X1 - X2);
    //     double const b = Y1 - a * X1;

    //     double const tol = std::max(0.0, std::min(1.0, (vars.size() * a) + b));
    //     double const rt = 2;

    //     cl_by_vars = cnf.get_clauses_by_vars(vars, 3);
    //     cl_by_vars_w = cnf.get_clauses_by_vars_wide(vars, 3);

    //     double const r = cl_by_vars.size() / (double)vars.size();

    //     // std::cout << "c " << i;
    //     // std::cout << ", " << vars.size();
    //     // std::cout << ", " << cl_by_vars.size();
    //     // std::cout << ", " << cl_by_vars_w.size();
    //     // std::cout << "\n";

    //     // if(vars.size() >= 150 && r <= rt + tol && rt - tol <= r) {
    //     //     std::cout << "s " << i;
    //     //     std::cout << ", " << vars.size();
    //     //     std::cout << ", " << cl_by_vars.size();
    //     //     std::cout << "\n";
    //     // }
    // }
    //return modul_q;
    LPA_res f_res;
    f_res.mod = modul_q;
    f_res.L = L;
    return f_res;
}

int community_clause_count(std::map<Variable, std::size_t> const& m, CNF const& cnf, Community const& cms, std::set<int> const& c) {
    int nb = 0;

    for(int i = 0; i < cnf.nb_clauses(); i++) {
        if(cnf.is_active(i)) {
            auto const& cls = cnf.clause(i);

            bool ok = std::all_of(cls.begin(), cls.end(), [&](auto const& l) {
                    return c.find(cms.c[m.at(Variable(l))]) != c.end();
                    });

            if(ok) {
                nb += 1;
            }
        }
    }

    return nb;
}

bool merge_commuities_by_level(std::map<Variable, std::size_t> const& m, CNF const& cnf, Community & cms, int const lvl, int const min_c_count) {

    using PR = std::pair<int, int>;
    std::map<PR, int> ben;
    std::vector<PR> vben;

    std::map<int, std::size_t> c_sizes = cms.community_sizes();
    int c_count = cms.nb_communities();

    for(int i = 0; i < cms.nb_communities() - 1; i++) {
        if(c_sizes[i] <= lvl) {
            for(int j = i + 1; j < cms.nb_communities(); j++) {
                if(c_sizes[j] <= lvl) {
                    auto tmp = std::make_pair(i, j);
                    auto si = community_clause_count(m, cnf, cms, {i});
                    auto sj = community_clause_count(m, cnf, cms, {j});
                    auto sm = community_clause_count(m, cnf, cms, {i, j});

                    ben[tmp] = sm - si - sj;
                    vben.push_back(tmp);
                }
            }
        }
    }

    /*
     * sorts vben such that a < b in vben if ben[a] < ben[b]
     * thus the highest scores are last in vben
     */
    std::sort(vben.begin(), vben.end(), [&](PR const& a, PR const& b) {
                return ben[a] < ben[b];
                });


    while(vben.size() > 0 && c_count >= min_c_count) {
        auto item = vben[vben.size() - 1];

        cms.merge(item.first, item.second);
        c_count -= 1;

        auto it = std::remove_if(vben.begin(), vben.end(), [&](PR const& p) {
                return p.first == item.first
                    || p.first == item.second
                    || p.second == item.first
                    || p.second == item.second;
                });

        vben.erase(it, vben.end());
    }

    return c_count > min_c_count;
}
