//
// Created by oz on 1/12/22.
//

#ifndef BN_CNF_HPP
#define BN_CNF_HPP

#include<set>
#include<vector>
#include<map>
#include<ostream>

#include "var.hpp"

/**
 * \brief Represents a clause as a list of literals
 */
struct Clause {
    private:
    //std::set<Literal> c;
    std::vector<Literal> c;

    public:
    /**
     * \brief Construct an empty clause
     */
    Clause();
    Clause(Clause const& c) = default;
    Clause(Clause && c) = default;

    Clause& operator=(Clause const& c) = default;
    Clause& operator=(Clause && c) = default;

    /**
     * \brief Add a literal to the clause if it is not already present
     */
    void push(Literal const& l);

    /**
     * \brief Remove the literal from the clause
     */
    void remove(Literal const& l);

    /**
     * \brief Remove every occurence of the varliable v from the clause
     * \details
     * removes the literals v and ~v from the clause;
     */
    void remove(Variable const& v);

    /**
     * \returns True if the clause contains l
     */
    bool contains(Literal const& l) const;

    /**
     * \returns True if the clause contains very literal in the clause cls
     */
    bool contains(Clause const& cls) const;

    /**
     * \returns An iterator to the beginning of the clause
     */
    inline auto begin() const {
        return c.begin();
    }

    /**
     * \returns An iterator to the end of the clause (past the end iterator)
     */
    inline auto end() const {
        return c.end();
    }

    /**
     * \returns The number of literals in the clause
     */
    inline std::size_t size() const {
        return c.size();
    }

    /**
     * \returns A reference to the literal at index i
     */
    inline auto& operator[](std::size_t const& i) {
        return c[i];
    }

    /**
     * \returns A constant reference to the literal at index i
     */
    inline auto const& operator[](std::size_t i) const {
        return c[i];
    }
};

/**
 * \brief Prints the clause in DIMACS format to the stream ('0' terminated)
 * \relates Clause
 */
inline std::ostream & operator<<(std::ostream & out, Clause const& c) {
    for(auto const& l : c) {
        out << l << " ";
    }
    out << "0";
    return out;
}

/**
 * \brief Represents a propositional formula in CNF form
 */
class CNF {
    private:
        std::vector<Clause> clauses;
        std::vector<bool> active;
        std::vector<std::set<std::size_t> > idx;

        std::set<Literal> units;
        std::set<Variable> free;

        std::set<Variable> vars;
        std::set<Variable> ind;
        std::set<Variable> prj;

        std::set<Variable> ign;
        std::set<Variable> dign;

        std::vector<std::size_t> available_ids;

        std::size_t nb_active = 0;

        void compute_idx();

    public:
        CNF() = default;
        CNF(std::size_t nbv);

        /**
         * \brief Reads a formula from a DIMACS file located at path
         * \param Path path to the file containing the formula in DIMACS format
         */
        CNF(char const* path);
        CNF(CNF const& c) = default;
        CNF(CNF && c) = default;

        CNF& operator=(CNF const& c) = default;
        CNF& operator=(CNF && c) = default;

        /**
         * \brief Computes the set of unconstrained variables (i.e., that do no appear in the formula syntactically)
         * \details The result is stored in a class member
         */
        void compute_free_vars();

        /**
         * \brief Applies boolean constraint propagation to the formula
         */
        void simplify();

        /**
         * \brief Removes clauses based on subsumption
         * \details
         * If we have two clauses A and B, and we have A.contains(B), then we know that A is a logical implication of B.
         *
         * Therefore, A is redundant as it will always be true if B is true and we can safely remove A from the formula.
         */
        void subsumption();

        /**
         * \brief Add the clause to the formula
         */
        void add_clause(Clause c);

        /**
         * \brief Add the clause only if there is no clause A such that c.contains(A)
         */
        void add_clause_nonredundant(Clause c);

        /**
         * \brief Removes the clause with the given id
         * \pre 0 <= id < nb_clauses()
         */
        void rm_clause(std::size_t id);

        /**
         * \brief Forget the variable v
         * \details
         * Perform resolution based forgetting (i.e., F = (F /\ v) \/ (F /\ ~v)
         */
        void forget(Variable v);

        /**
         * \brief Set the set of variables on which the formula will be projected
         * with the next call to project
         */
        void set_prj(std::set<Variable> const& v) { prj = v; }
        
        /**
         * \brief Project the formula with forget on the projection set.
         * Stops early if the timeout is reached (if the timeout is not set to -1)
         */
        void project(int const timeout = -1);

        /**
         * \returns The number of occurences of variable v
         */
        std::size_t occurrence_count(Variable v);

        /**
         * \returns The number of occurences of the positive literal
         * multiplied by the number of occurences of the negative lietaral.
         */
        std::size_t occurrence_product(Variable v);

        /**
         * \returns A new formula containing only the variables
         *  that appear syntactically. The variables get renamed to adhere to the DIMACS format.
         */
        CNF rename_vars();

        /**
         * \returns The true set of variables on which the formula has been projected
         * on the previous call to project(). It can differ from the set given to
         * set_prj() due to timeouts.
         */
        std::set<Variable> compute_true_projection() const;

        /**
         * \brief Removes clauses that contain variables outside of v.
         * \returns The set of variables actually appearing in the remaining clauses.
         */
        std::set<Variable> inplace_upper_bound(std::set<Variable> const& v);

        /**
         * \brief Sets the projectionset to the entire set of variables
         */
        void reset_prj() {
            prj = vars;
            ign.clear();
            dign.clear();
        }

        std::vector<std::size_t> get_nb_by_clause_len() const;
        std::vector<std::set<Variable> > get_vars_by_clause_len() const;

        /**
         * \returns The number of variables 
         */
        inline std::size_t nb_vars() const { return vars.size(); }

        /**
         * \returns The number of variables that do not appear syntactically in the formula
         * if compute_free_vars() has been called before
         */
        inline std::size_t nb_free_vars() const { return free.size(); }

        /**
         * \returns The number of unit clauses (containing only one literal)
         * if simplify() has been called before
         */
        inline std::size_t nb_units() const { return units.size(); }

        /**
         * \returns The number of variables that appear syntactically in the formula
         * if compute_free_vars() has been called before
         */
        inline std::size_t nb_c_vars() const { return nb_vars() - nb_free_vars(); }

        /**
         * \returns The total number of clauses
         */
        inline std::size_t nb_clauses() const { return clauses.size(); }

        /**
         * \returns The number of clauses that are active
         */
        inline std::size_t nb_active_clauses() const { return nb_active; }

    friend std::ostream & operator<<(std::ostream & out, CNF const& cnf);
};

/**
 * \brief Prints the formula in DIMACS format to the stream
 * \relates CNF
 */
std::ostream & operator<<(std::ostream & out, CNF const& cnf);

#endif //BN_CNF_HPP
