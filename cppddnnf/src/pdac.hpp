#ifndef PDAC_HPP
#define PDAC_HPP

#include <set>

/**
 * \details
 * This datastructure is used to store the necessary information to run the algorithms
 * described in our paper: DivKC: A Divide-and-Conquer Approach to Knowledge Compilation.
 *
 * Here, pnnf represents G_P and unnf G_U.
 * The field pvar contains the variables that appear in G_P (pnnf).
 */
struct PDAC {
    std::set<Variable> pvar;
    NNF pnnf;
    NNF unnf;
};

/**
 * \brief Read a PDAC from the files.
 * \param path A path to a DIMACS CNF file. (example: t.cnf)
 * \details
 * This function expects multiple files to exist:
 * - path + ".path.log"
 * - path + ".pnnf"
 * - path + ".unnf"
 *
 * \relates PDAC
 */
PDAC pdac_from_file(std::string const& path);

#endif
