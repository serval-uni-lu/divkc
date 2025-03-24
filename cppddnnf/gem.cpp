#include <filesystem>
#include <iostream>
#include <vector>
#include <string>

namespace fs = std::filesystem;

struct Dir {
    std::vector<fs::directory_entry> cpp;
    std::vector<fs::directory_entry> hpp;
};

Dir walk_directory(std::string const& path) {
    Dir res;

    for(auto const& f : fs::recursive_directory_iterator(path)) {
        if(f.is_regular_file()) {
            if(f.path().extension() == ".cpp") {
                res.cpp.push_back(f);
            }
            else if(f.path().extension() == ".hpp") {
                res.hpp.push_back(f);
            }
        }
    }

     return res;
}

std::string gen_rule(fs::path const& path) {
    std::string spath = path.string();
    std::string res;
    res += "obj/" + spath + ".o: " + spath + "\n";
    res += "\t$(CXX) $(CXXFLAGS) -c " + spath + " -o obj/" + spath + ".o\n";
    return res;
}

int main(void) {
    auto fl = walk_directory("src");
    std::cout << "CXX ::= g++\n";
    std::cout << "CXXFLAGS ::= -Wall\n\n";
    for(auto const& f : fl.cpp) {
        //std::cout << f.path() << "\n";
        std::cout << gen_rule(f.path()) << "\n";
    }
    return 0;
}
