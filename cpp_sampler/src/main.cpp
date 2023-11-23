#include<iostream>
#include<string>
#include<map>
#include<set>
#include<vector>
#include<fstream>
#include<sstream>

#include "CNF.hpp"
#include "ddnnf.hpp"

struct PartitionInfo {
    std::vector<std::set<Variable> > part;
    std::vector<int> part_sizes;
    std::vector<std::string> files;

    PartitionInfo (std::string const& path) {
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
                    part.push_back({});
                    part_sizes.push_back(0);
                    files.push_back("");
                }

                part[c].insert(Variable(v));
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
        for(auto const& v : pi.part[i]) {
            out << v << " ";
        }
        out << "\n";
        out << "    s " << pi.part_sizes[i] << "\n";
        out << "    " << pi.files[i] << "\n";

        out << "\n";
    }

    return out;
}

int main(int argc, char** argv) {
    Edge::freeVars = nullptr;
    Edge::init();

    std::string path(argv[1]);

    //PartitionInfo pi(path + ".log");

    //std::cout << pi;

    DDNNF nnf(path);
    nnf.compute_ordering();
    nnf.annotate_mc();

    std::cout << nnf.get_node(1)->mc << "\n";

    Edge::free();

    return 0;
}
