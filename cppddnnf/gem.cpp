#include <filesystem>
#include <iostream>
#include <vector>
#include <set>
#include <string>

namespace fs = std::filesystem;

struct SourceFile {
    std::string src_path;
    std::string obj_path;
    std::string elf_path;
};

std::string gen_rule(SourceFile const& s) {
    std::string res;
    res += s.obj_path + ": mkfolders gem " + s.src_path + "\n";
    res += "\t$(CXX) $(CXXFLAGS) $(CXXFLAGSREL) -c " + s.src_path + " -o " + s.obj_path + "\n";

    res += s.obj_path + "d: mkfolders gem " + s.src_path + "\n";
    res += "\t$(CXX) $(CXXFLAGS) $(CXXFLAGSDEBUG) -c " + s.src_path + " -o " + s.obj_path + "d\n";
    return res;
}

std::string gen_ld_rule(SourceFile const& s, std::vector<SourceFile> const& deps) {
    std::string res;
    res += s.elf_path + ".r: " + s.obj_path;
    for(auto const& c : deps) {
        if(c.elf_path == "") {
            res += " " + c.obj_path;
        }
    }
    res += "\n\t$(CXX) $(LDFLAGS) $(LDFLAGSREL) $? -o " + s.elf_path + ".r\n";

    res += s.elf_path + ".d: " + s.obj_path;
    for(auto const& c : deps) {
        if(c.elf_path == "") {
            res += " " + c.obj_path + "d";
        }
    }
    res += "\n\t$(CXX) $(LDFLAGS) $(LDFLAGSDEBUG) $? -o " + s.elf_path + ".d\n";
    return res;
}

std::string gen_variable(std::string const& var, std::vector<std::string> const& vals) {
    std::string res = var + " ::=";
    for(auto const& v : vals) {
        res += " " + v;
    }
    return res;
}

class Project {
private:
    std::set<std::string> obj_folders;

    std::vector<SourceFile> src;
    std::vector<std::string> headers;
    std::vector<std::string> cc_options;
    std::vector<std::string> ld_options;

    std::vector<std::string> cc_r_options;
    std::vector<std::string> ld_r_options;

    std::vector<std::string> cc_d_options;
    std::vector<std::string> ld_d_options;

public:
    void add_source_folder(std::string const& path, std::set<std::string> const& extensions) {
        for(auto const& f : fs::recursive_directory_iterator(path)) {
            if(f.is_regular_file()) {
                if(extensions.find(f.path().extension()) != extensions.end()) {
                    add_source_file(f.path());
                }
            }
        }
    }

    void add_source_file(std::string const& path, bool const is_main = false) {
        SourceFile f;
        f.src_path = path;
        f.obj_path = "obj/" + path + ".o";
        if(is_main) {
            f.elf_path = fs::path(path).stem().string();
        }
        src.push_back(f);
        obj_folders.insert(fs::path(f.obj_path).parent_path());
    }

    void set_main_property(std::string const& path, bool const is_main = true) {
        for(auto & e : src) {
            if(e.src_path == path) {
                if(is_main) {
                    e.elf_path = fs::path(path).stem().string();
                }
                else {
                    e.elf_path = "";
                }
            }
        }
    }

    void add_header_file(std::string const& path) {
        headers.push_back(path);
    }

    void add_compile_option(std::string const& o) {
        cc_options.push_back(o);
    }

    void add_linker_option(std::string const& o) {
        ld_options.push_back(o);
    }

    void add_realease_compile_option(std::string const& o) {
        cc_r_options.push_back(o);
    }

    void add_release_linker_option(std::string const& o) {
        ld_r_options.push_back(o);
    }

    void add_debug_compile_option(std::string const& o) {
        cc_d_options.push_back(o);
    }

    void add_debug_linker_option(std::string const& o) {
        ld_d_options.push_back(o);
    }

    void gen_makefile() {
        std::cout << gen_variable("CXXFLAGS", cc_options) << "\n";
        std::cout << gen_variable("LDFLAGS", ld_options) << "\n";
        std::cout << gen_variable("CXXFLAGSREL", cc_r_options) << "\n";
        std::cout << gen_variable("LDFLAGSREL", ld_r_options) << "\n";
        std::cout << gen_variable("CXXFLAGSDEBUG", cc_d_options) << "\n";
        std::cout << gen_variable("LDFLAGSDEBUG", ld_d_options) << "\n";

        std::cout << "\n";

        std::cout << "all:";
        for(auto const& f : src) {
            if(f.elf_path != "") {
                std::cout << " " << f.elf_path << ".r";
            }
        }

        std::cout << "\ndebug:";
        for(auto const& f : src) {
            if(f.elf_path != "") {
                std::cout << " " << f.elf_path << ".d";
            }
        }

        std::cout << "\n.PHONY: all mkfolders clean cleanall\n";
        std::cout << "mkfolders:\n";
        std::cout << "\tmkdir -p";

        for(auto const& f : obj_folders) {
            std::cout << " " << f;
        }
        std::cout << "\nclean:\n\trm -rf obj";
        std::cout << "\ncleanall: clean\n\trm -f *.r *.d";
        std::cout << "\ngem: gem.cpp\n";
        std::cout << "\tg++ gem.cpp -o gem\n";
        std::cout << "\t./gem > Makefile\n";
        std::cout << "\t@echo \"Had to regenerate the makefile. Please rerun make.\"\n\texit 1";
        std::cout << "\n\n";


        for(auto const& cpp : src) {
            std::cout << gen_rule(cpp);

            if(cpp.elf_path != "") {
                std::cout << gen_ld_rule(cpp, src);
            }
            std::cout << "\n";
        }
    }
};

int main(void) {
    Project prj;
    prj.add_compile_option("-Wall");
    prj.add_compile_option("-Wextra");
    prj.add_compile_option("-Wpedantic");
    prj.add_compile_option("-fopenmp");

    prj.add_realease_compile_option("-O3");

    prj.add_debug_compile_option("-g");

    prj.add_linker_option("-lboost_random");
    prj.add_linker_option("-lgmp");
    prj.add_linker_option("-fopenmp");

    prj.add_source_folder("src", {".cpp"});
    // prj.add_source_file("src/pdac.cpp");
    // prj.add_source_file("src/nnf.cpp");
    // prj.add_source_file("src/var.cpp");
    prj.set_main_property("src/pc.cpp");
    prj.set_main_property("src/rsampler.cpp");
    prj.set_main_property("src/lmc.cpp");
    prj.set_main_property("src/appmc.cpp");
    prj.set_main_property("src/sampler.cpp");

    prj.gen_makefile();
    
    return 0;
}
