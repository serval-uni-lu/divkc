#include<iostream>
#include<string>
#include<map>
#include<set>
#include<vector>
#include<fstream>
#include<sstream>

#include "CNF.hpp"

class PartitionInfo {
    public:
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
                }
                else if(line.find("p ", 0) == 0) {
                }
            }
        }
};

int main(int argc, char** argv) {
    std::cout << "hi\n";
    return 0;
}
