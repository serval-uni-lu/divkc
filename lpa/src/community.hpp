#pragma once

#include<vector>
#include<set>
#include<map>

#include "graph.hpp"
#include "common.hpp"
#include "CNF.hpp"

struct Community {
    //map: graph node -> community number
    std::vector<int> c;

    Community(int size) : c(size) {
        for(int i = 0; i < size; i++) {
            c[i] = i;
        }
    }

    Community() : c(0) {
    }

    Community(Community const& cms) : c(cms.c) {}

    void merge(int c1, int c2);

    std::set<int> get_communities() const;

    inline int nb_communities() const {
        return get_communities().size();
    }

    std::map<int, std::size_t> community_sizes() const;

    std::size_t max_comm_size() const;

    int max_comm_id() const;

    inline std::vector<int> get_biggest_community() const {
        int id = max_comm_id();
        return get_community_nodes(id);
    }

    std::size_t min_comm_size() const;

    double avg_comm_size() const;

    std::vector<int> get_community_nodes(int comm) const;

    /*
     * check if there is an edge from node i to community comm in graph g
     */
    bool is_connected(Graph const& g, int i, int comm) const;

    /**
     * reassigns the community ids so that they can be used as
     * array indices later on
     */
    void reassign();

    /**
     * reassigns the communities based on cms
     * ex:
     * if a node x has community 2 in c
     * and if node 2 in cms has community 3
     * then node x gets moved to community 3
     */
    void reassign(Community const& cms);

    void most_freq_label(int v, std::vector<int> const& N, int & rc, int & rf);
};

double modularity(Graph const& g, Community const& c);

Community oneLevel(Graph const& g);

/**
 * the ids in cms have to be usable as array indices
 * in other words -> cms.reassign() has to be called if necessary
 */
Graph fold(Graph const& g1, Community const& cms);

Graph build_graph(CNF const& cnf, std::map<Variable, std::size_t> const& m);

void remove_clause_from_graph(Graph & g, Clause const& cls, std::map<Variable, std::size_t> const& m);

void add_clause_to_graph(Graph & g, Clause const& cls, std::map<Variable, std::size_t> const& m);

void gfa(CNF const& cnf);

struct LPA_res {
    Community L;
    double mod;
};

LPA_res lpa(CNF const& cnf, std::map<Variable, std::size_t> const& m, Graph const& g);

int community_clause_count(std::map<Variable, std::size_t> const& m, CNF const& cnf, Community const& cms, std::set<int> const& c);

bool merge_commuities_by_level(std::map<Variable, std::size_t> const& m, CNF const& cnf, Community & cms, int const lvl, int const min_c_count);
