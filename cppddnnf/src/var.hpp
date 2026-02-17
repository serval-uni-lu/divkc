#ifndef VAR_HPP
#define VAR_HPP

#include <iostream>

#include <boost/dynamic_bitset.hpp>

struct Literal;
struct Variable;

/**
 * \brief Represents a Literal such as -1 or 1.
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
     * \brief Converts the input variable to a positive literal
     */
    Literal (Variable v);

    /**
     * \brief Converts the input variable to a literal with the given sign
     * \param v
     * \param sign either 1 or -1
     */
    Literal (Variable v, int sign);

    /**
     * \brief Converts the input literal in DIMACS format to our internal representation
     */
    Literal (int i);

    Literal(Literal const& v) = default;
    Literal(Literal && v) = default;

    Literal& operator= (Literal const& p) = default;
    Literal& operator= (Literal && p) = default;

    /**
     * \returns The sign (-1 or 1) of this literal
     */
    inline int sign() const { return l & 1 ? -1 : 1; }

    /**
     * \returns This literal in DIMACS format
     */
    inline int to_int() const { return sign() * ((l >> 1) + 1); }

    /**
     * \returns The internal representation as an int for use as
     *  an array index
     */
    inline int get() const { return l; }

    friend inline Literal operator ~ (Literal p);
};

namespace std {
    template<>
    struct hash<Literal> {
        std::size_t operator()(Literal const& l) const noexcept {
            return std::hash<int>()(l.get());
        }
    };
}

/**
 * \returns
 * true if a.get() < b.get().
 *
 * as an example we have in DIMACS format:
 * 1 < -1 < 2 < -2 < 3 < -3 < ...
 *
 * \relates Literal
 */
inline bool operator<(Literal const& a, Literal const& b) {
    return a.get() < b.get();
}

/**
 * \relates Literal
 */
inline bool operator>(Literal const& a, Literal const& b) {
    return a.get() > b.get();
}

/**
 * \relates Literal
 */
inline bool operator==(Literal const& a, Literal const& b) {
    return a.get() == b.get();
}

/**
 * \relates Literal
 */
inline bool operator>=(Literal const& a, Literal const& b) {
    return a > b || a == b;
}

/**
 * \returns
 * true if a.get() <= b.get().
 *
 * \relates Literal
 */
inline bool operator<=(Literal const& a, Literal const& b) {
    return a < b || a == b;
}

/**
 * \relates Literal
 */
inline bool operator!=(Literal const& a, Literal const& b) {
    return a.get() != b.get();
}

/**
 * \returns The logical negation of this literal
 * \relates Literal
 */
inline Literal operator ~ (Literal p) {
    p.l ^= 1;
    return p;
}

/**
 * \brief Prints this literal in DIMACS format to the stream
 * \relates Literal
 */
inline std::ostream & operator<<(std::ostream & out, Literal const& l) {
    out << l.to_int();
    //out << l.get();
    return out;
}

/**
 * \brief Represents a variable
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
     * \returns A positive literal representation of this variable in DIMACS format
     */
    inline int to_int() const { return v + 1; }

    /**
     * \returns The internal representation of this variable for use
     * as an array index.
     */
    inline int get() const { return v; }
};

namespace std {
    template<>
    struct hash<Variable> {
        std::size_t operator()(Variable const& v) const noexcept {
            return std::hash<int>()(v.get());
        }
    };
}

/**
 * \returns
 * true if a.get() < b.get().
 *
 * \relates Variable
 */
inline bool operator<(Variable const& a, Variable const& b) {
    return a.get() < b.get();
}

/**
 * \relates Variable
 */
inline bool operator>(Variable const& a, Variable const& b) {
    return a.get() > b.get();
}

/**
 * \relates Variable
 */
inline bool operator==(Variable const& a, Variable const& b) {
    return a.get() == b.get();
}

/**
 * \relates Variable
 */
inline bool operator>=(Variable const& a, Variable const& b) {
    return a > b || a == b;
}

/**
 * \returns
 * true if a.get() <= b.get().
 *
 * \relates Variable
 */
inline bool operator<=(Variable const& a, Variable const& b) {
    return a < b || a == b;
}

/**
 * \relates Variable
 */
inline bool operator!=(Variable const& a, Variable const& b) {
    return a.get() != b.get();
}

/**
 * \brief A set of literals using bitsets for fast insertion and lookup.
 * \details
 * This is not a multiset, therefore, elements have at most one occurence
 * in the set.
 */
struct LitSet {
    private:
    boost::dynamic_bitset<> set;

    public:
    /**
     * \brief Creates an empty set with a preallocated bitset.
     */
    LitSet(std::size_t const nb_lits);
    LitSet();

    LitSet(LitSet const& v) = default;
    LitSet(LitSet && v) = default;

    LitSet& operator= (LitSet const& p) = default;
    LitSet& operator= (LitSet && p) = default;

    /**
     * \brief Insert a literal into the set.
     */
    void insert(Literal const& l);
    /**
     * \brief Remove a literal from the set.
     */
    void remove(Literal const& l);
    /**
     * \brief Remove every literal from the set.
     */
    void reset();

    /**
     * \returns true if the literal is in the set, false otherwise.
     */
    bool contains(Literal const& l) const;

    /**
     * \returns The number of literals in the set.
     */
    inline std::size_t count() const { return set.count(); }

    /**
     * \brief Resizes the bitset to accomodate literals that may have been outside of the range.
     * \details
     * If 2*nb_lits is smaller than the current size of the bitset (set_size()),
     * then information may be lost.
     */
    void resize(std::size_t const nb_lits);

    /**
     * \returns The size of the underlying bitset.
     */
    inline std::size_t set_size() const { return set.size(); }

    friend bool operator == (LitSet const& a, LitSet const& b);
    friend bool operator<(LitSet const& l, LitSet const& r);
};

#endif
