//
// Created by oz on 1/12/22.
//

#ifndef BN_CNF_HPP
#define BN_CNF_HPP

#include<set>
#include<vector>
#include<map>
#include<ostream>

//using Literal = int;
struct Literal;
struct Variable;

/**
 * \brief represents a Literal such as -1 or 1.
 * \details
 * Literals are represented as integers with the low significant bit representing the sign.
 * There for the literal '1' is encoded as '0' and '-1' is encoded as '1'.
 * Similarily, '2' becomes '2' and '-2' becomes '3'.
 * This allows us to use the value as an array index because the values start at 0.
 * It also has the advantage that a literal and its negation have similar encodings.
 *
 * The exact encoding for the literal a is: ((abs(a) - 1) * 2) + (1 if sign(a) == -1, 0 otherwise)
 */
struct Literal {
    private:
    int l;

    public:
    /**
     * \brief converts the input variable to a positive literal
     */
    Literal (Variable v);

    /**
     * \brief converts the input variable to a literal with the given sign
     * \param v
     * \param sign either 1 or -1
     */
    Literal (Variable v, int sign);

    /**
     * \brief converts the input literal in DIMACS format to our internal representation
     */
    Literal (int i);

    Literal(Literal const& v) = default;
    Literal(Literal && v) = default;

    Literal& operator= (Literal const& p) = default;
    Literal& operator= (Literal && p) = default;

    /**
     * \return the sign (-1 or 1) of this literal
     */
    inline int sign() const { return l & 1 ? -1 : 1; }

    /**
     * \return this literal in DIMACS format
     */
    inline int to_int() const { return sign() * ((l >> 1) + 1); }

    /**
     * \return the internal representation as an int for use as
     *  an array index
     */
    inline int get() const { return l; }

    bool operator == (Literal p) const;
    bool operator != (Literal p) const;

    /**
     * \returns
     * true if this.get() < p.get().
     *
     * as an example we have in DIMACS format:
     * 1 < -1 < 2 < -2 < 3 < -3 < ...
     */
    bool operator <  (Literal p) const; // '<' makes p, ~p adjacent in the ordering.  

    friend inline Literal operator ~ (Literal p);
};

/**
 * \returns the logical negation of this literal
 * \relates Literal
 */
inline Literal operator ~ (Literal p) {
    p.l ^= 1;
    return p;
}

/**
 * \brief prints this literal in DIMACS format to the stream
 * \relates Literal
 */
inline std::ostream & operator<<(std::ostream & out, Literal const& l) {
    out << l.to_int();
    //out << l.get();
    return out;
}

/**
 * \brief represents a variable
 * \details
 * Variables are encoded as integers starting at 0.
 * A DIMACS literal a is converted to (abs(a) - 1).
 *
 * Therefore, '-1' becomes '0' and '1' becomes '0'.
 */
struct Variable {
    private:
    int v;

    public:
    
    /**
     * \brief Converts a DIMACS literal to a variable.
     */
    Variable(int i);

    /**
     * \brief Converts our literal representation to a variable.
     */
    Variable(Literal l);

    Variable(Variable const& v) = default;
    Variable(Variable && v) = default;

    Variable& operator= (Variable const& p) = default;
    Variable& operator= (Variable && p) = default;

    /**
     * \returns The internal representation of this variable for use
     * as an array index.
     */
    inline int to_int() const { return v + 1; }

    /**
     * \returns A positive literal representation of this variable in DIMACS format
     */
    inline int get() const { return v; }

    bool operator == (Variable p) const;
    bool operator != (Variable p) const;

    /**
     * \returns
     * true if this.to_int() < p.to_int()
     */
    bool operator <  (Variable p) const;
};

/**
 * \brief Prints the variable in DIMACS format to the stream
 * \relates Variable
 */
inline std::ostream & operator<<(std::ostream & out, Variable const& v) {
    out << v.to_int();
    return out;
}

/**
 * \brief Represents a clause as a list of literals
 */
struct Clause {
    private:
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
     * \brief add a literal to the clause if it is not already present
     */
    void push(Literal const& l);

    /**
     * \brief remove the literal from the clause
     */
    void remove(Literal const& l);

    /**
     * \brief remove every occurence of the varliable v from the clause
     * \details
     * removes the literals v and ~v from the clause;
     */
    void remove(Variable const& v);

    /**
     * \returns true if the clause contains l
     */
    bool contains(Literal const& l) const;

    /**
     * \returns true if the clause contains very literal in the clause cls
     */
    bool contains(Clause const& cls) const;

    /**
     * \returns an iterator to the beginning of the clause
     */
    inline auto begin() const {
        return c.begin();
    }

    /**
     * \returns an iterator to the end of the clause (past the end iterator)
     */
    inline auto end() const {
        return c.end();
    }

    /**
     * \returns the number of literals in the clause
     */
    inline std::size_t size() const {
        return c.size();
    }

    /**
     * \returns a reference to the literal at index i
     */
    inline auto& operator[](std::size_t const& i) {
        return c[i];
    }

    /**
     * \returns a constant reference to the literal at index i
     */
    inline auto const& operator[](std::size_t const& i) const {
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
 * \brief represents a propositional formula in CNF form
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

        std::size_t nb_active = 0;

    public:
        CNF() = default;

        /**
         * \brief reads a formula from a DIMACS file located at path
         * \param path path to the file containing the formula in DIMACS format
         */
        CNF(char const* path);
        CNF(CNF const& c) = default;
        CNF(CNF && c) = default;

        CNF& operator=(CNF const& c) = default;
        CNF& operator=(CNF && c) = default;

        /**
         * \brief computes the set of unconstrained variables (i.e., that do no appear in the formula syntactically)
         * \details the result is stored in a class member
         */
        void compute_free_vars();

        /**
         * \brief applies boolean constraint propagation to the formula
         */
        void simplify();

        /**
         * \brief removes clauses based on subsumption
         * \details
         * If we have two clauses A and B, and we have A.contains(B), then we know that A is a logical implication of B.
         *
         * Therefore, A is redundant as it will always be true if B is true and we can safely remove A from the formula.
         */
        void subsumption();

        std::map<Variable, std::size_t> get_reduced_mapping() const;

        std::vector<std::size_t> get_nb_by_clause_len() const;
        std::vector<std::set<Variable> > get_vars_by_clause_len() const;

        /**
         * \details
         * Because of simplify() and subsumption(), not every clause id refers to an active id.
         * \pre i must satisfy 0 <= i < nb_clauses()
         * \returns true if the clause id refers to an active clause, false otherwise
         */
        inline bool is_active(std::size_t i) const { return active[i]; }

        /**
         * \brief Sets the active property for a clause id
         * 
         * \pre i must satisfy 0 <= i < nb_clauses()
         */
        inline void set_active(std::size_t i, bool v) { 
            if(active[i] && !v) {
                nb_active--;
            }
            else if(!active[i] && v) {
                nb_active++;
            }
            active[i] = v;
        }

        /**
         * \pre i must satisfy 0 <= i < nb_clauses()
         * \returns the clause corresponding to id i
         */
        inline Clause const& clause(std::size_t i) const { return clauses[i]; }

        /**
         * \pre l must be a valid literal for the formula (0 <= l.get() < nb_vars() * 2)
         * \returns the set of clause ids that contain the literal l
         */
        inline std::set<std::size_t> const& get_idx(Literal l) const { return idx[l.get()]; }

        /**
         * \returns the number of variables 
         */
        inline std::size_t nb_vars() const { return vars.size(); }

        /**
         * \returns the number of variables that do not appear syntactically in the formula
         * if compute_free_vars() has been called before
         */
        inline std::size_t nb_free_vars() const { return free.size(); }

        /**
         * \returns the number of unit clauses (containing only one literal)
         * if simplify() has been called before
         */
        inline std::size_t nb_units() const { return units.size(); }

        /**
         * \returns the number of variables that appear syntactically in the formula
         * if compute_free_vars() has been called before
         */
        inline std::size_t nb_c_vars() const { return nb_vars() - nb_free_vars(); }

        /**
         * \returns the total number of clauses
         */
        inline std::size_t nb_clauses() const { return clauses.size(); }

        /**
         * \returns the number of clauses that are active
         */
        inline std::size_t nb_active_clauses() const { return nb_active; }

        /**
         * \returns the clauses that have all their variables in the set v
         *
         * clause length has to be >= m_len to be included
         */
        std::vector<Clause> get_clauses_by_vars(std::set<Variable> const& v, std::size_t m_len) const;

        /**
         * \returns the clauses that have at least one of their variables in the set v
         *
         * clause length has to be >= m_len to be included
         */
        std::vector<Clause> get_clauses_by_vars_wide(std::set<Variable> const& v, std::size_t m_len) const;

    friend std::ostream & operator<<(std::ostream & out, CNF const& cnf);
};

/**
 * \brief Prints the formula in DIMACS format to the stream
 * \relates CNF
 */
std::ostream & operator<<(std::ostream & out, CNF const& cnf);

#endif //BN_CNF_HPP
