#include<iostream>
#include<string>
#include<map>
#include<set>
#include<vector>
#include<fstream>
#include<sstream>
#include<chrono>

#include<omp.h>

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

#include "CNF.hpp"
#include "ddnnf.hpp"

using namespace boost::random;

struct PartitionInfo {
    std::vector<std::vector<bool> > part;
    std::vector<int> part_sizes;
    std::vector<std::string> files;

    PartitionInfo (int64_t const nb_vars, std::string const& path) {
        std::ifstream f(path);

        std::string line;

        while(std::getline(f, line)) {
            if(line.find("v ", 0) == 0) {
                std::stringstream iss(line);
                std::string tmp;
                iss >> tmp;

                int v, c;
                iss >> v >> c;

                while(part.size() <= c) {
                    part.push_back(std::vector<bool>(nb_vars, false));
                    part_sizes.push_back(0);
                    files.push_back("");
                }

                part[c][Variable(v).get()] = true;
                //auto tv = Variable(v);
                //while(part[c].size() < tv.get()) {
                //    part[c].push_back(false);
                //}
                //part[c].push_back(true);
                //part[c].insert(Variable(v));
            }
            else if(line.find("c p ", 0) == 0) {
                std::stringstream iss(line);
                std::string tmp;
                iss >> tmp >> tmp;

                int c, s;
                iss >> c >> s;

                while(part.size() <= c) {
                    part.push_back({});
                    part_sizes.push_back(0);
                    files.push_back("");
                }

                part_sizes[c] = s;
            }
            else if(line.find("p ", 0) == 0) {
                std::stringstream iss(line);
                std::string tmp;
                int c;
                iss >> tmp >> c >> tmp;

                files[c] = tmp;
            }
        }
    }

    int nb_communities() const {
        return part.size();
    }

};

std::ostream& operator<<(std::ostream & out, PartitionInfo const& pi) {
    for(int i = 0; i < pi.nb_communities(); i++) {
        out << i << "\n";

        out << "    ";
        //for(auto const& v : pi.part[i]) {
            //out << v << " ";
        //}
        for(int v = 0; v < pi.part[i].size(); v++) {
            if(pi.part[i][v]) {
                std::cout << v + 1 << " ";
            }
        }
        out << "\n";
        out << "    s " << pi.part_sizes[i] << "\n";
        out << "    " << pi.files[i] << "\n";

        out << "\n";
    }

    return out;
}

void sample(DDNNF const& nnf, std::vector<bool> & sample, mt19937 & mt) {
    std::vector<Node const*> stack;
    stack.reserve(nnf.nodes.size());

    stack.push_back(nnf.get_node(1));

    uniform_int_distribution<int64_t> bin_ui(0, 1);

    while(!stack.empty()) {
        Node const* n = stack[stack.size() - 1];
        stack.pop_back();

        if(n->get_type() == Node::Type::OrNode) {
            uniform_int_distribution<mpz_int> ui(1, n->mc);

            mpz_int key = ui(mt);

            for(auto const& c : n->children) {
                mpz_int tmc = c.target->mc * (mpz_int(1) << c.nb_free());

                if(key <= tmc) {
                    for(int64_t i = c.b_free; i < c.e_free; i++) {
                        sample[(2 * Edge::freeVars[i].get()) + bin_ui(mt)] = true;
                    }

                    for(int64_t i = c.b_cnst; i < c.e_cnst; i++) {
                        sample[Edge::unitLits[i].get()] = true;
                    }

                    stack.push_back(c.target);
                    break;
                }
                else {
                    key -= tmc;
                }
            }
        }
        else if(n->get_type() == Node::Type::AndNode) {
            for(auto const& c : n->children) {
                stack.push_back(c.target);
            }
        }
        else if(n->get_type() == Node::Type::UnaryNode) {
            auto const& c = n->children[0];

            for(int64_t i = c.b_free; i < c.e_free; i++) {
                sample[(2 * Edge::freeVars[i].get()) + bin_ui(mt)] = true;
            }

            for(int64_t i = c.b_cnst; i < c.e_cnst; i++) {
                sample[Edge::unitLits[i].get()] = true;
            }

            stack.push_back(c.target);
        }
        else if(n->get_type() == Node::Type::TrueNode) {
            //nothing to do here
        }
        else if(n->get_type() == Node::Type::FalseNode) {
            std::cerr << "Error, sampling from FalseNode, exiting\n";
            std::exit(EXIT_FAILURE);
        }
        else {
            std::cerr << "Error, unknown Node type during sampling, exiting\n";
            std::exit(EXIT_FAILURE);
        }
    }
}

bool is_valid_sample(std::vector<bool> const& s) {
    for(int i = 0; i < s.size(); i += 2) {
        if(s[i] == s[i + 1]) {
            std::cerr << "c e invalid sample\n";
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    auto ts = std::chrono::steady_clock::now();

    Edge::freeVars = nullptr;
    Edge::init();

    std::string const path(argv[1]);
    int64_t const nb_sampels = std::stoi(argv[2]);

    CNF cnf(path.c_str());
    PartitionInfo pi(cnf.nb_vars(), path + ".log");
//    std::cout << pi;

    std::vector<DDNNF> nnfs;

    mpz_int emc = 1;

    for(int64_t i = 0; i < pi.files.size(); i++) {
        std::string pnnf = path + "." + pi.files[i] + ".nnf";
        nnfs.emplace_back(pnnf, pi.part[i]);

        nnfs[nnfs.size() - 1].compute_ordering();
        nnfs[nnfs.size() - 1].annotate_mc();

        emc *= nnfs[nnfs.size() - 1].get_node(1)->mc;
    }

    std::cout << "c umc " << emc << "\n";
    std::cout << std::flush;

    int64_t samples = 0;
    int64_t nb_tries = 0;

    random_device rd;
    //mt19937 mt(rd);

    std::vector<mt19937> rngs;
    rngs.reserve(omp_get_max_threads());
    for(int i = 0; i < omp_get_max_threads(); i++) {
        rngs.emplace_back(rd);
    }

#pragma omp parallel for
    for(int i = 0; i < omp_get_max_threads(); i++) {
        while(samples < nb_sampels) {
#pragma omp atomic
            nb_tries++;

            std::vector<bool> cs(cnf.nb_vars() * 2, false);

            for(auto const& nnf : nnfs) {
                sample(nnf, cs, rngs[i]);
            }

            if(is_valid_sample(cs) && cnf.is_model(cs)) {
#pragma omp critical
                {
                    std::cout << "s";
                    for(int64_t i = 0; i < cs.size(); i++) {
                        if(cs[i]) {
                            std::cout << " " << (i & 1 ? -1 : 1) * ((i >> 1) + 1);
                        }
                    }
                    std::cout << "\n";

                    samples += 1;
                }
            }
        }
    }

    Edge::free();

    mpf_float r = (double)samples / nb_tries;
    std::cout << "c r " << r << "\n";
    std::cout << "c emc " << emc * r << "\n";

    std::chrono::duration<double> dur = std::chrono::steady_clock::now() - ts;
    std::cout << "c t " << dur.count() << "\n";

    return 0;
}
