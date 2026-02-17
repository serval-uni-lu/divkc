//
// Created by oz on 2/3/22.
//

#ifndef CNNF_NNF_HPP
#define CNNF_NNF_HPP

#include<string>
#include<vector>
#include<set>
#include<memory_resource>

#include <boost/multiprecision/gmp.hpp>

#include "var.hpp"

using boost::multiprecision::mpz_int;


/**
 * \brief NNF Node type
 * \details
 * And represents logical conjunction.
 * Or represents logical disjunction.
 * Unary represents an Or (or And) node with a unique child.
 * True and False are special nodes without children that represent the
 * true and false constants.
 */
enum class NodeType {
    And,
    Or,
    Unary,
    True,
    False
};

/**
 * \brief an Edge object connecting NNF Nodes
 */
struct Edge {
    /**
     * \brief The memory pool used for free variables and units on Edges
     */
    static std::pmr::unsynchronized_pool_resource MEM_POOL;

    /**
     * \brief id_dst contains the node id of the target node for this edge.
     */
    std::size_t id_dst;

    /**
     * \brief Contains the free (i.e., unconstrained) variables of this edge.
     */
    std::pmr::vector<Variable> freeVars{&MEM_POOL};

    /**
     * \brief Contains the constrained (i.e., forced) literals of this edge.
     */
    std::pmr::vector<Literal> units{&MEM_POOL};
};

/**
 * \brief Represents a generic node for the NNF
 */
struct Node {
    /**
     * \brief Tells us which type of node it is.
     */
    NodeType type;

    /**
     * \brief This field contains the outgoing edges for this node, if any.
     */
    std::vector<Edge> children;

    Node() {}
};

/**
 * \brief A basic class to represent NNFs
 */
class NNF {
private:
    std::size_t nb_vars;
    std::vector<Node> nodes;
    std::vector<std::size_t> ordering;

    void compute_ordering();
public:

    /**
     * \brief The id of the ROOT node in an NNF
     */
    static std::size_t constexpr ROOT = 1;

    NNF() = default;
    /**
     * \brief Construct the NNF object from a file.
     * \param path Path to the NNF file.
     */
    NNF(std::string const& path);
    /**
     * \brief Construct the NNF object from a file.
     * \param path Path to the NNF file.
     * \param pvar The set of variables to keep. Any other variable will be discarded.
     */
    NNF(std::string const& path, std::set<Variable> const& pvar);

    /**
     * \brief Basic function which initialized the NNF. This function is called by the
     * constructors.
     */
    void init(std::string const& path, bool project, std::set<Variable> const& pvar);

    /**
     * \returns The number of nodes in the NNF.
     */
    std::size_t nb_nodes() const;

    /**
     * \returns The number of edges in the NNF.
     */
    std::size_t nb_edges() const;

    /**
     * \returns A reference to the Node object associated with id i
     * \pre 0 <= i < nb_nodes()
     */
    Node& operator[](std::size_t i) {
        return nodes[i];
    }

    /**
     * \returns A constant reference to the Node object associated with id i
     * \pre 0 <= i < nb_nodes()
     */
    Node const& operator[](std::size_t i) const {
        return nodes[i];
    }

    /**
     * \returns A constant reference to an array containing an ordering on the node ids.
     * If the Nodes are walked in the order given in the array, then the children
     * will be visited before the parents. Therefore, this can be used to do a
     * bottom-up traversal of the full NNF without needing to rely on recusion
     * and without having to worry about multiple visits of nodes.
     */
    inline std::vector<std::size_t> const& get_ordering() const { return ordering; }
};

/**
 * \brief A basic class to represent an annotated NNF
 * \details
 * The NNF can be annotated either with the model counts or with
 * the path counts.
 * Note that the NNF given as input is given through a constant reference.
 * The NNF is not modified or copied, the map node id -> annotation is kept
 * withing ANNF. Therefore multiple threads can reference the NNF
 * but have different ANNFs for different annotations under different assumptions.
 */
class ANNF {
private:
    std::vector<mpz_int> mcv;
    NNF const& nnf;
    std::set<Literal> assumps;

    /**
     * \returns The model count for a given edge
     * \pre The node targeted by e needs to have been annotated
     */
    mpz_int get_mc(Edge const& e) const;

    /**
     * \returns The path count for a given edge
     * \pre The node targeted by e needs to have been annotated
     */
    mpz_int get_pc(Edge const& e) const;
public:
    /**
     * \param f A constant reference to the NNF that will be anotated.
     */
    ANNF(NNF const& f);
    // ANNF(NNF const& f, std::set<Literal> const& a);

    /**
     * \brief Annotates the NNF with the model counts.
     */
    void annotate_mc();

    /**
     * \brief Annotates the NNF with the path counts.
     */
    void annotate_pc();

    /**
     * \brief Sets the list of assumptions
     * \details This can be used to count the number of solutions to the formula
     * under different partial variable assignments.
     */
    void set_assumps(std::set<Literal> const& a);

    /**
     * \brief Sets the list of assumptions
     * \details This can be used to count the number of solutions to the formula
     * under different partial variable assignments.
     */
    void set_assumps(std::vector<Literal> const& a);

    /**
     * \returns The path count or model count (depending on annotation)
     * of the node with the given id
     */
    mpz_int mc(std::size_t id) const;

    /**
     * \brief Once annotated, the paths are mapped to the set of integers
     * between 1 and the number of paths. Therefore, with a path id,
     * a path can be returned. Two different ids will give two different
     * partial assignments.
     */
    void get_path(mpz_int const& id, std::set<Literal> & s) const;

    /**
     * \brief Once annotated, the models are mapped to the set of integers
     * between 1 and the number of models. Therefore, with a model id,
     * a model can be returned. Two different ids will give two different
     * models.
     */
    void get_solution(mpz_int const& id, std::set<Literal> & s) const;
};


#endif //CNNF_NNF_HPP
