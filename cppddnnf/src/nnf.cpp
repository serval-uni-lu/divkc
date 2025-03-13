//
// Created by oz on 2/3/22.
//

#include "nnf.hpp"

#include <fstream>
#include <sstream>
#include <stack>
#include <iostream>
#include <algorithm>
#include <utility>
#include <map>
#include <cmath>

std::pmr::unsynchronized_pool_resource MEM_POOL;

NNF::NNF(std::string const& path) {
    std::set<Variable> pvar;
    init(path, false, pvar);
}

NNF::NNF(std::string const& path, std::set<Variable> const& pvar) {
    init(path, true, pvar);
}

void NNF::init(std::string const& path, bool project, std::set<Variable> const& pvar) {
    std::ifstream f(path);

    if(!f) {
        std::cerr << "Error opening file: " << path;
        std::cerr << "\nexiting\n";
        exit(1);
    }

    std::string line;
    Node t;
    t.type = True;
    nodes.push_back(t);

    while(getline(f, line)) {
        std::stringstream stream(line);
        std::string tag;
        stream >> tag;

        if(tag == "t") {
            Node n;
            n.type = True;
            nodes.push_back(n);
        }
        else if(tag == "f") {
            Node n;
            n.type = False;
            nodes.push_back(n);
        }
        else if(tag == "a") {
            Node n;
            n.type = And;
            nodes.push_back(n);
        }
        else if(tag == "o") {
            Node n;
            n.type = Or;
            nodes.push_back(n);
        }
        else if(tag == "u") {
            Node n;
            n.type = Unary;
            nodes.push_back(n);
        }
        else {
            Edge e;
            std::size_t src = std::stoi(tag);
            stream >> tag;
            e.id_dst = std::stoi(tag);

            bool phase = false;
            while (stream >> tag) {
                if(tag == "0") {

                }
                else if(tag == ";") {
                    phase = true;
                }
                else if(phase) {
                    auto v = Variable(std::stoi(tag));
                    if(!project || pvar.find(v) != pvar.end()) {
                        e.freeVars.push_back(v);
                    }
                }
                else {
                    auto l = Literal(std::stoi(tag));
                    if(!project || pvar.find(Variable(l)) != pvar.end()) {
                        e.units.push_back(l);
                    }
                }
            }

            nodes[src].children.push_back(e);
        }
    }

    root = 1;
    f.close();

    compute_ordering();
}

std::size_t NNF::nb_nodes() const {
    return nodes.size();
}

std::size_t NNF::nb_edges() const {
    std::size_t res = 0;

    for(Node const& n : nodes) {
        res += n.children.size();
    }

    return res;
}

mpz_int m_pow(std::size_t const& e, std::size_t const& p) {
    mpz_int r = 1;

    for(std::size_t i = 0; i < p; i++) {
        r *= e;
    }

    return r;
}

mpz_int NNF::mc(std::size_t id) const {
    return nodes[id].mc;
}

mpz_int NNF::mc() const {
    return mc(root);
}

mpz_int NNF::get_mc(Edge const& e) const {
    for(auto const& l : e.units) {
        if(assumps.find(~l) != assumps.end()) {
            return 0;
        }
    }
    int nfree = e.freeVars.size();
    for(auto const& v : e.freeVars) {
        auto const l = Literal(v, 0);
        if(assumps.find(l) != assumps.end() || assumps.find(~l) != assumps.end()) {
            nfree -= 1;
        }
    }

    return nodes[e.id_dst].mc * m_pow(2, nfree);
}

mpz_int NNF::get_pc(Edge const& e) const {
    for(auto const& l : e.units) {
        if(assumps.find(~l) != assumps.end()) {
            return 0;
        }
    }
    return nodes[e.id_dst].mc;
}

void NNF::annotate_mc() {
    for(auto const& nid : ordering) {
        Node & n = nodes[nid];
        n.mc = 0;

        if(n.type == True) {
            n.mc = 1;
        }
        else if(n.type == False) {
            n.mc = 0;
        }
        else if(n.type == And) {
            n.mc = 1;
            for(auto const& i : n.children) {
                n.mc *= get_mc(i);
            }
        }
        else if(n.type == Or || n.type == Unary) {
            for(auto const& i : n.children) {
                n.mc += get_mc(i);
            }
        }
    }
}

void NNF::annotate_pc() {
    for(auto const& nid : ordering) {
        Node & n = nodes[nid];
        n.mc = 0;

        if(n.type == True) {
            n.mc = 1;
        }
        else if(n.type == False) {
            n.mc = 0;
        }
        else if(n.type == And) {
            n.mc = 1;
            for(auto const& i : n.children) {
                n.mc *= get_pc(i);
            }
        }
        else if(n.type == Or || n.type == Unary) {
            for(auto const& i : n.children) {
                n.mc += get_pc(i);
            }
        }
    }
}

void NNF::compute_ordering() {
    ordering.clear();
    std::vector<std::size_t> stack;
    std::set<std::size_t> visited;

    stack.push_back(root);

    while(stack.size() > 0) {
        auto num = stack[stack.size() - 1];
        if(visited.find(num) == visited.end()) {
            bool all_done = true;
            for(auto const& e : nodes[num].children) {
                if(visited.find(e.id_dst) == visited.end()) {
                    all_done = false;
                    stack.push_back(e.id_dst);
                }
            }

            if(all_done) {
                stack.pop_back();
                visited.insert(num);
                ordering.push_back(num);
            }
        }
        else {
            stack.pop_back();
        }
    }
}

void NNF::set_assumps(std::set<Literal> const& a) {
    assumps = a;
}

void NNF::set_assumps(std::vector<Literal> const& a) {
    assumps.clear();
    assumps.insert(a.begin(), a.end());
}

void NNF::get_path(mpz_int const& id, std::vector<Literal> & s) const {
    std::vector<std::pair<mpz_int, std::size_t> > stack;
    stack.push_back(std::pair(id, root));
    s.clear();

    while(stack.size() > 0) {
        auto lid = std::get<0>(stack[stack.size() - 1]);
        auto nid = std::get<1>(stack[stack.size() - 1]);
        stack.pop_back();

        Node const& n = nodes[nid];
        if(n.type == And) {
            auto tmc = mc(nid);
            if(lid > tmc) {
                std::cerr << "a " << lid << " <= " << tmc << "\n";
            }

            for(auto const& c : n.children) {
                auto cmc = get_pc(c);

                auto x = ((lid - 1) % cmc) + 1;
                auto y = ((lid - 1) / cmc) + 1;

                stack.emplace_back(x, c.id_dst);
                lid = y;
            }
        }
        else if(n.type == Or || n.type == Unary) {
            if(lid > mc(nid)) {
                std::cerr << "o " << lid << " <= " << mc(nid) << "\n";
            }

            for(auto const& c : n.children) {
                auto cmc = get_pc(c);

                if(lid <= cmc) {
                    for(auto const& l : c.units) {
                        s.push_back(l);
                    }

                    stack.emplace_back(lid, c.id_dst);
                    break;
                }
                else {
                    lid -= cmc;
                }
            }
        }
    }
}

void NNF::get_solution(mpz_int const& id, std::vector<Literal> & s) const {
    std::vector<std::pair<mpz_int, std::size_t> > stack;
    stack.push_back(std::pair(id, root));
    s.clear();

    while(stack.size() > 0) {
        auto lid = std::get<0>(stack[stack.size() - 1]);
        auto nid = std::get<1>(stack[stack.size() - 1]);
        stack.pop_back();

        Node const& n = nodes[nid];
        if(n.type == And) {
            auto tmc = mc(nid);
            if(lid > tmc) {
                std::cerr << "a " << lid << " <= " << tmc << "\n";
            }

            for(auto const& c : n.children) {
                auto cmc = get_mc(c);

                auto x = ((lid - 1) % cmc) + 1;
                auto y = ((lid - 1) / cmc) + 1;

                stack.emplace_back(x, c.id_dst);
                lid = y;
            }
        }
        else if(n.type == Or || n.type == Unary) {
            if(lid > mc(nid)) {
                std::cerr << "o " << lid << " <= " << mc(nid) << "\n";
            }

            for(auto const& c : n.children) {
                auto cmc = get_mc(c);

                if(lid <= cmc) {
                    for(auto const& l : c.units) {
                        s.push_back(l);
                    }

                    for(auto const& v : c.freeVars) {
                        auto const l = Literal(v, 0);
                        if(assumps.find(l) != assumps.end()) {
                            s.push_back(l);
                        }
                        else if(assumps.find(~l) != assumps.end()) {
                            s.push_back(~l);
                        }
                        else if(lid <= (cmc / 2)) {
                            s.push_back(l);
                            cmc /= 2;
                        }
                        else {
                            cmc /= 2;
                            lid -= cmc;
                            s.push_back(~l);
                        }
                    }

                    stack.emplace_back(lid, c.id_dst);
                    break;
                }
                else {
                    lid -= cmc;
                }
            }
        }
    }
}
