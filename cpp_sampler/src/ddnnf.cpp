#include "ddnnf.hpp"

#include<cstdlib>
#include<cassert>

#include<iostream>

#include<boost/algorithm/string.hpp>

int64_t constexpr FreeVars_BS = 1 << 20;
int64_t constexpr UnitLits_BS = 1 << 20;

Variable * Edge::freeVars;
int64_t Edge::szFreeVars;
int64_t Edge::capFreeVars;

Literal * Edge::unitLits;
int64_t Edge::szUnitLits;
int64_t Edge::capUnitLits;

void Edge::init() {
    freeVars = static_cast<Variable*>(malloc(sizeof(Variable) * FreeVars_BS));

    if(freeVars == NULL) {
        std::cerr << "Edge::freeVars alloc error, exiting\n";
        std::exit(EXIT_FAILURE);
    }

    szFreeVars = 0;
    capFreeVars = FreeVars_BS;

    unitLits = static_cast<Literal*>(malloc(sizeof(Literal) * UnitLits_BS));

    if(unitLits == NULL) {
        std::cerr << "Edge::unitLits alloc error, exiting\n";
        std::exit(EXIT_FAILURE);
    }

    szUnitLits = 0;
    capUnitLits = UnitLits_BS;
}

void Edge::free() {
    ::free(freeVars);
    ::free(unitLits);

    szFreeVars = 0;
    capFreeVars = 0;

    szUnitLits = 0;
    capUnitLits = 0;
}

void Edge::resizeFreeVars(int64_t nc) {
    if(nc > capFreeVars) {
        auto tmp = static_cast<Variable*>(realloc(freeVars, sizeof(Variable) * nc));

        if(tmp == NULL) {
            std::cerr << "Edge::freeVars realloc error, exiting\n";
            ::free(freeVars);
            ::free(unitLits);
            std::exit(EXIT_FAILURE);
        }

        freeVars = tmp;
        capFreeVars = nc;
    }
}

void Edge::resizeUnitLits(int64_t nc) {
    if(nc > capUnitLits) {
        auto tmp = static_cast<Literal*>(realloc(unitLits, sizeof(Literal) * nc));

        if(tmp == NULL) {
            std::cerr << "Edge::unitLits realloc error, exiting\n";
            ::free(freeVars);
            ::free(unitLits);
            std::exit(EXIT_FAILURE);
        }

        unitLits = tmp;
        capUnitLits = nc;
    }
}

int64_t Edge::save(std::vector<Variable> const& v) {
    int64_t sid = szFreeVars;
    szFreeVars += v.size();

    if(szFreeVars > capFreeVars) {
        resizeFreeVars(capFreeVars + FreeVars_BS);
    }

    //for(auto i = 0; i < v.size(); i++) {
    //    freeVars[sid + i] = v[i];
    //}
    memcpy(freeVars + sid, v.data(), sizeof(Variable) * v.size());

    return sid;
}

int64_t Edge::save(std::vector<Literal> const& l) {
    int64_t sid = szUnitLits;
    szUnitLits += l.size();

    if(szUnitLits > capUnitLits) {
        resizeFreeVars(capUnitLits + UnitLits_BS);
    }

    //for(auto i = 0; i < l.size(); i++) {
    //    unitLits[sid + i] = l[i];
    //}
    memcpy(unitLits + sid, l.data(), sizeof(Literal) * l.size());

    return sid;
}

Edge::Edge(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f) {
    this->target = target;

    b_cnst = save(consts);
    e_cnst = b_cnst + consts.size();

    b_free = save(f);
    e_free = b_free + f.size();
}

void AndNode::add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f) {
    assert(consts.empty());
    assert(f.empty());

    children.emplace_back(target, consts, f);
}

void AndNode::annotate_mc() {
    if(children.empty()) {
        mc = 0;
    }
    else {
        mc = 1;

        for(auto const& i : children) {
            mc *= i.target->mc;
        }
    }
}


void OrNode::add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f) {
    children.emplace_back(target, consts, f);
}

void OrNode::annotate_mc() {
    if(children.empty()) {
        mc = 1;
    }
    else {
        mc = 0;

        for(auto const& i : children) {
            mc += i.target->mc * (mpz_int(1) << i.nb_free());
        }
    }
}

void UnaryNode::add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f) {
    assert(children.empty());

    children.emplace_back(target, consts, f);
}

void UnaryNode::annotate_mc() {
    assert(children.size() == 1);

    if(children.empty()) {
        mc = 1;
    }
    else {
        mc = 0;

        for(auto const& i : children) {
            mc += i.target->mc * (mpz_int(1) << i.nb_free());
        }
    }
}

void TrueNode::add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f) {
    assert(false);
}

void TrueNode::annotate_mc() {
    mc = 1;
}

void FalseNode::add_child(Node const* target, std::vector<Literal> const& consts, std::vector<Variable> const& f) {
    assert(false);
}

void FalseNode::annotate_mc() {
    mc = 0;
}

void DDNNF::compute_ordering() {
    ordering.clear();
    ordering.reserve(nodes.size());

    std::vector<bool> visited(nodes.size(), false);

    std::vector<int64_t> stack;
    stack.reserve(nodes.size());
    stack.push_back(1);

    while(! stack.empty()) {
        int64_t num = stack[stack.size() - 1];

        if(!visited[num - 1]) {
            Node const * n = get_node(num);

            bool all_done = true;

            for(auto const& c : n->children) {
                if(! visited[c.target->num - 1]) {
                    all_done = false;
                    stack.push_back(c.target->num);
                }
            }

            if(all_done) {
                stack.pop_back();
                visited[num - 1] = true;
                ordering.push_back(num);
            }
        }
        else {
            stack.pop_back();
        }
    }
}

inline int64_t get_node_num(std::string const& l) {
    std::stringstream iss(l);
    std::string tmp;
    iss >> tmp;

    int64_t num;
    iss >> num;

    return num;
}

DDNNF::DDNNF(std::string const& path) {
    std::ifstream f(path);

    std::string line;

    while(std::getline(f, line)) {
        if(line.find("o ", 0) == 0) {
            int64_t num = get_node_num(line);
            add_node(new OrNode(num));
        }
        else if(line.find("a ", 0) == 0) {
            int64_t num = get_node_num(line);
            add_node(new AndNode(num));
        }
        else if(line.find("u ", 0) == 0) {
            int64_t num = get_node_num(line);
            add_node(new UnaryNode(num));
        }
        else if(line.find("t ", 0) == 0) {
            int64_t num = get_node_num(line);
            add_node(new TrueNode(num));
        }
        else if(line.find("f ", 0) == 0) {
            int64_t num = get_node_num(line);
            add_node(new FalseNode(num));
        }
        else if(line.find("c ", 0) == std::string::npos) {
            std::vector<std::string> strs;
            boost::split(strs, line, boost::is_any_of(";"));

            std::vector<std::string> strs1;
            boost::split(strs1, strs[0], boost::is_any_of(" "));

            std::vector<std::string> strs2;
            if(strs.size() > 1 && !strs[1].empty())
                boost::split(strs2, strs[1], boost::is_any_of(" "));

            std::vector<Literal> units;
            std::vector<Variable> f;

            for(int i = 2; i < strs1.size(); i++) {
                if(!strs1[i].empty()) {
                    int64_t t = std::stoi(strs1[i]);
                    if(t != 0)
                        units.emplace_back(t);
                }
            }


            for(int i = 0; i < strs2.size(); i++) {
                if(!strs2[i].empty()) {
                    int64_t t = std::stoi(strs2[i]);
                    if(t != 0)
                        f.emplace_back(t);
                }
            }

            Node * src = get_node(std::stoi(strs1[0]));
            Node * dst = get_node(std::stoi(strs1[1]));

            src->add_child(dst, units, f);
        }
    }
}
