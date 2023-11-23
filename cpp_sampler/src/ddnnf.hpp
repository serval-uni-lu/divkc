#pragma once

#include<vector>
#include<string>
#include<fstream>
#include<sstream>

#include<boost/multiprecision/gmp.hpp>

#include "CNF.hpp"

// https://www.boost.org/doc/libs/1_73_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/gmp_int.html

struct Node;

class Edge {
    public:

    static Variable * freeVars;
    static int64_t szFreeVars;
    static int64_t capFreeVars;

    static Literal * unitLits;
    static int64_t szUnitLits;
    static int64_t capUnitLits;

    Node const* target;
    int64_t b_cnst, e_cnst;
    int64_t b_free, e_free;

    static void init();
    static void free();
    static void resizeFreeVars(int64_t nc);
    static void resizeUnitLits(int64_t nc);
    static int64_t save(std::vector<Variable> const& v);
    static int64_t save(std::vector<Literal> const& l);

    Edge(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f);

    inline int64_t nb_free() const {
        return e_free - b_free;
    }
};

using namespace boost::multiprecision;

struct Node {
    int64_t num;
    mpz_int mc;

    std::vector<Edge> children;

    Node(int64_t id) : num(id), mc(-1) {}

    virtual void add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f) = 0;

    virtual void annotate_mc() = 0;
};

struct AndNode : Node {
    AndNode(int64_t id) : Node(id) {}

    void add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f);

    void annotate_mc();
};

struct OrNode : Node {
    OrNode(int64_t id) : Node(id) {}

    void add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f);

    void annotate_mc();
};

struct UnaryNode : Node {
    UnaryNode(int64_t id) : Node(id) {}

    void add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f);

    void annotate_mc();
};

struct TrueNode : Node {
    TrueNode(int64_t id) : Node(id) {}

    void add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f);

    void annotate_mc();
};

struct FalseNode : Node {
    FalseNode(int64_t id) : Node(id) {}

    void add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f);

    void annotate_mc();
};

struct DDNNF {
    std::vector<Node*> nodes;
    std::vector<int64_t> ordering;

    DDNNF() = default;

    DDNNF(std::string const& path);
    DDNNF(std::string const& path, std::vector<bool> const& c);

    ~DDNNF() {
        for(auto const* n : nodes) {
            delete n;
        }
    }

    inline void add_node(Node * n) {
        assert(nodes.size() + 1 == n->num);

        nodes.push_back(n);
    }

    inline Node* get_node(int64_t const id) {
        return nodes[id - 1];
    }

    void compute_ordering();

    inline void annotate_mc() {
        for(int64_t i : ordering) {
            get_node(i)->annotate_mc();
        }
    }

    private:
        void parse(std::string const& path, bool const project, std::vector<bool> const& c);
};
